// BluetoothConnector.hpp
#ifndef BLUETOOTH_CONNECTOR_HPP
#define BLUETOOTH_CONNECTOR_HPP
#include <stdio.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <tinyb.hpp>
#include <memory>
#include <mutex>

using namespace std;
using namespace tinyb;


//Designed for wt9011DCL_BT50 
class BluetoothConnector {
public:
    BluetoothConnector(const std::string &object_device_mac);
    ~BluetoothConnector();
    static std::string dataReceived;
    void establish_conn(){ __establish_conn();}
    bool stat = 0;
private:
    std::string device_mac_address;
    std::unique_ptr<tinyb::BluetoothDevice> device;
    tinyb::BluetoothManager *manager;
    std::string UUID_SERVICE = "0000ffe5-0000-1000-8000-00805f9a34fb";
    std::string UUID_RD = "0000ffe4-0000-1000-8000-00805f9a34fb";
    std::string UUID_WR = "0000ffe9-0000-1000-8000-00805f9a34fb";
    void __establish_conn();
    void disconnect_device();
    static void recv_notification_callback(const std::vector<uint8_t>& data);
    static std::string to_hex_string(const std::vector<uint8_t>& data);

};
std::string BluetoothConnector::dataReceived;

BluetoothConnector::BluetoothConnector(const std::string &object_device_mac) 
    : device_mac_address(object_device_mac), manager(tinyb::BluetoothManager::get_bluetooth_manager()) {
}

BluetoothConnector::~BluetoothConnector() {
    if(device!=nullptr)disconnect_device();
}

void BluetoothConnector::__establish_conn(){
    manager->start_discovery();
    cout << "find BLE devices..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(4));
    manager->stop_discovery();
    unique_ptr<BluetoothDevice> device = nullptr;
    int tryCount = 0;
    do {
        vector<unique_ptr<BluetoothDevice>> list = manager->get_devices();
        for (auto it = list.begin(); it != list.end(); ++it) {
            if ((*it)->get_address() == device_mac_address) {
                device = move(*it);
                break;
            }
        }
        if (device == nullptr) {
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    } while (device == nullptr && ++tryCount<5);

    if(device==nullptr){
        cerr<<"Device "<<device_mac_address<<"nor found."<<endl;
        return;
    }
    cout << "Attempting to connect to "<< device->get_address() << endl;
    if (!device->connect()) {
        cerr << "Connection failed." << endl;
        return;
    }
    cout << "Connected successfully." << endl;
    cout<<device->get_connected()<<endl;

    std::unique_ptr<tinyb::BluetoothGattService> service = device->find(&UUID_SERVICE);
    if (service == nullptr) {
            cerr << "Service not found." << endl;
            return;
    }
    auto characteristics = service->get_characteristics();
    for(auto& characteristic:characteristics){
        if (characteristic->get_uuid() == UUID_RD) {
            // 在此处注册回调
            characteristic->enable_value_notifications(recv_notification_callback);
        }
    }
    while(1){}
}

void BluetoothConnector::disconnect_device() {
    device->disconnect();
    cout << "device "<<device_mac_address<<"disconected."<< endl;
}

std::string BluetoothConnector::to_hex_string(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : data) {
        ss << std::setw(2) << static_cast<int>(byte) << " ";
    }
    return ss.str();
}

void BluetoothConnector::recv_notification_callback(const std::vector<uint8_t>& data){
    dataReceived = to_hex_string(data);
}





#endif