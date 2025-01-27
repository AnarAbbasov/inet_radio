#include <VS1053Driver.h>


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>


int CS = D2;
int DCS = D3;
int DREQ = D1;
VS1053 decoder(CS, DCS, DREQ);
int statPin = D8;
int volPin = D4;
WiFiClientSecure client;
char ssid[] = "Netzwerk";
char password[] = "TedMiller@2023";
const int maxStat = 4;
String stationName[] = {"Araz", "yurdfm", "ClassicFM", "mugam"};
char * host[maxStat] = {"relay1.slayradio.org",
                        "icecast.livetv.az",
                        "media-ice.musicradio.com",
                        "icecast.livetv.az"};
char * path[maxStat] = {"/antennfm",
                        "/yurdfm",
                        "/ClassicFMMP3",
                        "/mediamugam"};
int port[] = {443, 443, 80, 443};
unsigned char mp3buff[12000];
int station = 0;
int volume = 0;
volatile int newStation = 0;
volatile int newVolume = 100;

void setup() {
    Serial.begin(9600);
    SPI.begin();
    decoder.begin();
    decoder.switchToMp3Mode();
    //decoder.streamModeOn();
    decoder.setVolume(volume);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println("WiFi connected");
    pinMode(volPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(statPin), chan, RISING);
    attachInterrupt(digitalPinToInterrupt(volPin), vol, FALLING);
    client.setInsecure();  // Optional: Accept any server certificate (for testing purposes only)
}

void loop() {
    if (station != newStation) {
        station = newStation;
        Serial.print("connecting to CH"); Serial.print(station);
        Serial.print(" "); Serial.println(stationName[station]);
        if (client.connect(host[station], port[station])) {
            Serial.println("connected");
            client.println(String("GET ") + path[station] + " HTTP/1.1");
            Serial.println(String("GET ") + path[station]);
            client.println(String("Host: ") + host[station]);
            Serial.println(String("Host ") + host[station]);
            client.println("Connection: close");
            client.println();
        }
    }
    if (volume != newVolume) {
        volume = newVolume;
        Serial.print("volume "); Serial.println(volume);
        decoder.setVolume(volume);
    }
    if (client.available() > 0) {
        
       uint8_t bytesread1= client.read(mp3buff, 200);
       uint8_t bytesread2= client.read(mp3buff+bytesread1, 200);
        uint8_t bytesread3= client.read(mp3buff+bytesread1+bytesread2, 200);
        //Serial.println(bytesread1+bytesread2);
        
        decoder.writeAudio(mp3buff, bytesread1+bytesread2+bytesread3);
        
        
    }
}

IRAM_ATTR void chan() {
    newStation++;
    if (newStation > maxStat - 1) newStation = 0;
}

IRAM_ATTR void vol() {
    newVolume = newVolume + 5;
    if (newVolume > 101) newVolume = 50;
}
