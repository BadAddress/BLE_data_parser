#include "BluetoothConnector.hpp"


int main(){

    BluetoothConnector BLEhandler("CD:FE:C3:45:DB:0D");
    std::thread t([&BLEhandler](){
        BLEhandler.establish_conn();
    });

    while(!BLEhandler.stat){}
    while(1){
        cout<<BluetoothConnector::dataReceived<<endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


}