#include "platform.h"

void Platform::updatePlatformMovement(float x, float y, float bmvmX, float bmvmY){
    this->setPosition(x-bmvmX, y-bmvmY);
}
    
        

void Platform::platUpdate(float x, float y, float bmvmX, float bmvmY){
    this->updatePlatformMovement(x, y, bmvmX, bmvmY);
}

void Platform::platRender(sf::RenderTarget* target){
    target->draw(*this);
}

Platform::Platform(float moveX, float moveY, float xPos, float yPos, float xSize, float ySize, float spd){
    this->setPosition(xPos, yPos);
    this->Xposition = xPos;
    this->Yposition = yPos;
    this->setSize(sf::Vector2f(xSize, ySize));
    this->setFillColor(sf::Color::White);
    this->xAxis = moveX;
    this->yAxis = moveY;
    this->Tx = xAxis;
    this->Ty = yAxis;
    this->velocity = spd;
    
}

Platform::~Platform(){

}