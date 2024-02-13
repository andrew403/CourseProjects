//=============================================================================
// FILE:
//    FeatureDetection.cpp
//
// DESCRIPTION:
//    Visits all functions in a module, prints their names and the number of
//    arguments via stderr. Strictly speaking, this is an analysis pass (i.e.
//    the functions are not modified). However, in order to keep things simple
//    there's no 'print' method here (every analysis pass should implement it).
//
// USAGE:
//    1. New PM
//      opt -load-pass-plugin=libBranchPointerTrace.dylib -passes="branch-pointer-trace" `\`
//        -disable-output <input-llvm-file>
//
//
// License: MIT
//=============================================================================
#include <fstream>
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/ADT/StringRef.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// FeatureDetection implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {


bool isFunctionTakingInput(Function *F) {
  if (!F) {
      return false;
  }
  if (F -> getName().contains("scanf")) {
      return true;
  }
  return false;
}

void FindKeyPoint(Function &F){
  std::vector <std::vector<std::string>> InputDefs;
  std::map<std::string, std::string> DeclareMap;
	errs() << "======== Analyzing function: " << F.getName() <<" ========"<< "\n";
  std::set<Value *> inputFeatures;
  for (BasicBlock &BB : F) {
    for (Instruction &I : BB) {

      if (auto *dbgDeclare = dyn_cast<DbgDeclareInst>(&I)) {
        // Get the variable associated with the dbg.declare instruction.
        DIVariable *var = dbgDeclare->getVariable();
        std::string DeclareStr;
        raw_string_ostream stream(DeclareStr);
        stream << *dbgDeclare;
        if (var) {
          // Retrieve the variable name.
          StringRef varName = var->getName();

          for(int i = 0; i < DeclareStr.length(); i++){
            if(DeclareStr[i] == '%'){ // To find the index
              DeclareStr = DeclareStr.substr(i, 2);
              InputDefs.push_back({DeclareStr});
              errs() << "Declare Variable Name: " << varName << ", " << DeclareStr << "\n";
              DeclareMap.insert(std::pair < std::string, std::string> (DeclareStr, varName));
            }
          }
        }
      }

      if (auto *storeInst = dyn_cast<StoreInst>(&I)) {
          // Get store value
          Value *storedValue = storeInst->getValueOperand();
          // Get store pointer
          Value *storePointer = storeInst->getPointerOperand();
          std::string ValueStr;
          raw_string_ostream stream(ValueStr);
          stream << *storedValue;
          std::string PointerStr;
          raw_string_ostream stream2(PointerStr);
          stream2 << *storePointer;
          //errs() << "(*)Stored Value: " << ValueStr;
          //errs() << ", Stored Pointer: " << PointerStr << "\n";
          for(int j=0; j < PointerStr.length(); j++){
                if(PointerStr[j]== '='){
                  PointerStr = PointerStr.substr(2, j-3);
                  //errs()<<"LOADPTRSTR"<<PointerStr<<"ED"<<"\n";
                  break;
                }
              }
          for(int j=0; j < ValueStr.length(); j++){
                if(ValueStr[j]== '='){
                  ValueStr = ValueStr.substr(2, j-3);
                  //errs()<<"LOADVALSTR"<<ValueStr<<"ED"<<"\n";
                  break;
                }
              }

          bool Endloop = false;
          for(int i = 0; i < InputDefs.size(); i++){
            for(int k = 0; k < InputDefs[i].size(); k++){
              if(ValueStr == InputDefs[i][k] && k!= InputDefs[i].size()-1){
                std::vector <std::string> newRow(InputDefs[i].begin(), InputDefs[i].begin() + k + 1);
                InputDefs.push_back(newRow);
                
                // errs()<<"(*)New branch ";
                // errs()<< ValueStr<<"Updated queue from: "<<InputDefs[i][k]<<" to "<<PointerStr<<"\n";
                InputDefs[InputDefs.size()-1].push_back(PointerStr);
                Endloop = true;
                break;
              }
              else if(ValueStr == InputDefs[i][k] && k== InputDefs[i].size()-1){
                
                // errs()<<"(*)Updated queue from: "<<InputDefs[i][InputDefs[i].size()-1]<<" to "<<PointerStr<<"\n";
                InputDefs[i].push_back(PointerStr);
                Endloop = true;
                break;
              }
            }
          if(Endloop)
            break;
        }

          
        }


      if (auto *CI = dyn_cast<CallInst>(&I)) {
        // Check if the call is to a function that takes user input
        if (isFunctionTakingInput(CI->getCalledFunction()) && CI->getCalledFunction()->getName() == "__isoc99_scanf") {
          std::string CIStr;
          raw_string_ostream stream(CIStr);
          stream << *CI;

          // errs() << "(*)Found call to function taking input: " << CIStr << "\n";

          for (Use &U : CI->operands()) {
            if (auto *defInst = dyn_cast<Instruction>(U)) {
              std::string CIStr;
              raw_string_ostream stream(CIStr);
              stream << *defInst;
              // errs() << "(*)Found definition of : " << CIStr << "\n";
              for(int i=0; i < CIStr.length(); i++){
                if(CIStr[i]== '='){ // To find the index
                  CIStr = CIStr.substr(2, i-3);
                  InputDefs.push_back({CIStr});
                  // errs()<<"(*)Store "<<CIStr<<" to vector"<<"\n";
                  break;
                }
              }
            }
          }
          
        }
      }

      if (auto *LI = dyn_cast<LoadInst>(&I)) {
        Value *loadedValue = LI->getPointerOperand();
        std::string LoadStr;
        raw_string_ostream stream(LoadStr);
        stream << *LI;

        // errs() << "(*)Found Load instruction: " << LoadStr << "\n";
        std::string LoadToStr;
        std::string LoadFromStr;
        for(int j=0; j < LoadStr.length(); j++){
                if(LoadStr[j]== '='){
                  LoadToStr = LoadStr.substr(2, j-3);
                  //errs()<<"LOADSBSTR"<<LoadStr<<"\n";
                  break;
                }
              }
          
        for(int i = LoadStr.length()-1; i>=0; i--){
          if(LoadStr[i] == '%'){
            std::string temp = "";
            for(int j = i; j< LoadStr.length()-1; j++){
              if(LoadStr[j]!=',')
                temp += LoadStr[j];
              else
                break;
            }
            LoadFromStr = temp;
            break;
            //errs() <<"SUBSTRICMP = "<< icmpString<<"ED"<<"\n";
          }
        }
        bool Endloop = false;

        for(int i = 0; i < InputDefs.size(); i++){
          int Isize = InputDefs[i].size()-1;
          for(int k = 0; k < InputDefs[i].size(); k++){
            if(LoadFromStr == InputDefs[i][k] && k!= InputDefs[i].size()-1 ){
              std::vector <std::string> newRow(InputDefs[i].begin(), InputDefs[i].begin() + k + 1);
              InputDefs.push_back(newRow);
              
              // errs()<<"(*)New branch ";
              // errs()<<"Updated queue from: "<<InputDefs[InputDefs.size()-1][InputDefs[InputDefs.size()-1].size()-1]<<" to "<<LoadToStr<<"\n";
              InputDefs[InputDefs.size()-1].push_back(LoadToStr);
              Endloop = true;
              break;
            }
            else if(LoadFromStr == InputDefs[i][k] && k == InputDefs[i].size()-1 ){
              
              // errs()<<"(*)Updated queue from: "<<InputDefs[i][InputDefs[i].size()-1]<<" to "<<LoadToStr<<"\n";
              InputDefs[i].push_back(LoadToStr);
              Endloop = true;
              break;
            }
          }
          if(Endloop)
            break;
                       
        }
      }

      if (auto *icmpInst = dyn_cast<ICmpInst>(&I)) {
        std::string icmpString;
        raw_string_ostream icmpStream(icmpString);
        icmpStream << *icmpInst;
        icmpStream.flush();

        // errs() << "(*)ICMP Instruction: " << icmpString << "\n";

        for(int i = icmpString.length(); i>=0;i--){
          if(icmpString[i] == '%'){
            std::string temp = "";
            for(int j = i; j< icmpString.length()-1; j++){
              if(icmpString[j]!=',')
                temp += icmpString[j];
              else
                break;
            }
            icmpString = temp;
            //errs() <<"SUBSTRICMP = "<< icmpString<<"ED"<<"\n";
            break;
          }
        }

        for(int i = 0; i < InputDefs.size(); i++){
          if(icmpString == InputDefs[i][InputDefs[i].size()-1]){
            errs() << "Found key point: " << InputDefs[i][InputDefs[i].size()-1] << "\n";
            errs() << "Trace back: " ;
            // Trace back
            for(int j = InputDefs[i].size()-1; j>=0; j--){
              errs() << InputDefs[i][j] <<  " -> ";
            }
            errs() << "END." << "\n";

            std::map < std::string ,std::string > ::iterator it;
            std::map < std::string ,std::string > ::iterator itEnd;
            it = DeclareMap.begin();
            itEnd = DeclareMap.end();
            while (it != itEnd) {
              if(it->first == InputDefs[i][0]){
                errs() << "Key point variable name: " << it->second << "\n";
                break;
              }
              it++;
            }
            break;
          }
        }
      }
    }
  }
}
	
struct FeatureDetection : PassInfoMixin<FeatureDetection> {
    // Main entry point, takes IR unit to run the pass on (&F) and the
    // corresponding pass manager (to be queried if need be)
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
        FindKeyPoint(F);

        
        return PreservedAnalyses::all();
    }
    // Without isRequired returning true, this pass will be skipped for functions
    // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
    // all functions with optnone.
    static bool isRequired() { return true; }
};
}

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getFeatureDetectionPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "FeatureDetection", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "feature-detection") {
                    FPM.addPass(FeatureDetection());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize FeatureDetection when added to the pass pipeline on the
// command line, i.e. via '-passes=branch-pointer-trace'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getFeatureDetectionPluginInfo();
}