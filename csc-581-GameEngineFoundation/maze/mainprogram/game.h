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
    Platform boundary0;
    Platform boundary1;
    Platform boundary2;
    Platform boundary3;
    Platform wall0;
    Platform wall1;
    Platform wall2;
    Platform wall3;
    Platform wall4;
    Platform wall5;
    Platform wall6;
    Platform wall7;
    Platform wall8;
    Platform wall9;
    Platform wall10;
    Platform wall11;
    Platform wall12;
    Platform wall13;
    Platform wall14;
    Platform wall15;
    Platform wall16;
    Platform wall17;
    Platform wall18;
    Platform wall19;
    Platform wall20;
    Platform wall21;
    Platform wall22;
    Platform wall23;
    Platform wall24;
    Platform wall25;
    Platform wall26;
    Platform wall27;
    Platform wall28;
    Platform wall29;
    Platform wall30;
    Platform wall31;
    Platform wall32;
    Platform wall33;
    Platform wall34;
    Platform wall35;
    Platform wall36;
    Platform wall37;
    Platform wall38;
    Platform wall39;
    Platform wall40;
    Platform wall41;
    Platform wall42;
    Platform wall43;
    Platform wall44;
    Platform wall45;
    Platform wall46;

    Platform startZone;
    Platform deadZone0;
    Platform deadZone1;
    Platform deadZone2;

    //Platform winZone;

    sf::CircleShape win;

    Zone deathZone;
    Event onLeft;
    Event onRight;
    Event onUp;
    Event onDown;
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