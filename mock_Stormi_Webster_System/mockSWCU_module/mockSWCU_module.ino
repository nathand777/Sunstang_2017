 /*
 * Name:Steven Lawrence
 * Date: 02/19/18
 * Description: test SW module code for SW system. goes to sleep right off start up and waits to be awaken by an external interrupt and takes over CAN communication
 * upon changing back to normal mode, the module will wake up the ECU and put itself to sleep
 * 
 * Current State: unfinished, have to figure out how to wake the ECU up and there is a problem with the module entering the wake up function when it goes to sleep. 
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
  Serial.println("waking up");
  delay(1000);
  byte readyMsg=0x1;
  CAN.sendMsgBuf(0x401,0, 1, &readyMsg);
}

//wake ECU pin;
int wakeECUPin=4;

//sleep function;
void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(digitalPinToInterrupt(wakePin),wakeUpNow,LOW);
  sleep_mode();
  sleep_disable();
  detachInterrupt(0);
}

//CAN stuff
unsigned char lenReceive=0;
byte bufReceive;

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
  pinMode(wakeECUPin,OUTPUT);
 

  attachInterrupt(digitalPinToInterrupt(wakePin),wakeUpNow,LOW);

  Serial.println("going to sleep");
  delay(100);
  sleepNow();

}

void loop() 
{
  if(CAN_MSGAVAIL==CAN.checkReceive())
  {
    CAN.readMsgBuf(&lenReceive, &bufReceive);
    for(int i=0; i<3; i++)
    {
      Serial.print(bitRead(bufReceive,i));
      Serial.print(" ,");
    }
    Serial.println();
  }

  if(bitRead(bufReceive,0))
  {
    Serial.println("Normal mode!");
    Serial.println("Waking up  ECU");
    digitalWrite(wakeECUPin,HIGH);
    Serial.println("SW module going to sleep");
    delay(100);
    sleepNow();
  }
  if (bitRead(bufReceive,1))
  {
    Serial.println("Charging mode!");
  }
  if(bitRead(bufReceive,2))
  {
    Serial.println("Debugging mode!");
  }
}
