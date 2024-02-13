#include <zmq.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include "timeline.cpp"

vector<float> sndmsg = { 0, 0, 0, 0, 0, 0 };  // Store and send the coordinates of the platform
vector<float> rcvchar0 = { -1, -1, 0, 0 };  // The coordinates of the clients
vector<float> rcvchar1 = { -1, -1, 1, 0 };
vector<float> rcvchar2 = { -1, -1, 2, 0 };
vector<float> rcvchar3 = { -1, -1, 3, 0 };
vector<float> rcvchar4 = { -1, -1, 4, 0 };
vector<float> temp = {0,0,-1};
int iteration[5] = {0, 0, 0, 0, 0};
int iteration_id[5] = {0, 0, 0, 0, 0};
int curr_connection=0;  // Number of connected clients
timeline timeIter;

class PlatformData{
    public:
        float Xdist = 0.f;
        float Ydist = 0.f;
        float Xpos = 0.f;
        float Ypos = 0.f;
        float width = 10.f;
        float height = 10.f;
        float vel = 0.f;
        float currentPosX = 0.f;
        float currentPosY = 0.f;
        void updatePlatformMovement(float currTime);
        PlatformData(float xd, float yd, float xp, float yp, float w, float h, float v){
            this->Xdist = xd ; 
            this->Ydist = yd;
            this->Xpos = xp;
            this->Ypos = yp;
            this->width = w;
            this->height = h;
            this->vel = v;
            this->currentPosX = Xpos;
            this->currentPosY = Ypos;
            this->Tx = Xdist;
            this->Ty = Ydist;
        }
        float Tx;   //X distance changed from the starting point
        float Ty;   //Y distance changed from the starting point
        bool goBackward = false;
        bool goUpward = false;


};


void PlatformData::updatePlatformMovement(float dt){
    float spd = this->vel * dt; //calculate moving distance
    
    //cout<<"distance: "<<vel<<", "<<dt<<", "<<spd<<endl;
    if(this->Xdist!=0 && this->goBackward == false){

        if( this->vel>=0){
            this->Tx-=spd;
            this->currentPosX -= spd;
        }
        if(this->Tx <= 0){
            Tx=0;
            this->currentPosX = this->Xpos-this->Xdist;
            goBackward = true;
        }
    }
    else if(this->Xdist!=0  && this->goBackward == true){
        
        if( this->vel>=0){
            this->Tx+=spd;
            this->currentPosX += spd;
        }
        if(this->Tx >= Xdist){
            Tx = Xdist;
             this->currentPosX = this->Xpos;
            goBackward = false;
        }
            
    }
    if(this->Ydist!=0 && this->goUpward == false){

        if(this->vel>=0){
            this->Ty-=spd;
            this->currentPosY -= spd;
        }
        if(this->Ty <= 0){
            Ty = 0;
            this->currentPosY = this->Ypos;
            goUpward = true;
        }
    }
    else if(this->Ydist!=0 && this->goUpward == true){

        if(this->vel>=0){
            this->Ty+=spd;
            this->currentPosY += spd;
        }
        if(this->Ty >= Ydist){
            Ty=Ydist;
            this->currentPosY = this->Ypos+this->Ydist;
            goUpward = false;
        }
    }
    
    }


void runPlatform(){
    timeline serverRT;
    PlatformData enm0(110.f, 0.f, 370.f, 610.f, 120.f, 120.f, 10);// (moving x-axis, moving y-axis, position x, position y, width, height, velocity)
    PlatformData enm1(110.f, 0.f, 850.f, 370.f, 120.f, 120.f, 15);
    PlatformData enm2(110.f, 0.f, 970.f, 970.f, 120.f, 120.f, 20);
    
     cout<<"Game running on id: "<<gettid()<<endl;
    
    while(true){
        serverRT.updateDeltaT();
        enm0.updatePlatformMovement(serverRT.deltaT);
        enm1.updatePlatformMovement(serverRT.deltaT);
        enm2.updatePlatformMovement(serverRT.deltaT);
        
        
        sndmsg[0]=enm0.currentPosX;
        sndmsg[1]=enm0.currentPosY;

        sndmsg[2]=enm1.currentPosX;
        sndmsg[3]=enm1.currentPosY;

        sndmsg[4]=enm2.currentPosX;
        sndmsg[5]=enm2.currentPosY;

        
        sleep(0.001);
        //cout<<sndmsg[4]<<", "<< sndmsg[5]<<endl;
    }
        

}

void runPUBServer(){
    int it = 0;
    cout<<"PUB Server running on id: "<<gettid()<<endl;
    auto start = chrono::high_resolution_clock::now();
    zmq::context_t context(1);
    zmq::socket_t publisher(context, ZMQ_PUB);
    publisher.bind("tcp://*:5559");
    map<int, int> connected_clients; // map the client id to the connection order

    while(true){
        
        // ============ PUB ============
        int find_id=temp[2]; //Get the client id from data
        if (connected_clients.find(find_id) == connected_clients.end() && find_id != -1){ // Check if the client id has appeared on the map list
            curr_connection++; // Add new client
            cout<<"NEW CONNECTED CLIENT: "<<find_id<<", CURRENT CONNECTIONS: "<<curr_connection<<endl;
            connected_clients[find_id] = curr_connection; // map client id with the order of connection
            
            string broadcast_message = "New NNNNNWERRclient joined: " + to_string(find_id);
            zmq::message_t broadcast(broadcast_message.c_str(), broadcast_message.size());
            publisher.send(broadcast);

            zmq::message_t broadcast_list(zmq::message_t("NEWCLIENT")); // Broadcast current client list, just for easy understanding
            publisher.send(broadcast_list);
            //publisher.send(broadcast_list);
            //publisher.send(broadcast_list);
            //publisher.send(broadcast_list);
            //publisher.send(broadcast_list);
            for (const auto& client : connected_clients) {
                cout<<client.first<<endl;;
                publisher.send(zmq::message_t(to_string(client.first)));
                
                
            }
            //publisher.send(zmq::message_t("END"));
        }
        map<int, int>::iterator iter=connected_clients.begin();
        
        
       
        while(iter!=connected_clients.end()){ // In this while loop, we sort the data by client id and store into corresponding vector
            //cout<<rcvchar0[0]<<rcvchar0[1]<<endl;
            if(iter->first==0 &&find_id==0){
                copy(temp.begin(), temp.end(), rcvchar0.begin());
                rcvchar0[3]=curr_connection;
                //cout<<"send "<<iter->first<<": "<<rcvchar0[0]<<", "<<rcvchar0[1]<<"(0)"<<endl;
                zmq::message_t p0msg(rcvchar0.size() * sizeof(float));
                memcpy(p0msg.data(), rcvchar0.data(), rcvchar0.size() * sizeof(float));
                // Broadcast client updates to every client
                publisher.send(p0msg);
               
                
            }
            else if(iter->first==1 &&find_id==1){
                copy(temp.begin(), temp.begin()+3, rcvchar1.begin());
                rcvchar1[3]=curr_connection;
                //cout<<"send "<<iter->first<<": "<<rcvchar1[0]<<", "<<rcvchar1[1]<<endl;
                zmq::message_t p1msg(rcvchar1.size() * sizeof(float));
                memcpy(p1msg.data(), rcvchar1.data(), rcvchar1.size() * sizeof(float));
                publisher.send(p1msg);
                
                
            }
            else  if(iter->first==2 &&find_id==2){
                copy(temp.begin(), temp.end(), rcvchar2.begin());
                rcvchar2[3]=curr_connection;
                //cout<<"send "<<iter->first<<endl;
                memcpy(rcvchar2.data(), temp.data(), temp.size());
                zmq::message_t p2msg(rcvchar2.size() * sizeof(float));
                memcpy(p2msg.data(), rcvchar2.data(), rcvchar2.size() * sizeof(float));
                publisher.send(p2msg);
                
                

            }
            else  if(iter->first==3 &&find_id==3){
                copy(temp.begin(), temp.end(), rcvchar3.begin());
                rcvchar3[3]=curr_connection;
                //cout<<"send "<<iter->first<<endl;
                memcpy(rcvchar3.data(), temp.data(), temp.size());
                zmq::message_t p3msg(rcvchar3.size() * sizeof(float));
                memcpy(p3msg.data(), rcvchar3.data(), rcvchar3.size() * sizeof(float));
                publisher.send(p3msg);
                
                

            }
            else  if(iter->first==4 &&find_id==4){
                copy(temp.begin(), temp.end(), rcvchar4.begin());
                rcvchar4[3]=curr_connection;
                //out<<"send "<<iter->first<<": "<<rcvchar1[0]<<", "<<rcvchar1[1]<<endl;
                memcpy(rcvchar4.data(), temp.data(), temp.size());
                zmq::message_t p4msg(rcvchar4.size() * sizeof(float));
                memcpy(p4msg.data(), rcvchar4.data(), rcvchar4.size() * sizeof(float));
                publisher.send(p4msg);
                
                

            }
            sleep(0.01); // Sleep to prevent new client joined message being ignored by the client
            iter++;
        }
        it++;
        
        if(it == 1000000){
            auto end = chrono::high_resolution_clock::now();
            auto difference = chrono::duration_cast<chrono::milliseconds>(end - start);
            
            cout<<"Duration time of PUB thread in ms: ";
            cout<<difference.count()<<endl;
    
        }
    }
    
}

void runREQServer(){
    int it=0;
    auto start = chrono::high_resolution_clock::now();
    cout<<"REQ Server running on id: "<<gettid()<<endl;
    zmq::context_t context(1);
    zmq::socket_t responder(context, ZMQ_REP);
    responder.bind("tcp://*:5555");

    while (true) {

        zmq::message_t request;
        responder.recv(&request);
        
        std::memcpy(temp.data(), request.data(), request.size());
    
        // ============ REP =============
        // Send platform coordinates to every client
        zmq::message_t reply(sndmsg.size() * sizeof(float));
        memcpy(reply.data(), sndmsg.data(), sndmsg.size() * sizeof(float));
        responder.send(reply);
        it++;
        //cout<<it<<endl;
        if(it == 1000000){
            auto end = chrono::high_resolution_clock::now();
            auto difference = chrono::duration_cast<chrono::milliseconds>(end - start);
            
            cout<<"Duration time of REQ thread in ms: ";
            cout<<difference.count()<<endl;
    
        }
    }

}



int main() {
    
    thread t1(runPlatform);
    thread t2(runREQServer);
    thread t3(runPUBServer);

    t1.join();
    t2.join();
    t3.join();
    

    return 0;
}
