#include <iostream>
#include <unordered_map>
#include<stdio.h>
#include<unistd.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<windows.h>
#include <unistd.h>
#include <numeric>
#include <string>
#include<thread>
#include<chrono>
#include <queue>

using namespace std;

string find_CS(string);
class Hub;
class EndDevice {
public:
    string mac_address;
    string name;
    int id;
    void* connected_to; // Pointer to connected device (hub or switch)
    int port;           // Port number on the connected device

    EndDevice(string name){this->name=name;}

    EndDevice(int id, const string& name) {
        this->id = id;
        this->name = name;
        this->mac_address = "00:1A:2B:3C:4D:5E" + to_string(id);  // Unique MAC address
        this->connected_to = nullptr;
        this->port = -1;
    }

    void printing_data(){
    cout<<id<<" "<<name<<" "<<mac_address<<" "<<endl;
    }

    void receiving_Data(string data) {
        size_t pos = data.find('#');  // Find the '#' separator in the data string
        if (pos != string::npos) {    // Ensure that '#' was found
            string checksum = data.substr(pos + 1);  // Extract the checksum part
            string actual_data = data.substr(0, pos);  // Extract the data part (before '#')

            // Calculate the checksum of the received data
            string new_checksum = find_CS(actual_data);

            // Compare the calculated checksum with the received checksum
            if (new_checksum == checksum) {
                cout << this->name << " received data: " << actual_data << endl;
                cout << "Sending ack " << endl;
            } else {
                cout << "Error detected in data" << endl;
            }
        } else {
            cout << "Invalid data format, no checksum found" << endl;
        }
    }


};
class Hub{
    public:
    string name;
    vector<EndDevice*> connectedDevices;
    Hub(string n){name=n;}
    void connectDevice(EndDevice* device) {
        connectedDevices.push_back(device);
        cout << "Device " <<device->name << " connected to the "<<this->name << endl;
    }
    void broadCastDataWithinHub (string data,EndDevice* source){
    for(EndDevice* device:connectedDevices){
        if(device!=source){
            cout<<device->name<<" receiving data ("<<data<<") from "<<source->name<<endl;
        }
    }
    }

    void sendToOtherHub(EndDevice* source,Hub hub,string data){
            int port_number;
        if(source->port==1) port_number=2;
        else port_number=1;
        cout<<"Sending Data to port "<<port_number<<endl;
        for(EndDevice* device:connectedDevices){
            cout<<device->name<<" receiving data ("<<data<<") from "<<source->name<<endl;
        }

    }

};


class Switch {
public:
    vector<Hub*> connectedhubs;
    unordered_map<string, int> mac_table;
    vector<EndDevice*> connected_end_devices;

    void connect_end_devices_switch(EndDevice* device, int port) {
        connected_end_devices.push_back(device);
        device->connected_to = this;
        device->port = port;
        cout << device->name << " (" << device->mac_address << ") connected to Switch on port " << port << endl;
    }

    void learn_address(const string& mac_address, int port) {
        mac_table[mac_address] = port;
        cout << "Switch learned MAC address " << mac_address << " on port " << port << endl;
    }

    void forward_frame(const string& source_mac, const string& destination_mac, const string& frame) {
        // Forwarding logic based on destination MAC address (if it's known)
        int port = find_port_by_mac(destination_mac);
        if (port != -1) {
            cout << "Forwarding frame to port: " << port << endl;
        } else {
            cout << "Destination MAC not found. Broadcasting to all ports." << endl;
            for (auto device : connected_end_devices) {
                cout << "Forwarding frame to " << device->name << endl;
                device->receiving_Data(frame);
            }
        }
    }

    int find_port_by_mac(const string& mac_address) {
        if (mac_table.find(mac_address) != mac_table.end()) {
            return mac_table[mac_address];  // Return corresponding port
        } else {
            cout << "MAC address not found!" << endl;
            return -1;  // Return -1 if MAC address is not found
        }
    }

    void print_mac_table() {
        cout << "MAC Table:" << endl;
        for (const auto& entry :mac_table) {
            cout << "MAC Address: " << entry.first << ", Port: " << entry.second << endl;
        }
    }
    void connected_Hubs(Hub* hub){
    connectedhubs.push_back(hub);
    cout<<hub->name<<" connected to switch"<<endl;

    }
};

//void EndDevice::sendDataToHub(const string& data, Hub* hub) {
  //  if (status == "active" && hub != nullptr) {
    //    cout << "End Device " << deviceID << " sending data to Hub: " << data << endl;
      //  hub->broadcastData(data, this);
    //} else {
      //  cout << "End Device " << deviceID << " is inactive or no hub connected." << endl;
    //}
//}

class CSMA_CD {
private:
    bool busy_channel = false;

public:
    CSMA_CD() {
        srand(time(0));
    }

    bool sense_channel() {
        busy_channel = (rand() % 10) <3; // 30% chance the channel is busy
        return busy_channel;
    }

    void transmit(const string& data) {
        cout << "Sensing channel..." << endl;
        if (sense_channel()) {
            cout << "Channel is busy. Waiting..." << endl;
            sleep(3); //waiting for 3 seconds
            transmit(data); // Retry transmission
            return;
        }

        if (rand() % 10 < 2) {  // 20% chance of collision
            cout << "Collision detected! Backing off..." << endl;
            sleep(1); //waiting for 1 second
            transmit(data); // Retry transmission
            return;
        }

        cout << "Transmitting data: " << data << endl;
    }
};

class SlidingWindow {
private:
    int window_size;
    queue<string> send_buffer;
    queue<string> ack_buffer;

public:
    SlidingWindow(int window_size) : window_size(window_size) {}

    void send_data(const string& data) {
        if (send_buffer.size() < window_size) {
            send_buffer.push(data);
            cout << "Data added to send buffer: " << data << endl;
        } else {
            cout << "Window is full. Waiting for acknowledgment..." << endl;
        }
    }

    void receive_ack(const string& ack_data) {
        ack_buffer.push(ack_data);
        cout << "Acknowledgment received for data: " << ack_data << endl;

        if (ack_buffer.size() > 0) {
            cout << "Window sliding: Moving to next data" << endl;
            send_buffer.pop();
            ack_buffer.pop();
        }
    }

    void process_buffer() {
        while (!send_buffer.empty()) {
            cout << "Sending data from buffer: " << send_buffer.front() << endl;
            send_buffer.pop();
        }
    }

    void simulate_transmission() {
        string data[] = {"Packet 1", "Packet 2", "Packet 3", "Packet 4", "Packet 5"};
        int total_packets = 5;
        int i = 0;

        while (i < total_packets) {
            while (send_buffer.size() < window_size && i < total_packets) {
                send_data(data[i]);
                i++;
            }

            while (!send_buffer.empty()) {
                Sleep(1000); // Sleep for 1 second on Windows
                receive_ack("Acknowledgment for " + send_buffer.front());
            }

            process_buffer();
        }
    }
};


// Checksum Calculation
string find_CS(string data) {
    int checksum = 0;
    for (char ch : data) {
        checksum += static_cast<int>(ch);
    }
    checksum %= 256;
    return to_string(checksum);
}

// Sliding Window Flow Control Protocol


class Bridge{
    public:
    unordered_map<string, int> mac_table_Bridge;


    void receive_data(EndDevice source,EndDevice destination,string data){

        cout<<"device "<<destination.name<<" Received data "<<data<<" from "<<source.name<<endl;
    }

    void send_data(EndDevice source,EndDevice destination,string data){
        int destination_port;
    if(source.port==destination.port){
            cout<<"communincation within the network"<<endl;
            receive_data(source,destination,data);
    }
    else{
        cout<<"Communication across Bridge"<<endl;

        if (mac_table_Bridge.find(destination.mac_address) != mac_table_Bridge.end()) {
            destination_port=mac_table_Bridge[destination.mac_address];  // Return corresponding port
        } else {
            cout << "MAC address not found!" << endl;
            return;  
        }
        cout<<"Destination mac address "<<destination.mac_address<<" is present at port "<<destination_port<<endl;
        receive_data(source,destination,data);

    }
    }
    void learn_addressing(EndDevice d1,int port){
    mac_table_Bridge[d1.mac_address]=port;
    }

        void print_mac_table_Bridge() {
        cout << "MAC Table:" << endl;
        for (const auto& entry : mac_table_Bridge) {
            cout << "MAC Address: " << entry.first << ", Port: " << entry.second << endl;
        }
    }
};





void bridge(){
    Bridge mybridge;
EndDevice device1(1,"EndDevice1");
EndDevice device2(2,"EndDevice2");
device1.port=device2.port=1;
mybridge.send_data(device1,device2,"Hello");
EndDevice device3(3,"EndDevice3");
EndDevice device4(4,"EndDevice4");
device3.port=device4.port=2;

mybridge.learn_addressing(device1,device1.port);
mybridge.learn_addressing(device2,device2.port);
mybridge.learn_addressing(device3,device3.port);
mybridge.learn_addressing(device4,device4.port);
mybridge.send_data(device1,device3,"Hello");
}

void testcase1() {
    vector<EndDevice*> devices;
    Switch mySwitch;
    devices.push_back(nullptr);

    for (int i = 1; i <= 5; ++i) {
        EndDevice* device = new EndDevice(i, "EndDevice" + to_string(i));  // Naming from EndDevice1 to EndDevice5
        devices.push_back(device);
        mySwitch.connect_end_devices_switch(device, i);  // Connect to switch on port i
        mySwitch.learn_address(device->mac_address, i);
    }

    int i, j;
    string data;
    cout << "Enter source Device (1 to 5): " << endl;
    cin >> i;
    cout << "Enter Destination Device (1 to 5): " << endl;
    cin >> j;
    cout << "Enter Data: " << endl;
    cin.ignore();
    getline(cin, data);
    string checksum = find_CS(data);

    data += "#";
    data += checksum;
    cout << "Code Word = " << data << endl;

    cout << "Finding Destination.... " << endl;
    int Destination_port = mySwitch.find_port_by_mac(devices[j]->mac_address);

    cout << "Destination port is : " << Destination_port << endl;
    CSMA_CD cd;
    cd.transmit(data);

    devices[j]->receiving_Data(data);

    vector<string> packets={"p1","p2","p3","p4","p5"};
    // Sliding window control
    //SlidingWindow window(3);
   // window.send_data(data); // Simulating data sending in the window
    //window.process_buffer(); // Processing send buffer after receiving acknowledgment

    // Now print the MAC table
    //mySwitch.print_mac_table();

    cout<<"Collision domains: 5 (one for each device, as each device is in a separate collision domain in a switch)."<<endl;
    cout<<"Broadcast domain: 1 (since all devices are connected to the same switch and broadcast frames are forwarded to all devices in the same network)."<<endl;
}



void testcase2(){
Hub hub1("hub1");
Hub hub2("hub2");
Switch switch1;
vector<EndDevice*> devices;

devices.push_back(NULL);

 for (int i = 1; i <= 10; ++i) {
        EndDevice* device = new EndDevice(i,"EndDevice" + to_string(i));  // Naming from EndDevice1 to EndDevice5
        devices.push_back(device);
    }

for(int i=1;i<=5;i++){
    devices[i]->port=1;
    switch1.learn_address(devices[i]->mac_address,1);
}
for(int i=6;i<=10;i++){
    devices[i]->port=2;
    switch1.learn_address(devices[i]->mac_address,2);
}

switch1.print_mac_table();

hub1.connectDevice(devices[1]);
hub1.connectDevice(devices[2]);
hub1.connectDevice(devices[3]);
hub1.connectDevice(devices[4]);
hub1.connectDevice(devices[5]);

hub2.connectDevice(devices[6]);
hub2.connectDevice(devices[7]);
hub2.connectDevice(devices[8]);
hub2.connectDevice(devices[9]);
hub2.connectDevice(devices[10]);


switch1.connected_Hubs(&hub1);
switch1.connected_Hubs(&hub2);

int i ,j;
string data;
cout<<"Enter source device"<<endl;
cin>>i;
cout<<"Enter Destination device"<<endl;
cin>>j;

cout << "Enter Data: " << endl;
cin.ignore();
getline(cin, data);
if(i>10||j>10){
    cout<<"Invalid devices id's"<<endl;
    return ;
}
else if(i<=5&&j<=5){
    hub1.broadCastDataWithinHub( data,devices[i]);
}
else if(i>6&&j>6){
hub2.broadCastDataWithinHub( data,devices[i]);
}
else{
    if(i<=5){// if source is hub 1
            hub2.sendToOtherHub(devices[i],hub1,data);
    }
    else{
        hub1.sendToOtherHub(devices[i],hub2,data);
    }
}
cout<<"Total Broadcast Domains: 2"<<endl;
cout<<"Total Collision Domains: 10"<<endl;
}

int main() {
    //bridge();
    testcase1();
   //testcase2();
     //SlidingWindow sw(3);
    //sw.simulate_transmission();

}