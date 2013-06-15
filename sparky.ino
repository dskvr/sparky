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

bool flame = 0;
int intval = 100;

void setup() {
  // start the Ethernet connection:
  Ethernet.begin(mac);
  // start the serial library:
  Serial.begin(9600);
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 1337)) {
    Serial.println("connected"); Serial.flush();
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed"); Serial.flush();
  }

}

int ti = millis();
int lastTrigger = ti;
int sinceLast = 0;

bool on = 1;
char c;
int fixedIntval = 50;

int totalvalves = 9;
int flame[totalvalves];

int valvestatus[9] = {0,0,0,0,0,0,0,0,0};

int currenteffect = 0;
int currentframe = 0;
int totaleffects = 5;
int totalframes = 9;
int effects[totaleffects][totalframes][totalvalves] = {
	{ 
		//0 Tracer
		{1,0,0,0,0,0,0,0,0},
		{0,1,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0,0},
		{0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0},
		{0,0,0,0,0,1,0,0,0},
		{0,0,0,0,0,0,1,0,0},
		{0,0,0,0,0,0,0,1,0},
		{0,0,0,0,0,0,0,0,1}
	},
	{ //1 a
	 	{1,0,0,0,0,0,0,0,0},
		{0,1,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0,0},
		{0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,1,0},
		{0,0,0,0,0,0,1,0,0},
		{0,0,0,0,0,1,0,0,0},
		{0,0,0,0,1,0,0,0,0}
	},
	{ //2 b
		{0,0,0,1,0,0,0,0,0},
		{1,0,1,0,0,0,1,0,1},
		{0,1,0,1,0,1,0,1,0},
		{0,0,0,0,1,0,0,0,0},
		{0,0,1,0,0,0,1,0,0},
		{1,0,0,0,0,0,1,0,0},
		{0,0,1,0,0,0,1,0,0},
		{1,0,0,0,0,0,1,0,0},
		{0,0,1,0,0,0,1,0,0}
	},
	{ //3 c
		{0,0,0,0,1,0,0,0,0},
		{1,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,1},
		{0,0,1,0,0,0,0,0,0},
		{0,0,0,0,0,0,1,0,0},
		{0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,1,0,0,0},
		{0,0,0,0,0,0,0,1,0},
		{0,1,0,0,0,0,0,0,0}
	},
	{//4 d
		{1,0,0,0,0,1,1,0,0},
		{1,0,0,0,1,0,0,0,1},
		{1,1,1,0,0,0,0,0,0},
		{0,0,1,0,1,0,1,0,0},
		{0,0,1,1,0,0,0,0,1},
		{1,0,0,0,1,0,0,0,1},
		{1,1,1,0,0,0,0,0,0},
		{0,0,1,0,1,0,1,0,0},
		{1,0,0,1,1,0,1,0,0}	
	},
	{ //5 e
		{0,1,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0},
		{0,0,0,0,0,0,0,1,0},
		{1,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,1,0,0,0},
		{0,0,0,0,0,0,1,0,0},
		{0,0,1,0,0,0,0,0,0},
		{0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,1}
	}
} 

// char* sequences[5];

void read(char c){
	//Speed Settings
	if(c) {
		if(c == '0') {flame=0;}
		if(c == '1') {flame=1; intval = 1111;}
		if(c == '2') {flame=1; intval = 750;}
		if(c == '3') {flame=1; intval = 580;}
		if(c == '4') {flame=1; intval = 410;}
		if(c == '5') {flame=1; intval = 300;}
		if(c == '6') {flame=1; intval = 200;}
		if(c == '7') {flame=1; intval = 140;}
		if(c == '8') {flame=1; intval = 85;}
		if(c == '9') {flame=1; intval = 30;}
	}
	//effects
}

void loop()
{
	
	ti = millis();
	sinceLast = ti - lastTrigger;
  
  //if you are connected and data is available
	if (client.available()) {
		c = client.read();	
	}
	  
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    delay(5000);
  }

	//Read incoming bytes.
	read(c);
	
	//Iterate through effect frames
	if(flame==1) {
		for(int v;v<totalvalves;v++){
			// Serial.println("on");
			on = effect[currenteffect][currentframe][v];
			if( sinceLast >= intval && on == '1') { 
	      shifter.setPin(v, HIGH); shifter.write();
				valvestatus[v] = '1';
	     } else if ( (sinceLast >= fixedIntval && valvestatus[v] == '1') || on == '0') 
			 { //the valve is on, but it's time to go off; 
	      shifter.setPin(v, LOW); shifter.write(); 
				valvestatus[v] = '0';
	     }
		}
		lastTrigger = millis();
		currentframe++;
		currentframe = (currentframe >= totalframes) ? 0 : currentframe;
	}

	if(flame==0) {
		// Serial.println("off"); 
		Serial.flush();
		for(int v;v<totalvalves;v++) {
			shifter.setPin(v, LOW); shifter.write(); 
		}
		currenteffect++; 
		currenteffect = (currenteffect < totaleffects) ? currenteffect : 0;
	}
	Serial.flush();

}
