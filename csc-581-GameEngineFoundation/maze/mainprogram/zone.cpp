#pragma once
#include<iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include"character.h"

using namespace std;
class Zone{
    public:
        void checkCollision(Character &myCharacter){
            if(myCharacter.CharShape.getGlobalBounds().intersects(this->ZoneShape.getGlobalBounds())){
                myCharacter.boundaryMovementX=0;
                myCharacter.boundaryMovementY=0;
                myCharacter.CharShape.setPosition(myCharacter.spawnX, myCharacter.spawnY);
            }
            //cout<<myCharacter.getCharPosX()<<", "<<myCharacter.getCharPosY()<<endl;
        }
        void zoneRender(sf::RenderTarget* target){
        target->draw(this->ZoneShape);
        }

        Zone(float x = 100.f, float y = 100.f, float w = 100.f, float h = 100.f){
            this->XPos = x;
            this->YPos = y;
            this->width = w;
            this->height = h;
            this->ZoneShape.setFillColor(sf::Color::Yellow);
            this->ZoneShape.setSize(sf::Vector2f(width, height));
            this->ZoneShape.setPosition(sf::Vector2f(XPos, YPos));
        }
        sf::RectangleShape ZoneShape;
        virtual ~Zone(){

        }
        float width = 10.f;
        float height = 10.f;
        float XPos = 300.f;
        float YPos = 300.f;
        private:
};
