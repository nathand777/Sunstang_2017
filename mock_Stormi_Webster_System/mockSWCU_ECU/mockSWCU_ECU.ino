/*
 * Name:Steven Lawrence
 * Date: 02/19/18
 * Description: test ECU code for SW system. Takes in mode from SWCU and turns on SW module if in debugging mode then goes to sleep. Wakes up by an external interrupt
 * 
 * Current State: unfinished, have to figure out how to wake up from an interrupt. 
 * 
 */


#include<mcp_can.h>
#include<SPI.h>
#include<avr/sleep.h>

const int SPI_CS_PIN=10;

MCP_CAN CAN(SPI_CS_PIN);

//set wake up pin
int wakePin=2;

//wake up function
void wakeUpNow()
{
  delay(1000);
  byte readyMsg=0x02;
  CAN.sendMsgBuf(0x402,0,1, &readyMsg);
}

void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(wakePin),wakeUpNow,LOW);
  sleep_mode();
  sleep_disable();
  detachInterrupt(0);
}


//wake up SW module pin
int wakeSWModule=4;

//CAN stuff
unsigned char lenReceive=3;
unsigned char bufReceive;


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

  CAN.init_Mask(0,0,0x7ff);
  CAN.init_Mask(1,0,0x7ff);

  CAN.init_Filt(0,0,0x400);
  CAN.init_Filt(1,0,0x7ff);
  CAN.init_Filt(2,0,0x7ff);
  CAN.init_Filt(3,0,0x7ff);
  CAN.init_Filt(4,0,0x7ff);
  CAN.init_Filt(5,0,0x7ff);

  pinMode(wakePin,INPUT);
  pinMode(wakeSWModule,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(wakePin),wakeUpNow,RISING);
  
}

void loop() 
{
  if (CAN_MSGAVAIL==CAN.checkReceive())
  {
    CAN.readMsgBuf(&lenReceive, &bufReceive);
    Serial.println();
  }

  if(bitRead(bufReceive,0))
  {
    Serial.println("Normal mode!");
  }
  if (bitRead(bufReceive,1))
  {
    Serial.println("Charging mode!");
  }
  if(bitRead(bufReceive,2))
  {
    Serial.println("Debugging mode!");

    Serial.println("Waking up SW module");
    digitalWrite(wakeSWModule,HIGH); 
    Serial.println("ECU going to sleep");
    delay(100);
    sleepNow();
    
  }
}
