#include "platform.h"
#include "zone.cpp"

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

    private:
    // Initialize game
    void initWindows();
    // Generates window
    sf::VideoMode vidMode;
    sf::Event event;
   
    

};