#pragma once
#include "event.cpp"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <thread>
#include <time.h>
#include <filesystem>
#include <utility>
#include <v8.h>

class Game : public timeline{
    public:
    Game();
    virtual ~Game();
    void start();
    void gameUpdateAndRender();
    void update();
    void render();
    void updateEv();
    void detectCharacterCollision(Platform targetPlatform);
    void updatePlatform();
    void updateCharacter();
    void renderOtherCharacter();
    void ExecuteEvents();
    void ExecuteReplayEvents();
    bool isRecording = false;
    bool isReplaying = false;
    sf::RenderWindow* window;
    // Game objects
    Character MyCharacter;
    Character clientCharacter_0;
    Character clientCharacter_1;
    Character clientCharacter_2;
    Platform mainPlatform;
    Platform movingPlatformX;
    Platform movingPlatformY;
    Platform StaticPlatform0;
    Platform StaticPlatform1;
    Zone deathZone;
    Event onLeft;
    Event onRight;
    Event onJump;
    Event onCollide;
    Event onDeath;
    Event onRespawn;
    Event onChordEv;
    Event onRCVServermsg;
    Event startRecording;
    Event stopRecording;
    FooHandler HandleEvent;
    EventManager ManageEvent;
    timeline gameTimer;

    bool goRight = false;

    string guid;

    v8::Local<v8::Object> exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name="default");
    static int getCurrentGUID();
    static std::vector<Game*> game_objects;
    static Game* GameObjectFactory(std::string context_name="default");
	static void ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void setSPD(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getSPD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    static void setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
	static void getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    static void setRight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getRight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

    static int current_guid;

	v8::Isolate* isolate;
	v8::Global<v8::Context>* context;

    private:
    // Initialize game
    void initWindows();
    // Generates window
    sf::VideoMode vidMode;
    sf::Event event;
   
    

};