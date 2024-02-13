#pragma once
#include<iostream>
#include<vector>
#include<ctime>
#include<time.h>
#include <thread>

#include "timeline.cpp"
class Platform : public sf::RectangleShape{
    public:
    Platform(float moveX = 0, float moveY = 0, float xPos = 0, float yPos = 0, float xSize = 50, float ySize = 50, float spd = 0);
    virtual ~Platform();
    void platUpdate( float x, float y, float boundaryMVM);
    void platRender(sf::RenderTarget* target);
    void updatePlatformMovement(float x, float y, float boundaryMVM);

    private:
    float velocity;
    float xAxis;    // Mvoing distance X
    float yAxis;    //Moving distance Y
    float Tx;   //X distance changed from the starting point
    float Ty;   //Y distance changed from the starting point
    float Xposition;  // start position of X axis
    float Yposition;  //start position of Y axis
    bool goBackward = false;
    bool goUpward = false;
   

};