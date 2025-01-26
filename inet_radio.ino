#include <VS1053Driver.h>

//#include <VS1053.h>




// include VS1053 library
#include <ESP8266WiFi.h> 			
// include ESP8266WiFi library
int CS = D2;
int DCS = D3; 				
// define VS1053 decoder pins
int DREQ = D1;
VS1053 decoder(CS, DCS, DREQ); 		
// associate decoder with VS1053
int statPin = D8; 				
// define switch pins for
int volPin = D4; 				
// station and volume
WiFiClient client; 				
// associate client and library
char ssid[] = "Netzwerk"; 			
// change xxxx to Wi-Fi ssid
char password[] = "TedMiller@2023"; 			
// change xxxx to Wi-Fi password
const int maxStat = 4; 			
// number of radio stations
String stationName[] = {"somafm", "slayradio2", "ClassicFM", "slayradio4"};
char * host[maxStat] = {"ice2.somafm.com", // station host
"ice2.somafm.com",
"media-ice.musicradio.com",
"bbcmedia.ic.llnwd.net"};
char * path[maxStat] = {"/synphaera-64-aac",
"/synphaera-128-aac",
"/ClassicFMMP3",
"/synphaera-32-aac"};
int port[] = {80,80,80,80}; 		
// default station port is 80
unsigned char mp3buff[200]; 			
// VS1053 loads data in 32 bytes
int station = 0;
int volume = 0; 				
// volume level 0-100
volatile int newStation = 2; 			
// station number at start up
volatile int newVolume = 80; 			
// volume at start up



void setup() {
  // put your setup code here, to run once:
Serial.begin(115200); 		
// Serial Monitor baud rate
SPI.begin(); 			
// initialise SPI bus
decoder.begin(); 			
// initialise VS1053 decoder
decoder.switchToMp3Mode();
// MP3 format mode
decoder.setVolume(volume);
// set decoder volume
WiFi.begin(ssid, password);
// initialise Wi-Fi
while (WiFi.status() != WL_CONNECTED) delay(500);
Serial.println("WiFi connected"); // wait for Wi-Fi connection
pinMode(volPin, INPUT_PULLUP);
// switch pin uses internal
					// pull-up resistor
attachInterrupt(digitalPinToInterrupt(statPin), chan, RISING);
attachInterrupt(digitalPinToInterrupt(volPin), vol, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
if(station != newStation)
// new station selected
{
station = newStation;
// display updated station name
Serial.print("connecting to CH"); Serial.print(station);
Serial.print(" ");Serial.println(stationName[station]);
if(client.connect(host[station], port[station]));
{ 				
// connect to radio station URL
client.println(String("GET ")+ path[station] + " HTTP/1.1");
Serial.println(String("GET ")+ path[station]);
client.println(String("Host: ") + host[station]);
client.println("Connection: close");
client.println(); 	
Serial.print("connected"); 
// new line is required
}
}
if(volume != newVolume) 		
// change volume selected
{
volume = newVolume; 		
// display updated volume
Serial.print("volume ");Serial.println(volume);
decoder.setVolume(volume);
// set decoder volume
}
if(client.available() > 0)
// when audio data available
{				
 
// decode data 32 bytes at a time
uint8_t bytesread = client.read(mp3buff, 200);
decoder.writeAudio(mp3buff, bytesread);
}
}


IRAM_ATTR void chan() 		
// ISR to increment station number
{
newStation++;
if(newStation > maxStat-1) newStation = 0;
}					
// stations numbered 0, 1, 2...
IRAM_ATTR void vol() 			
// ISR to increase volume
{
newVolume = newVolume + 5;
if(newVolume > 101) newVolume = 50;
}	