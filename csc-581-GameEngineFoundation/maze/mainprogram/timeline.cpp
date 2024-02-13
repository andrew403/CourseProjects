#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include<time.h>
#include <thread>
using namespace std;

class timeline{
    public:
        float velocity = 1;
        sf::Clock c;
        bool pause = false;
        chrono::time_point<chrono::high_resolution_clock> start_time;
        chrono::time_point<chrono::high_resolution_clock> pause_time;
        chrono::time_point<chrono::high_resolution_clock> unpause_time;
        chrono::time_point<chrono::high_resolution_clock> startRCD_time;
        chrono::time_point<chrono::high_resolution_clock> stopRCD_time;
        chrono::time_point<chrono::high_resolution_clock> replayStart_time;
        chrono::time_point<chrono::high_resolution_clock> replayStop_time;
        float deltaT;
        float timescale = 1.f;
        void updateDeltaT(){
        this->deltaT=this->c.restart().asSeconds()*this->timescale;
            //cout<<"delta:"<<this->deltaT<<endl;
        }
        float getTimeAsMS(){
    return c.getElapsedTime().asMilliseconds();
        }
        timeline(){
            start_time = std::chrono::high_resolution_clock::now();
            c.restart();
        }
        

};
