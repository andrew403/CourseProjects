//=============================================================================
// FILE:
//    BranchPointerTrace.cpp
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
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/GlobalVariable.h"

using namespace llvm;

//-----------------------------------------------------------------------------
// BranchPointerTrace implementation
//-----------------------------------------------------------------------------
// No need to expose the internals of the pass to the outside world - keep
// everything in an anonymous namespace.
namespace {

// variable for BranchCounter
GlobalVariable *BranchCounterVar = nullptr;

void declareBranchCounter(Module &M) {
  /**
   * Declare a global variable named "counter", for tracing the usage of branches
   * print out the result at the end
   */
  IRBuilder<> Builder(M.getContext());  
  Type *Int32Ty = Builder.getInt32Ty();
  BranchCounterVar = new GlobalVariable(
      M, Int32Ty, false, GlobalValue::ExternalLinkage, 
      ConstantInt::get(Int32Ty, 0), "counter");  
}

void counterIncrement(BasicBlock &BB) {
  /**
   * Insert counter++ while we met branches
   */
  IRBuilder<> Builder(&BB, BB.begin());
  Value *CurrentCounter = Builder.CreateLoad(Builder.getInt32Ty(), BranchCounterVar);
  Value *UpdatedCounter = Builder.CreateAdd(CurrentCounter, Builder.getInt32(1));
  Builder.CreateStore(UpdatedCounter, BranchCounterVar);
}

void insertPrintf(Function &F, BasicBlock &BB, int BranchInfoCounter) {
  /**
   * Insert printf in the .ll file to trace branches
   */
  IRBuilder<> Builder(&BB, BB.begin());
  std::string BranchID = "br_" + std::to_string(BranchInfoCounter);
  // StringRef FileName = F.getParent()->getSourceFileName();
  // Create the function type for printf
  FunctionType *PrintfType = FunctionType::get(Builder.getInt32Ty(), // Return type
                                              Builder.getInt8Ty()->getPointerTo(), // Format string type
                                              true);
  // Get or declare the printf function
  FunctionCallee PrintfFunc = F.getParent()->getOrInsertFunction("printf", PrintfType);
  // Create the format string
  Value *FormatString = Builder.CreateGlobalStringPtr(BranchID+"\n");
  // Create the call to printf
  Builder.CreateCall(PrintfFunc, {FormatString});
}


void storeBranchInfo(int BranchInfoCounter, Twine filename, int lineStatement, int lineGoto){
  /**
   * Store branch information while we met new branches, including filename and line number.
   */
  std::string branchInfo = "br_" + std::to_string(BranchInfoCounter) + ": " + filename.str() + ", " + std::to_string(lineStatement) + ", " + std::to_string(lineGoto);
  // errs() << branchInfo << "\n";

  // store into file
  std::string outFileName = filename.str().substr(0, (filename.str()).length() - 2) + "_BranchDictionary.txt";
  std::ofstream outFile;
  outFile.open(outFileName, std::ios::app);
  outFile << branchInfo << std::endl;
}

void traceBranch(Module &M, int BranchInfoCounter) {
  /**
   * Main function for tracing branches
   * Go through all functions and instruction to find out all conditional branches
   */
  declareBranchCounter(M);   // set a global variable .ll file for counting the branches
  for (Function &F : M){
    if (!F.isDeclaration()){
      errs() << "*func_" << &F << " -> " << F.getName() <<"\n";  // Print the address of F
    }
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        if (auto *Branch = dyn_cast<BranchInst>(&I)) {
          const DebugLoc &statement = I.getDebugLoc();
          if (Branch -> isConditional()) {        

            // true successor
            BasicBlock *TrueSuccessor = Branch -> getSuccessor(0);
            const DebugLoc &trueSucc = TrueSuccessor -> getFirstNonPHIOrDbgOrLifetime() -> getDebugLoc();
            if (trueSucc -> getLine() != statement -> getLine()) {
              // store branch information into file
              storeBranchInfo(BranchInfoCounter, statement->getFilename(), statement->getLine(), trueSucc->getLine());
              // insert counter increment and printf at the start of the true successor
              counterIncrement(*TrueSuccessor);
              insertPrintf(F, *TrueSuccessor, BranchInfoCounter);
              ++BranchInfoCounter;
              
              // false successor
              BasicBlock *FalseSuccessor = Branch -> getSuccessor(1);
              const DebugLoc &falseSucc = FalseSuccessor -> getFirstNonPHIOrDbgOrLifetime() ->getDebugLoc();
              // store branch information into file
              storeBranchInfo(BranchInfoCounter, statement->getFilename(), statement->getLine(), falseSucc->getLine());
              // insert counter increment and printf at the start of the false successor
              counterIncrement(*FalseSuccessor);
              insertPrintf(F, *FalseSuccessor, BranchInfoCounter);
              ++BranchInfoCounter;
            }
          } 
        }
      }
    }
  }
}

void printResult(Module &M) {
  /**
   * Insert printf before the terminator of last basicblock of main function,
   * so that we can make sure trace record all branches
   */
  IRBuilder<> Builder(M.getContext());
  Function *MainFunction = M.getFunction("main");
  BasicBlock *lastBB = nullptr;
  for (BasicBlock &BB : *MainFunction) {
    lastBB = &BB;
  }
  // BasicBlock &EntryBlock = MainFunction->getEntryBlock();
  Instruction *Terminator = lastBB->getTerminator();
  Builder.SetInsertPoint(lastBB, Terminator->getIterator());
  FunctionType *PrintfType = FunctionType::get(Builder.getInt32Ty(), // Return type
                                              Builder.getInt8Ty()->getPointerTo(), // Format string type
                                              true);
  // Get or declare the printf function
  FunctionCallee PrintfFunc = M.getOrInsertFunction("printf", PrintfType);
  Value *CurrentCounter = Builder.CreateLoad(Builder.getInt32Ty(), BranchCounterVar);
  // Create the format string
  Value *FormatString = Builder.CreateGlobalStringPtr("Conditional Branch Count: %d\n");
  // Create the call to printf
  Builder.CreateCall(PrintfFunc, {FormatString, CurrentCounter});
}

// New PM implementation
struct BranchPointerTrace : PassInfoMixin<BranchPointerTrace> {
  // Main entry point, takes IR unit to run the pass on (&F) and the
  // corresponding pass manager (to be queried if need be)
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &) {
    int BranchInfoCounter = 0;
    traceBranch(M, BranchInfoCounter);
    // printResult(M);
    return PreservedAnalyses::all();
  }

  // Without isRequired returning true, this pass will be skipped for functions
  // decorated with the optnone LLVM attribute. Note that clang -O0 decorates
  // all functions with optnone.
  static bool isRequired() { return true; }
};
} // namespace

//-----------------------------------------------------------------------------
// New PM Registration
//-----------------------------------------------------------------------------
llvm::PassPluginLibraryInfo getBranchPointerTracePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "BranchPointerTrace", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "branch-pointer-trace") {
                    MPM.addPass(BranchPointerTrace());
                    return true;
                  }
                  return false;
                });
          }};
}

// This is the core interface for pass plugins. It guarantees that 'opt' will
// be able to recognize BranchPointerTrace when added to the pass pipeline on the
// command line, i.e. via '-passes=branch-pointer-trace'
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getBranchPointerTracePluginInfo();
}