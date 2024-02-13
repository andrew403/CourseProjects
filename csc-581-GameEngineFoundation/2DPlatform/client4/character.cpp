#include "character.h"
using namespace std;
void Character::updateWindowCollision(sf::RenderTarget* target){
    if(this->CharShape.getGlobalBounds().left <= 0.f){
        
        this->CharShape.setPosition(0, this->CharShape.getGlobalBounds().top);
    }
    // Right side
    else if(this->CharShape.getGlobalBounds().left + this->CharShape.getGlobalBounds().width >= target->getSize().x){
        this->CharShape.setPosition(target->getSize().x - this->CharShape.getGlobalBounds().width, this->CharShape.getGlobalBounds().top);
    }
    // Top left
    else if(this->CharShape.getGlobalBounds().left <= 0.f && this->CharShape.getGlobalBounds().top <= 0.f){
        this->CharShape.setPosition(0, 0);
    }
    // Top side
    if(this->CharShape.getGlobalBounds().top <= 0.f ){
        this->CharShape.setPosition((int)this->CharShape.getGlobalBounds().left  , 0.f);
    }
    // Bottom side
    else if(this->CharShape.getGlobalBounds().top + this->CharShape.getGlobalBounds().height >= target->getSize().y){
        this->CharShape.setPosition((int)this->CharShape.getGlobalBounds().left , target->getSize().y - this->CharShape.getGlobalBounds().height);
    }


}

void Character::CharRender(sf::RenderTarget* target){

    target->draw(this->CharShape);
}

void Character::updateInput(sf::RenderTarget* Target, float dt){
    //Process keyboard input here
        
        float spd = dt*this->velocity;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) 
            this->CharShape.move(-1*spd, 0.f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            this->CharShape.move(spd, 0.f);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            this->CharShape.move(0.f, -1*spd);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            this->CharShape.move(0.f, spd);
            

}

void Character::CharUpdate(sf::RenderTarget* target, float dt){
    this->updateInput(target, dt);
    //Set boundary collision
    //this->updateWindowCollision(target);
    
   
}

float Character::getCharPosX(){
    return this->CharShape.getPosition().x;
}

float Character::getCharPosY(){
    return this->CharShape.getPosition().y;
}

Character::Character(){
    this->spawnX = 200;
    this->spawnY = 100;
    this->CharShape.setPosition(this->spawnX, this->spawnX);
    this->velocity = 500.f;
    this->initChar();
}

Character::~Character(){
}

void Character::initChar(){
    // Import texture
    if(!this->skin.loadFromFile("image/ufo.png")){
        cout << "Fail to load character texture" << endl;
    }
    this->CharShape.setTexture(this->skin);
    this->CharShape.setScale(0.1f, 0.1f);

}