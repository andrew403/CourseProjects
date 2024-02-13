#include "game.h"
#include <zmq.hpp>

using namespace std;
vector<float> rcvmsg = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
vector<float> charmsg = { 0, 0 ,4};
vector<bool> updateOtherCharacter = { false, false, false, false};
vector<vector<float>> rcvcharmsg = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

int curr_connection=0;
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
        this->updateCharacter();
        this->deathZone.checkCollision(this->MyCharacter);
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
            this->MyCharacter.CharUpdate(this->window, this->deltaT);
        
        if(this->MyCharacter.getCharPosX()>700.f){
            this->MyCharacter.boundaryMovement+=this->MyCharacter.getCharPosX()-700.f;
            this->MyCharacter.CharShape.setPosition(sf::Vector2f(700.f, MyCharacter.getCharPosY()));
        }
        else if(this->MyCharacter.getCharPosX()<100.f){
            this->MyCharacter.boundaryMovement+=this->MyCharacter.getCharPosX()-100.f;
            this->MyCharacter.CharShape.setPosition(sf::Vector2f(100.f, MyCharacter.getCharPosY()));
        }
        else if(this->MyCharacter.getCharPosY()>600.f){
            this->MyCharacter.boundaryMovement=0;
            this->MyCharacter.CharShape.setPosition(sf::Vector2f(this->MyCharacter.spawnX,this->MyCharacter.spawnY));
        }

}

void Game::updatePlatform(){
    this->detectCharacterCollision(this->mainPlatform);
    this->detectCharacterCollision(this->movingPlatformX);
    this->detectCharacterCollision(this->movingPlatformY);
    this->detectCharacterCollision(this->StaticPlatform0);
    this->detectCharacterCollision(this->StaticPlatform1);
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
    this->MyCharacter.CharRender(this->window);
    this->mainPlatform.platRender(this->window);
    this->movingPlatformX.platRender(this->window);
    this->movingPlatformY.platRender(this->window);
    this->StaticPlatform0.platRender(this->window);
    this->StaticPlatform1.platRender(this->window);
    this->deathZone.zoneRender(this->window);

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



Game::Game(){
    
    this->initWindows();
    this->mainPlatform = Platform(0.f, 0.f, 160.f, 550.f, 480.f, 30.f, -1); // (moving x-axis, moving y-axis, position x, position y, width, height, velocity)
    this->mainPlatform.setFillColor(sf::Color::Blue);
    this->movingPlatformX = Platform(100.f, 0.f, 250.f, 200.f, 200.f, 20.f, 100.f);
    this->movingPlatformY = Platform(100.f, 100.f, 400.f, 400.f, 200.f, 20.f, 50.f);
    this->StaticPlatform0 = Platform(0.f, 0.f, 160.f, 300.f, 200.f, 15.f, 0);
    this->StaticPlatform0.setFillColor(sf::Color::Red);
    this->StaticPlatform1 = Platform(0.f, 0.f, 460.f, 250.f, 200.f, 15.f, 0);
    this->StaticPlatform1.setFillColor(sf::Color::Green);
    //3333333333333333333
    this->deathZone = Zone(300.f, 60.f, 10.f, 100.f);  // (X Position, Y Position, Width, Height)
}


Game::~Game(){
    delete this->window;
}

// ====== Client ======
void runClient(){
    zmq::context_t context(1);
    zmq::socket_t requester(context, ZMQ_REQ);
    requester.connect("tcp://localhost:5555");

    

    /*zmq::context_t context(1);
    zmq::socket_t client(context, ZMQ_DEALER);

    // 设置客户端身份信息，可以是唯一的字符串
    std::string client_id = "MAINCPP";
    client.setsockopt(ZMQ_IDENTITY, client_id.c_str(), client_id.length());


    // 连接到服务器
    client.connect("tcp://localhost:5555");*/
    

    while (true) {

        string request_data = "Type A Request";
        zmq::message_t request(charmsg.size() * sizeof(float));
        memcpy(request.data(), charmsg.data(), charmsg.size() * sizeof(float));
        requester.send(request);

        /*// 发送数组给服务器
        zmq::message_t request(charmsg.size() * sizeof(float));
        memcpy(request.data(), charmsg.data(), charmsg.size() * sizeof(float));
        // 向服务器发送请求
        client.send(request, zmq::send_flags::none);

        // 接收服务器的回复
        zmq::message_t reply;
        client.recv(reply);

        // 处理服务器的回复
        // 处理服务器的回复（包含浮点数数组）
        if(reply.size()==24){
        const float* float_data = static_cast<const float*>(reply.data());
        size_t float_data_size = reply.size() / sizeof(float);
        std::vector<float> received_float_array(float_data, float_data + float_data_size);
        copy(received_float_array.begin(), received_float_array.end(), rcvmsg.begin());
        }
        else{
        zmq::message_t extraMessageData;
        //client.recv(reply);
        std::string extraMessage(static_cast<const char*>(reply.data()), reply.size());
        std::cout << "Received extra message from server: " << reply<<" Size: "<<reply.size()<<endl;
        }
        //std::cout << "Received reply from server: ";
        
        // 控制客户端发送请求的速率*/
        // 等待服务器响应
        zmq::message_t reply;
        requester.recv(&reply);
        const float* float_data = static_cast<const float*>(reply.data());
        size_t float_data_size = reply.size() / sizeof(float);
        std::vector<float> received_float_array(float_data, float_data + float_data_size);
        copy(received_float_array.begin(), received_float_array.end(), rcvmsg.begin());
        //cout<<reply<<endl;
       
        


        
       
    }

}

// ====== Client End ======

void runSubscriber(){
    zmq::context_t context(1);
    // 订阅第二种类型的消息
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5559"); // 服务器的地址
    subscriber.setsockopt(ZMQ_SUBSCRIBE, "", 0); // 订阅所有消息


    while ((true))
    {
        zmq::message_t broadcast;
        subscriber.recv(&broadcast);
        if(broadcast.size()==10||broadcast.size()==1||broadcast.size()>=20){
            std::string broadcast_str = std::string(static_cast<char*>(broadcast.data()), broadcast.size());
            std::cout << "Client Received Broadcast: " << broadcast_str << std::endl;
        }
        else {
            vector<float> temp ={ 0 ,0 ,0 ,0 };
            std::memcpy(temp.data(), broadcast.data(), broadcast.size());
            //cout<<temp[0]<<", "<<temp[1]<<", "<<temp[2]<<endl;
            // You are in Client 1
            if(temp[2]==0){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[0].begin());
                //cout<<rcvcharmsg[0][0]<<", "<<rcvcharmsg[0][1]<<", "<<rcvcharmsg[0][2]<<", "<<rcvcharmsg[0][3]<<endl;
                curr_connection=rcvcharmsg[0][3];
                updateOtherCharacter[0]=true;
            }
            else if(temp[2]==1){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[1].begin());
                //cout<<rcvcharmsg[1][0]<<", "<<rcvcharmsg[1][1]<<", "<<rcvcharmsg[1][2]<<", "<<rcvcharmsg[1][3]<<endl;
                curr_connection=rcvcharmsg[1][3];
                updateOtherCharacter[1]=true;

            }
            else if(temp[2]==2){
                copy(temp.begin(), temp.end()+1, rcvcharmsg[2].begin());
                //cout<<rcvcharmsg[2][0]<<", "<<rcvcharmsg[2][1]<<", "<<rcvcharmsg[2][2]<<", "<<rcvcharmsg[2][3]<<endl;
                curr_connection=rcvcharmsg[2][3];
                updateOtherCharacter[2]=true;
                
            }
            else if(temp[2]==3){
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
    
    while (game.window->isOpen())
    {   
        // Update delta time
        game.updateDeltaT();
        // Update
        game.update();
        // Render
        game.render();
    }
      t3.join();
      t2.join();

    return 0;
}
