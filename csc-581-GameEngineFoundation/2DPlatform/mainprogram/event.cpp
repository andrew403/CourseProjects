#include<iostream>
#include <map>
#include<string>
#include<vector>

#include "platform.h"
#include "zone.cpp"

using namespace std;
using EventFunction = void(*)();

struct Point
{
    float x;
    float y;
    float execution_time;
};


class Event{
    public:
    string type = "";
    Character* targetObj = nullptr;
    Platform* targetPlt = nullptr;
    float execution_time=0;
    float spd = 0;
    
    Event() : type(""), targetObj(nullptr), spd(0.0f) {}

    Event(string name, Character* target, float dt){
        this->type = name;
        this->targetObj = target;
        this->spd = dt* target->velocity;
    }
    virtual ~Event(){}

};



class EventHandler {
	public:
		virtual void onEvent(Event e) = 0;
        virtual ~EventHandler(){}
    
    private:

};

class FooHandler:EventHandler{
    public:
        void HandleCollisionEvent(Event e){

        sf::FloatRect characterBounds = e.targetObj->CharShape.getGlobalBounds();
        sf::FloatRect platformBounds = e.targetPlt->getGlobalBounds();

        float deltaX = (characterBounds.left + characterBounds.width/2) - (platformBounds.left + platformBounds.width/2);
        float deltaY = (characterBounds.top + characterBounds.height/2) - (platformBounds.top + platformBounds.height/2);
        float intersectX = abs(deltaX) - (30 + (platformBounds.width/2));
        float intersectY = abs(deltaY) - (30 + (platformBounds.height/2));

        if(intersectX < 0.f && intersectY < 0.f){
            if(intersectX>intersectY){
                if(deltaX>0.f){
                    //cout<<"left side collide"<<endl;
                    float newCharacterX = platformBounds.left + platformBounds.width ;
                    e.targetObj->CharShape.setPosition(newCharacterX, e.targetObj->CharShape.getPosition().y);
                    
                }
                else{
                    //cout<<"right side collide"<<endl;
                    float newCharacterX = platformBounds.left - characterBounds.width;
                    e.targetObj->CharShape.setPosition(newCharacterX, e.targetObj->CharShape.getPosition().y);
                }
            }
            else{
                if(deltaY>0.f){
                    //cout<<"bottom side collide"<<endl;
                    float newCharacterY = platformBounds.top + platformBounds.height;
                    e.targetObj->CharShape.setPosition(e.targetObj->CharShape.getPosition().x, newCharacterY);
                }
                else{
                    e.targetObj->velocityY = 0;
                    //cout<<"top side collide"<<endl;
                    float newCharacterY = platformBounds.top - characterBounds.height;
                    e.targetObj->CharShape.setPosition(e.targetObj->CharShape.getPosition().x, newCharacterY);
                    e.targetObj->isjumping = false;
                }

            }
        }

    }
        void onEvent(Event e) {
            if (e.type == "Death"){
                
                e.targetObj->renderingCharacter=false;

            }
            else if (e.type == "Respawn"){
                e.targetObj->boundaryMovement=0; // Reset the window position
                e.targetObj->velocityY = 0; // Reset the velocity
                e.targetObj->CharShape.setPosition(sf::Vector2f(e.targetObj->spawnX,e.targetObj->spawnY));
                e.targetObj->renderingCharacter=true;
                //cout<<"Respawned"<<endl;

            }
            else if (e.type == "up"){
                e.targetObj->jump();

            }
            else if (e.type == "left"){
                e.targetObj->mvLeft(float(e.spd));

            }
            else if (e.type == "right"){
                e.targetObj->mvRight(float(e.spd));

            }
            else if (e.type == "intersect"){
                HandleCollisionEvent(e);
            }
            else if (e.type == "newclient"){
                cout<<"======New Client Joined !======"<<endl;
            }
            else if (e.type == "start"){
                cout<<"Start recording"<<endl;
                e.targetObj->tmp_spawnX=e.targetObj->getCharPosX();
                e.targetObj->tmp_spawnY=e.targetObj->getCharPosY();
                e.targetObj->tmp_boundarymovement = e.targetObj->boundaryMovement;

            }
            else if (e.type == "stop"){
                cout<<"Stop recording"<<endl;
            }
            else if (e.type == "chord"){
                cout<<"Chord event handled !"<<endl;
            }
        }
        virtual ~FooHandler(){}


};




class EventManager {
	public:
		void registerEvent(const string& eventName, EventFunction eventFunction) {
        eventMap[eventName] = eventFunction;
    }
		void deregister(string s){

        }
		void raise(Event e){
            raised_events.push_back(e);
        }
        EventManager(){
            
        }
        map<string, EventFunction> eventMap;
        vector<Event> raised_events;
        vector<Point> recorded_events;
        virtual ~EventManager(){}
		
};



