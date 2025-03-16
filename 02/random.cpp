#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <numeric> 

using namespace std;

class EndDevice {
    public:
        string mac_address;
        //string name;
        int id;
        void* connected_to; // Pointer to connected device (hub or switch)
        int port;           // Port number on the connected device
    
        EndDevice(int id, const string& name) {
            this->id = id;
            this->name = name;
            this->mac_address = "00:1A:2B:3C:4D:5E" + to_string(id);  // Unique MAC address
            this->connected_to = nullptr;
            this->port = -1;
        }
    
        void send_data(const string& data) {
            cout << name << " (" << mac_address << ") sending data: " << data << endl;
        }
    };


class Switch {
public:
    unordered_map<string, int> mac_table;
    vector<EndDevice*> connected_end_devices;
    vector<Hub*> connected_hubs;

    void connect_end_devices_switch(EndDevice* device, int port) {
        connected_end_devices.push_back(device);
        device->connected_to = this;
        device->port = port;
        cout << device->name << " (" << device->mac_address << ") connected to Switch on port " << port << endl;
    }

    void connect_hub(Hub* hub, int port) {
        connected_hubs.push_back(hub);
        hub->connected_to = this;
        hub->port = port;
        cout << "Hub " << hub->id << " connected to Switch on port " << port << endl;
    }

    void learn_address(const string& mac_address, int port) {
        mac_table[mac_address] = port;
        cout << "Switch learned MAC address " << mac_address << " on port " << port << endl;
    }

    void forward_frame(const string& source_mac, const string& destination_mac, const string& frame) {
        // First, learn the source MAC address
        if (!source_mac.empty()) {
            for (auto device : connected_end_devices) {
                if (device->mac_address == source_mac) {
                    learn_address(source_mac, device->port);
                    break;
                }
            }

            // Also check devices connected through hubs
            for (auto hub : connected_hubs) {
                for (auto device : hub->connected_devices) {
                    if (device->mac_address == source_mac) {
                        learn_address(source_mac, hub->port); // Learn that this MAC is reachable through the hub's port
                        break;
                    }
                }
            }
        }

        // Then forward the frame
        if (mac_table.find(destination_mac) != mac_table.end()) {
            int destination_port = mac_table[destination_mac];
            cout << "Switch forwarding frame to port " << destination_port << " for MAC address " << destination_mac << endl;

            // Check if the destination is an end device connected directly
            for (auto device : connected_end_devices) {
                if (device->port == destination_port) {
                    cout << "  - Delivered to " << device->name << endl;
                    return;
                }
            }

            // Check if the destination is through a hub
            for (auto hub : connected_hubs) {
                if (hub->port == destination_port) {
                    cout << "  - Forwarded to Hub " << hub->id << " which will broadcast to all its devices" << endl;
                    // Hub would then broadcast to all its connected devices
                    hub->forward_frame(nullptr, frame, destination_mac);
                    return;
                }
            }
        } else {
            cout << "Switch broadcasting frame to all ports (MAC address not found)" << endl;
            // Forward to all end devices
            for (auto device : connected_end_devices) {
                cout << "  - Broadcasting to " << device->name << endl;
            }

            // Forward to all hubs
            for (auto hub : connected_hubs) {
                cout << "  - Broadcasting to Hub " << hub->id << endl;
                hub->forward_frame(nullptr, frame, destination_mac);
            }
        }
    }
};

class CSMA_CD {
private:
    bool busy_channel = false;

public:
    CSMA_CD() {
        srand(time(0));
    }

    bool sense_channel() {
        busy_channel = (rand() % 10) < 3; // 30% chance the channel is busy
        return busy_channel;
    }

    void transmit(const string& data) {
        cout << "Sensing channel..." << endl;
        if (sense_channel()) {
            cout << "Channel is busy. Waiting..." << endl;
            usleep(300000); // Random wait time 300ms
            transmit(data); // Retry transmission
            return;
        }

        if (rand() % 10 < 2) {  // 20% chance of collision
            cout << "Collision detected! Backing off..." << endl;
            usleep(rand() % 500000); // Random backoff time
            transmit(data); // Retry transmission
            return; // Ensures current execution is terminated
        }

        cout << "Transmitting data: " << data << endl;
    }
};


void testcase1(){
    vector<End
    for(int i=1;i<=5;i++){

    }
}
int main(){
    testcase1();
}