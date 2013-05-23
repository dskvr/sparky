#include <SPI.h>
#include <Ethernet.h>
#include <Shifter.h>

#define SER_Pin 5 //SER_IN
#define RCLK_Pin 3 //L_CLOCK
#define SRCLK_Pin 2 //CLOCK
#define NUM_REGISTERS 1 //how many registers are in the chain

Shifter shifter(SER_Pin, RCLK_Pin, SRCLK_Pin, NUM_REGISTERS); 

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip = IPAddress(192,168,1,111);
byte server[] = { 192,168,1,100 };

EthernetClient client;

bool flame = false;
bool intval = 100;
bool connected = false;

int ti = millis();
int lastTrigger = 0;
int sinceLast = 0;

char mode = 'on';

void flameon(){
	shifter.setPin(0, HIGH); shifter.write(); 
	char mode = 'off';
	lastTrigger = millis();
}
void flameoff(){
	shifter.setPin(0, LOW); shifter.write(); 
	char mode = 'on';
	lastTrigger = millis();
}

void setup() {
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
  // start the serial library:
  Serial.begin(9600);
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 1337)) {
    Serial.println("connected");
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}

void loop()
{
	
	ti = millis();
	sinceLast = ti - lastTrigger;
  
  //if you are connected and data is available
	if (client.available()) {
	    
			char c = client.read();
			
			if(c == '0') {flame = false;}
			if(c == '1') {flame = true;interval = 30;}
			if(c == '2') {flame = true;interval = 85;}
			if(c == '3') {flame = true;interval = 140;}
			if(c == '4') {flame = true;interval = 200;}
			if(c == '5') {flame = true;interval = 300;}
			if(c == '6') {flame = true;interval = 410;}
			if(c == '7') {flame = true;interval = 580;}
			if(c == '8') {flame = true;interval = 750;}
			if(c == '9') {flame = true;interval = 1111;}
			
			Serial.print(c);
			client.write(interval);
			
			
			if(flame = true) {
				if( sinceLast > intval && mode == 'on' ) { flameon(); }
				if( sinceLast > intval && mode == 'off' ) { flameoff(); }
			}
			
			if(flame = false) {
				if( sinceLast > intval ) { flameoff(); }
			}
			
	  }

	  if (!client.connected()) {
	    Serial.println();
	    Serial.println("disconnecting.");
	    client.stop();
	    for(;;)
	      ;
	  }

}
