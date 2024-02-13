
#include "game.h"
#include <cstdio>
#include <stdio.h>
#include <zmq.hpp>
#include <filesystem>

using namespace std;
vector<float> rcvmsg = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // Add new space when create scene objects such as platform, death zone etc.
vector<float> charmsg = { 0, 0 ,0}; // To store and send local character's position info: (posX, posY, ID)
vector<bool> updateOtherCharacter = { false, false, false, false};  // If other clients dropped oput, stop updating / rendering their character
vector<bool> passOrder = { false, false, false, false}; // V, B, N, M
vector<vector<float>> rcvcharmsg = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};
std::vector<Game*> Game::game_objects;
bool updateNewClient = false;
bool raiseOnChord = false;
int Game::current_guid = 0;
int curr_connection = 0;
string chordstr = "string";
int chordptr = 0;
void Game::detectCharacterCollision(Platform targetPlatform){
    sf::FloatRect characterBounds = MyCharacter.CharShape.getGlobalBounds();
    sf::FloatRect platformBounds = targetPlatform.getGlobalBounds();

    float deltaX = (characterBounds.left + characterBounds.width/2) - (platformBounds.left + platformBounds.width/2);
    float deltaY = (characterBounds.top + characterBounds.height/2) - (platformBounds.top + platformBounds.height/2);
    float intersectX = abs(deltaX) - (30 + (platformBounds.width/2));
    float intersectY = abs(deltaY) - (30 + (platformBounds.height/2));

    if(intersectX < 0.f && intersectY < 0.f){
        if(intersectX>intersectY){
            if(deltaX>0.f){
                //cout<<"left side collide"<<endl;
                float newCharacterX = platformBounds.left + platformBounds.width ;
                MyCharacter.CharShape.setPosition(newCharacterX, MyCharacter.CharShape.getPosition().y);
                
            }
            else{
                //cout<<"right side collide"<<endl;
                float newCharacterX = platformBounds.left - characterBounds.width;
                MyCharacter.CharShape.setPosition(newCharacterX, MyCharacter.CharShape.getPosition().y);
            }
        }
        else{
            if(deltaY>0.f){
                //cout<<"bottom side collide"<<endl;
                float newCharacterY = platformBounds.top + platformBounds.height;
                MyCharacter.CharShape.setPosition(MyCharacter.CharShape.getPosition().x, newCharacterY);
            }
            else{
                this->MyCharacter.velocityY = 0;
                //cout<<"top side collide"<<endl;
                float newCharacterY = platformBounds.top - characterBounds.height;
                MyCharacter.CharShape.setPosition(MyCharacter.CharShape.getPosition().x, newCharacterY);
                this->MyCharacter.isjumping = false;
            }

        }
    }
    
    
}

void Game::updateEv(){ //Event listener
    
while (this->window->pollEvent(this->event))
        {
            if (this->event.type == sf::Event::Closed)
                this->window->close();

            if(event.type == sf::Event::Resized){
                sf::Vector2u size = this->window->getSize();
            }
            if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1){
                this->window->create(this->vidMode, "SFML works!", sf::Style::Close);  //Fixed window
                this->window->setFramerateLimit(60);
            }
            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2){
                this->window->create(this->vidMode, "SFML works!", sf::Style::Default);  //Changable size
                this->window->setFramerateLimit(60);
            }
            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1){
                cout<<"1x speed"<<endl;
                this->timescale = 1.f;
            }
            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num2){
                cout<<"0.5x speed"<<endl;
                this->timescale = 0.5f;
            }
            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num3){
                cout<<"1.5x speed"<<endl;
                this->timescale = 1.5f;
            }
            // Pause function
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)&&this->pause == false){
                this->pause_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(this->unpause_time - this->pause_time);
                if(duration.count()!=0){
                    this->pause = true;
                    cout<<"paused"<<duration.count()<<endl;
                }
            }
                
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)&&this->pause == true){
                this->unpause_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(this->unpause_time - this->pause_time);
                if(duration.count()!=0){
                    this->pause = false;
                    cout<<"unpaused"<<duration.count()<<endl;
                }
                
            }

            //Recording function
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)&&this->isRecording == false){
                this->startRCD_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(this->stopRCD_time - this->startRCD_time);
                if(duration.count()!=0){
                    this->ManageEvent.raise(startRecording);
                    this->isRecording = true;
                    cout<<"Start recording"<<endl;
                }
                
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)&&this->isRecording == true){
                this->stopRCD_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(this->stopRCD_time - this->startRCD_time);
                if(duration.count()!=0){
                    this->ManageEvent.raise(stopRecording);
                    this->isRecording = false;
                    cout<<"Stop recording"<<endl;
                }
                
            }
            // Replay function
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)&&this->isReplaying == false){
                this->replayStart_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(this->replayStop_time - this->replayStart_time);
                if(duration.count()!=0){
                    this->isReplaying = true;
                    cout<<"Start replaying..."<<endl;
                    this->MyCharacter.boundaryMovement = this->MyCharacter.tmp_boundarymovement;
                    this->MyCharacter.CharShape.setPosition(sf::Vector2f(this->MyCharacter.tmp_spawnX, this->MyCharacter.tmp_spawnY));
                    sf::Clock replayClock;
                    float firstEvent = ManageEvent.recorded_events[0].execution_time;
                    replayClock.restart();
                    while (!ManageEvent.recorded_events.empty())
                        {
                                if(ManageEvent.recorded_events[0].execution_time <= replayClock.getElapsedTime().asMilliseconds() + firstEvent ){
                                MyCharacter.CharShape.setPosition(sf::Vector2f(ManageEvent.recorded_events[0].x, ManageEvent.recorded_events[0].y));
                                ManageEvent.recorded_events.erase(ManageEvent.recorded_events.begin());
                                //cout<<"upd"<<endl;
                                }
                                
                                this->render();
                                this->updatePlatform();

                        }
                    this->isReplaying = false;
                    this->MyCharacter.boundaryMovement = this->MyCharacter.tmp_boundarymovement;
                    this->MyCharacter.CharShape.setPosition(sf::Vector2f(this->MyCharacter.tmp_spawnX, this->MyCharacter.tmp_spawnY));
                    this->ManageEvent.recorded_events.clear();
                    cout<<"Replay stopped"<<endl;
                }
                
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)&&this->isReplaying == true){
                this->replayStop_time = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::seconds>(this->replayStop_time - this->replayStart_time);
                if(duration.count()!=0){
                    this->isReplaying = false;
                    this->MyCharacter.boundaryMovement = this->MyCharacter.tmp_boundarymovement;
                    this->MyCharacter.CharShape.setPosition(sf::Vector2f(this->MyCharacter.tmp_spawnX, this->MyCharacter.tmp_spawnY));
                    this->ManageEvent.recorded_events.clear();
                    cout<<"Replay stopped"<<endl;
                }
                
            }

            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::V){
                passOrder[0] = true;
            }

            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::B){
                passOrder[1] = true;
            }

            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::N){
                passOrder[2] = true;
            }

            else if(sf::Event::KeyPressed && event.key.code == sf::Keyboard::M){
                passOrder[3] = true;
            }
            
 
        }
}



void Game::initWindows(){
    this->vidMode.height=600;
    this->vidMode.width=800;
    this->window = new sf::RenderWindow(this->vidMode, "SFML works!", sf::Style::Close);
    this->window->setFramerateLimit(60);

}

void Game::update(){
    this->updateEv();
    if(pause == false){
        this->updatePlatform();
        // Detect collide event
        if(mainPlatform.getGlobalBounds().intersects(MyCharacter.CharShape.getGlobalBounds())){
            this->onCollide.targetPlt = &mainPlatform;
            this->onCollide.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onCollide);

        }
        else if(movingPlatformX.getGlobalBounds().intersects(MyCharacter.CharShape.getGlobalBounds())){
            this->onCollide.targetPlt = &movingPlatformX;
            this->onCollide.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onCollide);

        }
        else if(movingPlatformY.getGlobalBounds().intersects(MyCharacter.CharShape.getGlobalBounds())){
            this->onCollide.targetPlt = &movingPlatformY;
            this->onCollide.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onCollide);

        }
        else if(StaticPlatform0.getGlobalBounds().intersects(MyCharacter.CharShape.getGlobalBounds())){
            this->onCollide.targetPlt = &StaticPlatform0;
            this->onCollide.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onCollide);

        }
        else if(StaticPlatform1.getGlobalBounds().intersects(MyCharacter.CharShape.getGlobalBounds())){
            this->onCollide.targetPlt = &StaticPlatform1;
            this->onCollide.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onCollide);

        }

        // Detect keypress event
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            this->onLeft=Event("left", &MyCharacter, this->deltaT);
            this->onLeft.execution_time = gameTimer.getTimeAsMS();
            //cout<<"raising Left event"<<endl;
            this->ManageEvent.raise(this->onLeft);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            this->onRight=Event("right", &MyCharacter, this->deltaT);
            this->onRight.execution_time = gameTimer.getTimeAsMS();
            //cout<<"raising Right event"<<endl;
            this->ManageEvent.raise(this->onRight);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            this->onJump=Event("up", &MyCharacter, this->deltaT);
            this->onJump.execution_time = gameTimer.getTimeAsMS();
            //cout<<"raising Up event"<<endl;
            this->ManageEvent.raise(this->onJump);
        }

        if(this->MyCharacter.getCharPosY()>600.f && this->MyCharacter.getCharPosY()<650.f){ // To prevent too much duplicated death events clogging the event queue,
            this->onDeath.execution_time = gameTimer.getTimeAsMS(); // I sized down the area for triggering the event
            this->ManageEvent.raise(this->onDeath);
            this->onRespawn.execution_time = gameTimer.getTimeAsMS() + 3000; // Wait 3 seconds before respawn
            this->ManageEvent.raise(this->onRespawn);
            
        }

        if(raiseOnChord){
            raiseOnChord = false;
            this->onChordEv=Event("chord", &MyCharacter, this->deltaT);
            this->onChordEv.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onChordEv);
        }

        if(updateNewClient){
            updateNewClient = false;
            this->onRCVServermsg.execution_time = gameTimer.getTimeAsMS();
            this->ManageEvent.raise(this->onRCVServermsg);

        }
        this->updateCharacter();


        //this->deathZone.checkCollision(this->MyCharacter);
    }
    
    
    //  Update need to follow the same order as rendering 
    
    
}

void Game::updateCharacter(){
    // Jump function start
        this->MyCharacter.positionX = this->MyCharacter.getCharPosX();
        this->MyCharacter.positionY = this->MyCharacter.getCharPosY();

        this->MyCharacter.velocityY += this->MyCharacter.gravity;
        this->MyCharacter.positionY += this->MyCharacter.velocityY;
        this->MyCharacter.positionX += this->MyCharacter.velocityX;
    
        if(this->MyCharacter.velocityY < -10.0)
            this->MyCharacter.velocityY = -10.0;

        this->MyCharacter.CharShape.setPosition(sf::Vector2f(this->MyCharacter.positionX, this->MyCharacter.positionY));
    // Jump function end
        //cout<<"update character"<<endl;
        charmsg[0] = this->MyCharacter.getCharPosX();
        charmsg[1] = this->MyCharacter.getCharPosY();
        if(pause == false)
            //this->MyCharacter.CharUpdate(this->window, this->deltaT);
        
        if(this->MyCharacter.getCharPosX()>700.f){
            this->MyCharacter.boundaryMovement+=this->MyCharacter.getCharPosX()-700.f;
            this->MyCharacter.CharShape.setPosition(sf::Vector2f(700.f, MyCharacter.getCharPosY()));
        }
        else if(this->MyCharacter.getCharPosX()<100.f){
            this->MyCharacter.boundaryMovement+=this->MyCharacter.getCharPosX()-100.f;
            this->MyCharacter.CharShape.setPosition(sf::Vector2f(100.f, MyCharacter.getCharPosY()));
        }
       
}

void Game::updatePlatform(){
    
    this->mainPlatform.platUpdate(rcvmsg[0], rcvmsg[1], this->MyCharacter.boundaryMovement);
    this->movingPlatformX.platUpdate(rcvmsg[2], rcvmsg[3], this->MyCharacter.boundaryMovement);
    this->movingPlatformY.platUpdate(rcvmsg[4], rcvmsg[5], this->MyCharacter.boundaryMovement);
    this->StaticPlatform0.platUpdate(rcvmsg[6], rcvmsg[7], this->MyCharacter.boundaryMovement);
    this->StaticPlatform1.platUpdate(rcvmsg[8], rcvmsg[9], this->MyCharacter.boundaryMovement);
    this->deathZone.ZoneShape.setPosition(sf::Vector2f(this->deathZone.XPos-this->MyCharacter.boundaryMovement, this->deathZone.YPos));
    //cout<<rcvmsg[4]<<", "<< rcvmsg[5]<<endl;
}

void Game::render(){
    // Clear old frame
    this->window->clear();
    // Render objects, need to follow the same order as updating 
    if(this->MyCharacter.renderingCharacter)
        this->MyCharacter.CharRender(this->window);
    
    this->mainPlatform.platRender(this->window);
    this->movingPlatformX.platRender(this->window);
    this->movingPlatformY.platRender(this->window);
    this->StaticPlatform0.platRender(this->window);
    this->StaticPlatform1.platRender(this->window);
   
    this->renderOtherCharacter();
    //Display frame
    //Should be done at last
    this->window->display();
    
}

void Game::renderOtherCharacter(){
    int temp=curr_connection;
    //cout<<temp<<endl;
    for(int i=0;i<4;i++){
        if(temp>1&&rcvcharmsg[i][3]==temp&&updateOtherCharacter[i]==true){
            //cout<<"draw"<<endl;
            clientCharacter_0.CharShape.setPosition(rcvcharmsg[i][0], rcvcharmsg[i][1]);
            this->clientCharacter_0.CharRender(this->window);
            updateOtherCharacter[i]=false;
        }
    }
    
}

void Game::ExecuteEvents(){
    //cout<<"Run Event Thread"<<endl;
    while (true)
    {
        if(!ManageEvent.raised_events.empty() && !isReplaying){
            if(ManageEvent.raised_events[0].execution_time <= gameTimer.getTimeAsMS()){
            HandleEvent.onEvent(ManageEvent.raised_events[0]);
                if(isRecording == true){
                    ManageEvent.recorded_events.push_back({MyCharacter.getCharPosX(), MyCharacter.getCharPosY(), ManageEvent.raised_events[0].execution_time});
                }
            ManageEvent.raised_events.erase(ManageEvent.raised_events.begin());
            }
            else {
                if(ManageEvent.raised_events.size() >= 2)
                    rotate(ManageEvent.raised_events.begin(), ManageEvent.raised_events.begin() + 1, ManageEvent.raised_events.end());
            }
        }
    }
    
}

void Game::ExecuteReplayEvents(){
    
}



void usage()
{
	// Usage
	std::cout << "\n"
		<< "JavaScript <--> C++ Integration Example\n"
		<< "---------------------------------------\n"
		<< "\n"
		<< "Commands:\n"
		<< "\tv: to print this message\n"
		<< "\tb: run perform_function.js\n"
		<< "\tn: perform_function.js\n"
		<< "\tm: run ctrl_right.js"
		<< std::endl;

}




void Game::ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate *isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	std::string context_name("default");
	if(args.Length() == 1)
	{
		v8::String::Utf8Value str(args.GetIsolate(), args[0]);
		context_name = std::string(v8helpers::ToCString(str));
#if GO_DEBUG
		std::cout << "Created new object in context " << context_name << std::endl;
#endif
	}
	Game *new_object = new Game();
	v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
	args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}

void Game::setSPD(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Game*>(ptr)->timescale = value->Int32Value();
}

void Game::getSPD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<Game*>(ptr)->timescale;
	info.GetReturnValue().Set(x_val);
}

void Game::setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString());
	static_cast<Game*>(ptr)->guid = *utf8_str;
}

void Game::getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	std::string guid = static_cast<Game*>(ptr)->guid;
	v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str(), v8::String::kNormalString);
	info.GetReturnValue().Set(v8_guid);
}

void Game::setRight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Game*>(ptr)->goRight = value->Int32Value();
}

void Game::getRight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int x_val = static_cast<Game*>(ptr)->goRight;
	info.GetReturnValue().Set(x_val);
}


v8::Local<v8::Object> Game::exposeToV8(v8::Isolate *isolate, v8::Local<v8::Context> &context, std::string context_name)
{
	vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
	v.push_back(v8helpers::ParamContainer("spd", getSPD, setSPD));
    v.push_back(v8helpers::ParamContainer("guid", getGameObjectGUID, setGameObjectGUID));
    v.push_back(v8helpers::ParamContainer("right", getRight, setRight));

	return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}

void detectChord(){
    char c;
    while(true){
        c = getchar();
        if(c=='s' && chordstr[chordptr] == c){
            //cout<<"s"<<chordptr<<endl;
            chordptr++;
        }
        else if(c=='t' && chordstr[chordptr] == c){
            //cout<<"t"<<endl;
            chordptr++;
        }
        else if(c=='r' && chordstr[chordptr] == c){
            //cout<<"r"<<endl;
            chordptr++;
        }
        else if(c=='i' && chordstr[chordptr] == c){
            //cout<<"i"<<endl;
            chordptr++;
        }
        else if(c=='n' && chordstr[chordptr] == c){
            //cout<<"n"<<endl;
            chordptr++;
        }
        else if(c=='g' && chordstr[chordptr] == c){
            raiseOnChord = true;
            chordptr = 0;
        }
        else{ 
            cout<<"restart"<<endl;
            chordptr = 0;
            }
    }
}




Game::Game(){
  
    this->initWindows();
    this->mainPlatform = Platform(0.f, 0.f, 160.f, 550.f, 480.f, 30.f, 0); // (moving x-axis, moving y-axis, position x, position y, width, height, velocity)
    this->mainPlatform.setFillColor(sf::Color::Blue);
    this->movingPlatformX = Platform(100.f, 0.f, 250.f, 200.f, 200.f, 20.f, 100.f);
    this->movingPlatformY = Platform(100.f, 100.f, 400.f, 400.f, 200.f, 20.f, 50.f);
    this->StaticPlatform0 = Platform(0.f, 0.f, 160.f, 300.f, 200.f, 15.f, 0);
    this->StaticPlatform0.setFillColor(sf::Color::Red);
    this->StaticPlatform1 = Platform(0.f, 0.f, 460.f, 250.f, 200.f, 15.f, 0);
    this->StaticPlatform1.setFillColor(sf::Color::Green);
    this->onLeft = Event("left", &MyCharacter, 0);
    this->onRight = Event("right", &MyCharacter, 0);
    this->onJump = Event("up", &MyCharacter, 0);
    this->onCollide = Event("intersect", &MyCharacter, 0);
    this->onDeath = Event("Death", &MyCharacter, 0);
    this->onRespawn = Event("Respawn", &MyCharacter, 0);
    this->onRCVServermsg = Event("newclient", &MyCharacter, 0);
    this->startRecording = Event("start", &MyCharacter, 0);
    this->stopRecording = Event("stop", &MyCharacter, 0);
    this->onChordEv = Event("chord", &MyCharacter, 0);
    //this->deathZone = Zone(300.f, 60.f, 10.f, 100.f);  // (X Position, Y Position, Width, Height)
    this->guid = "gameobject" + std::to_string(current_guid);
	this->current_guid++;
	game_objects.push_back(this);
}


Game::~Game(){
    delete this->window;
    context->Reset();
}

// ====== Client ======
void runClient(){
    zmq::context_t context(1);
    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5555");
    
    while (true) {

        zmq::message_t request(charmsg.size() * sizeof(float));
        memcpy(request.data(), charmsg.data(), charmsg.size() * sizeof(float));
        requester.send(request);

        zmq::message_t reply;
        requester.recv(&reply);
        const float* float_data = static_cast<const float*>(reply.data());
        size_t float_data_size = reply.size() / sizeof(float);
        std::vector<float> received_float_array(float_data, float_data + float_data_size);
        copy(received_float_array.begin(), received_float_array.end(), rcvmsg.begin());
    }

}

// ====== Client End ======

void runSubscriber(){
    zmq::context_t context(1);
    // Create subscriber to receive client update
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5559"); 
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0); // Subscribe all message


    while ((true))
    {
        zmq::message_t broadcast;
        subscriber.recv(&broadcast);
        if(broadcast.size()==10||broadcast.size()==1||broadcast.size()>=20){
            std::string broadcast_str = std::string(static_cast<char*>(broadcast.data()), broadcast.size());
            std::cout << "Client Received Broadcast: " << broadcast_str << std::endl;
            if(broadcast_str =="NEWCLIENT")
                cout<<"LEOEJNIBFIULWS"<<endl;
                updateNewClient = true;
        }
        else {  // Client coordinate goes here
            vector<float> temp ={ 0 ,0 ,0 ,0 };
            std::memcpy(temp.data(), broadcast.data(), broadcast.size());
            //cout<<temp[3]<<endl;
            // You are in client 0
            if(temp[2]==1){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[0].begin());
                //cout<<rcvcharmsg[0][0]<<", "<<rcvcharmsg[0][1]<<", "<<rcvcharmsg[0][2]<<", "<<rcvcharmsg[0][3]<<endl;
                curr_connection=rcvcharmsg[0][3];
                updateOtherCharacter[0]=true;
            }
            else if(temp[2]==2){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[1].begin());
                //cout<<rcvcharmsg[1][0]<<", "<<rcvcharmsg[1][1]<<", "<<rcvcharmsg[1][2]<<", "<<rcvcharmsg[1][3]<<endl;
                curr_connection=rcvcharmsg[1][3];
                updateOtherCharacter[1]=true;

            }
            else if(temp[2]==3){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[2].begin());
                //cout<<rcvcharmsg[2][0]<<", "<<rcvcharmsg[2][1]<<", "<<rcvcharmsg[2][2]<<", "<<rcvcharmsg[2][3]<<endl;
                curr_connection=rcvcharmsg[2][3];
                updateOtherCharacter[2]=true;
                
            }
            else if(temp[2]==4){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[3].begin());
                curr_connection=rcvcharmsg[3][3];
                updateOtherCharacter[3]=true;
            }
            
            
        }
    }
    
   

}


int main()
{

    Game game;
    thread t2(runClient);
    thread t3(runSubscriber);
    thread t1(&Game::ExecuteEvents, &game);
    thread t4(detectChord);

    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.release());
    v8::V8::InitializeICU();
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(create_params);

     { // anonymous scope for managing handle scope
        v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
        v8::HandleScope handle_scope(isolate);

		// Best practice to isntall all global functions in the context ahead of time.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        // Bind the global 'print' function to the C++ Print callback.
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
		// Bind the global static factory function for creating new GameObject instances
		global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, Game::ScriptedGameObjectFactory));
		// Bind the global static function for retrieving object handles
		global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));
        v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
		v8::Context::Scope default_context_scope(default_context); // enter the context

        ScriptManager *sm = new ScriptManager(isolate, default_context); 

		// Without parameters, these calls are made to the default context
        sm->addScript("perform_function", "scripts/perform_function.js");

		// Create a new context
		v8::Local<v8::Context> object_context = v8::Context::New(isolate, NULL, global);
		sm->addContext(isolate, object_context, "object_context");

		Game *go = &game;
		go->exposeToV8(isolate, object_context);

		// With the "object_context" parameter, these scripts are put in a
		// different context than the prior three scripts
		sm->addScript("modify_spd", "scripts/mdspd.js", "object_context");
        sm->addScript("right", "scripts/ctrl_right.js", "object_context");
		

		// Use the following 4 lines in place of the above 4 lines as the
		// reference if you don't plan to use multiple contexts
		/* sm->addScript("create_object", "scripts/create_object.js"); */
		/* sm->addScript("random_object", "scripts/random_object.js"); */
		/* sm->addScript("random_position", "scripts/random_position.js"); */
		/* sm->addScript("modify_position", "scripts/modify_position.js"); */

		usage();
 


    bool reload = false;
    //thread t4(&Game::RunScripting, &game);
    
    while (game.window->isOpen())
    {   
        // Update delta time
        game.updateDeltaT();
        // Update
        game.update();
        // Render
        game.render();

        if(passOrder[0] == true)
				{
					usage();
                    passOrder[0] = false;
				}
		else if(passOrder[1] == true)
				{
					sm->runOne("perform_function"); //Handle function
					passOrder[1] = false;
                    reload = false;
				}
		else if(passOrder[2] == true)
				{
                    cout<<"Current speed: "<< go->timescale <<endl; // Change the moving speed
					sm->runOne("modify_spd", reload,  "object_context");
                    cout<<"Change speed to "<< go->timescale <<endl;
                    passOrder[2] = false;
                    reload = false;
				}
		else if(passOrder[3] == true)
				{
                    sm->runOne("right", reload,  "object_context");
					if(game.goRight){ // Control the character to move right
                        //cout<<"go rignt"<<endl;
                        game.onRight=Event("right", &game.MyCharacter, game.deltaT);
                        game.onRight.execution_time = game.gameTimer.getTimeAsMS();
                        game.ManageEvent.raise(game.onRight);
                        game.goRight = false;
                    }
                    passOrder[3] = false;
                    reload = false;
				}
    }
      t2.join();
      t3.join();
      t1.join();
      t4.join();

     }
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
    return 0;
}
