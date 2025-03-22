#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Hub;


class EndDevice {
public:
    int deviceID;
    string status;
    vector<string> data; 
    EndDevice* directConnection;  

    EndDevice(int id) {
        deviceID = id;
        status = "active";
        directConnection = nullptr;  // No direct connection by default
        cout << "End device " << id << " created" << endl;
    }

    
    void sendDataToDirectConnection(const string& data) {
        if (status == "active" && directConnection != nullptr) {
            cout << "End Device " << deviceID << " sending data to Device " << directConnection->deviceID << ": " << data << endl;
            directConnection->receiveData(data);
        } else {
            cout << "End Device " << deviceID << " is inactive or no direct connection." << endl;
        }
    }

    // function to send data to a Hub (for star topology)
    void sendDataToHub(const string& data, Hub* hub);

    // Method to receive data
    void receiveData(const string& data) {
        cout << "End Device " << deviceID << " received data: " << data << endl;
        this->data.push_back(data);  // Changed dataBuffer to data
    }
};


class Hub {
public:
    vector<EndDevice*> connectedDevices;

   // connecting an End Device to the Hub
    void connectDevice(EndDevice* device) {
        connectedDevices.push_back(device);
        cout << "Device " << device->deviceID << " connected to the hub." << endl;
    }

    // function to broadcast data to all connected devices
    void broadcastData(const string& data, EndDevice* senderDevice) {
        cout << "Hub broadcasting data from Device " << senderDevice->deviceID << "." << endl;
        for (EndDevice* device : connectedDevices) {
            if (device != senderDevice) {  // Don't send data back to the sender
                device->(data);
            }
        }
    }
};

// Implementation of the sendData function of the EndDevice class for Hub communication
void EndDevice::sendDataToHub(const string& data, Hub* hub) {
    if (status == "active" && hub != nullptr) {
        cout << "End Device " << deviceID << " sending data to Hub: " << data << endl;
        hub->broadcastData(data, this);
    } else {
        cout << "End Device " << deviceID << " is inactive or no hub connected." << endl;
    }
}

int main() {
    // Test Case 1: Direct Connection between Device 1 and Device 2
    EndDevice device1(1);
    EndDevice device2(2);

    // Create a direct connection between device1 and device2
    device1.directConnection = &device2;
    device2.directConnection = &device1;

    cout << "\nTest Case 1: Direct Connection between Device 1 and Device 2" << endl;
    device1.sendDataToDirectConnection("Hello Device 2 via Direct Connection");
    device2.sendDataToDirectConnection("Hello Device 1 via Direct Connection");

    // Test Case 2: Star Topology with 5 Devices and 1 Hub
    Hub hub1;  
    EndDevice device3(3);
    EndDevice device4(4);
    EndDevice device5(5);

    // Connecting devices to the hub1
    hub1.connectDevice(&device1);
    hub1.connectDevice(&device2);
    hub1.connectDevice(&device3);
    hub1.connectDevice(&device4);
    hub1.connectDevice(&device5);

    cout << "\nTest Case 2: Star Topology with 5 Devices and Hub" << endl;
    
    // Enable communication from  devices to the hub1
    device1.sendDataToHub("Hello from Device 1 to all", &hub1);
    device2.sendDataToHub("Hi from Device 2", &hub1);
    device3.sendDataToHub("Greetings from Device 3", &hub1);
    device4.sendDataToHub("Message from Device 4", &hub1);
    device5.sendDataToHub("Device 5 sending data", &hub1);

    return 0;
}
