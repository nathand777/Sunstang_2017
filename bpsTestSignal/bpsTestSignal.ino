//USE THIS TO TEST BPS SIGNAL FOR LIGHTS AND HORNS BOARD
#include <mcp_can.h>
#include <SPI.h>



const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

const int switched = 4;
const int strobe = 5;

bool currentSwitch;
bool currentStrobe;
bool lastSwitch = 0;
bool lastStrobe = 0;

byte sendMsg = 0;

void setup(){
  Serial.begin(9600);

  pinMode(switched, INPUT);
  pinMode(strobe, INPUT);
  
  //initialize CAN 
START_INIT:

  if (CAN_OK == CAN.begin(CAN_500KBPS))
  {
    Serial.println("CAN BUS Shield init ok!");
  }
  else
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
    goto START_INIT;
  } 

   CAN.init_Mask(0,0,0x7ff);
   CAN.init_Mask(1,0,0x7ff);

   CAN.init_Filt(0,0,0xE1);
   CAN.init_Filt(1,0,0xE2);
   CAN.init_Filt(2,0,0xE3);
   CAN.init_Filt(3,0,0xE4);
   CAN.init_Filt(4,0,0xE5);
   CAN.init_Filt(5,0,0xE6);
}

void loop(){
   currentSwitch = digitalRead(switched);
   currentStrobe = digitalRead(strobe);

   //check if switched has changed states
   if(currentSwitch != lastSwitch){
    if(!currentStrobe){
      if(currentSwitch)
        bitSet(sendMsg,0);
      else
        bitClear(sendMsg,0);
    }
    else if (currentStrobe)
      bitClear(sendMsg, 0);

    CAN.sendMsgBuf(0x100,0,1,&sendMsg);
   }

   //check if strobe has changed 
   if (currentStrobe != lastStrobe){
    if(currentStrobe)
      bitSet(sendMsg,1);
    else
      bitClear(sendMsg,1);

    CAN.sendMsgBuf(0x100,0,1,&sendMsg);
   }

   
}

