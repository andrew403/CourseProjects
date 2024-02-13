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
#include<X11/Xlib.h>

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
    sf::Sprite CharShape; // The object of the character
    float spawnX = 0.f;
    float spawnY = 0.f;
    float boundaryMovement = 0;
    float positionX, positionY;     // Position of the character
    float velocityX = 0.f;
    float velocityY = 0.f;     // Velocity of the character
    float gravity = 0.5f;           // How strong is gravity
    bool isjumping = false;

    private:
    float velocity;
    sf::Texture skin;
    void initChar();
};