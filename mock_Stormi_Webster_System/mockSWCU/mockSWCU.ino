/*
 * Name: Steven Lawrence
 * Date: 02/17/18
 * Description: mock SWCU (Stormi Webster Control Unit) code; This code will send a message through CAN to all the ECU's to interrupt the SW module out of sleep mode and 
 * putting itself to sleep. It will then send another message to the SW module to report back if it is on. 
 * 
 * Current State: finished
 */

#include<mcp_can.h>
#include<SPI.h>

const int SPI_CS_PIN=10;

MCP_CAN CAN(SPI_CS_PIN);

/*mode 0 = normal operations
 * mode 1 = charging mode
 * mode 2 = debugging mode
 */
int mode=0, lastMode=0;
byte sendMsg;

//recieve CAN
unsigned char lenReceive=0;
byte bufReceive;

//set pins
const int debugPin=6;
const int chargePin=7;
bool debugOn;
bool chargeOn;

void setup() 
{

  //init Serial 
  Serial.begin(115200);

  //init can bus : baudrate = 500k
  while(CAN_OK != CAN.begin(CAN_500KBPS))
  {
    Serial.println("CAN BUS init fail");
    Serial.println("CAN BUS init fail again");
    delay(100);
  }
  Serial.println("CAN BUS init ok!");
  Serial.println("Normal mode!");

  CAN.init_Mask(0,0, 0x7ff);
  CAN.init_Mask(1,0, 0x7ff);

  CAN.init_Filt(0,0, 0x401);
  CAN.init_Filt(1,0, 0x402);
  
  CAN.init_Filt(2,0, 0x7ff);
  CAN.init_Filt(3,0, 0x7ff);
  CAN.init_Filt(4,0, 0x7ff);
  CAN.init_Filt(5,0, 0x7ff);

  pinMode(debugPin, INPUT);
  pinMode(chargePin, INPUT);

}


void loop() 
{
debugOn=digitalRead(debugPin);
chargeOn=digitalRead(chargePin);
  
if(debugOn)
{
mode=2;
}
if(chargeOn)
{
mode=1;
}
if((!debugOn)&&(!chargeOn))
{
  mode=0;
}

if(mode != lastMode)
{
lastMode=mode;
switch(mode)
{
  case 0:
  {
    Serial.println("Normal mode!");
    bitSet(sendMsg,0);
    bitClear(sendMsg,1);
    bitClear(sendMsg,2);
    for(int i=0; i<3; i++)
    {
      Serial.print(bitRead(sendMsg,i));
      Serial.print(" ,");
    }
    Serial.println();
    CAN.sendMsgBuf(0x400,0,1,&sendMsg);
    break;
  }
  case 1:
  {
    Serial.println("Charging mode!");
    bitClear(sendMsg,0);
    bitSet(sendMsg,1);
    bitClear(sendMsg,2);
    for(int i=0; i<3; i++)
    {
      Serial.print(bitRead(sendMsg,i));
      Serial.print(" ,");
    }
    Serial.println();
    CAN.sendMsgBuf(0x400,0,1,&sendMsg);
    break;
  }
  case 2:
  {
    Serial.println("Debugging mode!");
    bitClear(sendMsg,0);
    bitClear(sendMsg,1);
    bitSet(sendMsg,2);
    for(int i=0; i<3; i++)
    {
      Serial.print(bitRead(sendMsg,i));
      Serial.print(" ,");
    }
    Serial.println();
    CAN.sendMsgBuf(0x400,0,1,&sendMsg);
    break;
  }
  
}
}

if(CAN_MSGAVAIL==CAN.checkReceive())
{
  CAN.readMsgBuf(&lenReceive, &bufReceive);
}
if (bitRead(bufReceive,0))
{
  Serial.println("SW module up");
}
if (bitRead(bufReceive,1))
{
  Serial.println("ECU up");
}
}
