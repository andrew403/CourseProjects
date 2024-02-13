#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include<iostream>
#include<vector>
#include<ctime>
#include<time.h>
#include <thread>
#include<string>
#include <libplatform/libplatform.h>
#include <filesystem>
#include  <v8.h>
#include <utility>
#include "ScriptManager.h"
#include "v8helpers.h"
#define SM_DEBUG 0
#define RELOAD 0
#define V8H_DEBUG 1
#define GO_DEBUG 0

class Character {
    public:
    Character();
    virtual ~Character();
    void CharUpdate(sf::RenderTarget* Target, float dt);
    void CharRender(sf::RenderTarget* Target);
    void updateInput(sf::RenderTarget* Target, float dt);
    void updateWindowCollision(sf::RenderTarget* Target);
    float getCharPosX();
    float getCharPosY();
    void jump();
    void mvLeft(float v);
    void mvRight(float v);
    
    sf::Sprite CharShape; // The object of the character
    float spawnX = 0.f;
    float spawnY = 0.f;
    float boundaryMovement = 0;
    float positionX, positionY;     // Position of the character
    float velocityX = 0.f;
    float velocityY = 0.f;     // Velocity of the character
    float gravity = 0.5f;           // How strong is gravity
    bool isjumping = false;
    bool renderingCharacter = true;
    float velocity;
    // Parameters to store the original position of the character
    float tmp_boundarymovement = 0;
    float tmp_spawnX = 0;
    float tmp_spawnY = 0;
    float tmp_velocity = 0;

    /*std::string guid = "";
    int current_guid = 0;
    std::vector<Character*> game_objects;

    static void setGameObjectX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getGameObjectX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
	static void setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    void ScriptedCharacterFactory(const v8::FunctionCallbackInfo<v8::Value>& args);
    v8::Local<v8::Object> exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name = "default");*/
    

    private:

    sf::Texture skin;
    void initChar();
};