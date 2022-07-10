#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEAddress.h>
#include <bits/stdc++.h>

// Tuy chỉnh các thông số để phù hợp với môi trường hiện tại
#define BaseW  2000
#define BaseI  5000
#define SmallW  1000
#define N 5             // Ngưỡng thiết bị
#define redundantN 30   // Ngưỡng thiết bị dư thừa
#define I 2000
#define q 0.75
#define mS_TO_S_FACTOR 1000

std::vector <String> allDevicesList;  // list cho phép trùng lặp
std::set <String> peersList;          // list không cho phép trùng lặp
RTC_DATA_ATTR BLEScan* pBLEScan;
RTC_DATA_ATTR int lastPeers = 0;
RTC_DATA_ATTR int scanWindow = BaseW;
RTC_DATA_ATTR int scanInterval = BaseI;
RTC_DATA_ATTR int timeToSleep;
RTC_DATA_ATTR int sleepMode = 0;      // 0: deepSleep ;  1: lightSleep

// Class callBack Thêm địa chỉ thiết bị quét được vào danh sách
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      allDevicesList.push_back(advertisedDevice.getAddress().toString().c_str()); 
      peersList.insert(advertisedDevice.getAddress().toString().c_str());
    }
};

void Scan(){
  Serial.println("Scanning...");
  BLEDevice::init("Hao");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setWindow(scanWindow/3);
  pBLEScan->setInterval(scanWindow/3);
  BLEScanResults foundDevices = pBLEScan->start(scanWindow/1000, false); // Đối số truyền vào kiểu integer
  Serial.println("Scan done!");
  pBLEScan->clearResults();
}

void hDiscovery(){
  int peers = peersList.size();
  int redundantPeers = allDevicesList.size()- peersList.size();
  if(redundantPeers > redundantN){
      scanWindow = SmallW;
      scanInterval = q * BaseI;
      sleepMode = 0;
  }
  else {
      if(peers > N){
        scanWindow = BaseW;
        sleepMode = 1;
      }
      else{
        scanWindow = SmallW;
        sleepMode = 0;
      }
      if(peers == 0 && lastPeers == 0)
        scanInterval = q * BaseI;
      else if (peers != 0 && lastPeers == 0)
        scanInterval = BaseI;
      else if (peers > lastPeers)
        scanInterval -= I;
      else if (peers < lastPeers)
        scanInterval += I;
  }
  lastPeers = peers; 
  Serial.println("Peers: " + String(peers));
  Serial.println("redundantPeers: " + String(redundantPeers));
  Serial.println("ScanWindow: " + String(scanWindow));
  Serial.println("ScanInterval: " + String(scanInterval));
}
void mulSleepMode(){
  timeToSleep =  scanInterval - scanWindow;
  esp_sleep_enable_timer_wakeup(timeToSleep * mS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(timeToSleep) + " Seconds"); 
  if (sleepMode == 0){
    Serial.println("Going to deep sleep now");
    Serial.flush();
    esp_deep_sleep_start();
  }
  else{
    Serial.println("Going to light sleep now");
    Serial.flush();
    esp_light_sleep_start();
  }
}
void setup() {
  Serial.begin(115200);
}

void loop() {
  Scan();
  hDiscovery();
  mulSleepMode();
}
