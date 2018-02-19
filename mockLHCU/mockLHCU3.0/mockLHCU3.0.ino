/*
 * Name: Steven Lawrence
 * Date: 02/19/18
 * Description: LHCU code 4.0 adding hazard and strobe functionalities
 */




#include "mcp_can.h"
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);

const int leftLed = 2;
const int rightLed = 3;
const int headLight = 5;
const int brakes = 6;
const int horn = 4;

unsigned long previousMillis = 0;
int blinkinterval = 500;
bool leftLedState;
bool rightLedState;

 unsigned char lenReceive = 0;
  unsigned char bufReceive[5]={0,0,0,0,0};
  unsigned char canId=0;

void setup() {
  Serial.begin(115200);

  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(headLight, OUTPUT);
  pinMode(brakes, OUTPUT);
  pinMode(horn, OUTPUT);



   while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS init fail");
        Serial.println("Init CAN BUS again");
        delay(100);
    }
    Serial.println("CAN BUS init ok!");

  CAN.init_Mask(0,0,0xff);
  CAN.init_Mask(1,0,0xff);

  CAN.init_Filt(0,0,0x8A);
  CAN.init_Filt(1,0,0x8B);
  CAN.init_Filt(2,0,0x8C);
  CAN.init_Filt(3,0,0x8D);
  CAN.init_Filt(4,0,0x8E);
  CAN.init_Filt(5,0,0x8F);
}

void loop() {
  unsigned long currentMillis = millis();

  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBuf(&lenReceive, bufReceive);    // read data,  len: data length, buf: data buffer

    canId = CAN.getCanId(); // read the message id associated with this CAN message (also called a CAN frame)

    // Print the data from the can message, including the ID it was associated with
    Serial.print("Sensor Value from ID: ");
    Serial.print(canId,BIN);
    Serial.print(" = ");

    for (int i = 0; i < lenReceive; i++)    // print the data
    {
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }
    Serial.println();
  }

     if(bufReceive[0]){
      Serial.println("HORN");
      digitalWrite(horn, HIGH);
    }
    else
      digitalWrite(horn, LOW);

    if(bufReceive[1]){
      Serial.println("LEFT");

      if (currentMillis - previousMillis >= blinkinterval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
   if (leftLedState == LOW) {
      leftLedState = HIGH;
    } else {
      leftLedState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(leftLed, leftLedState);
    
    }
    }
    else digitalWrite(leftLed,LOW);
    
      
    if(bufReceive[2]){
      Serial.println("RIGHT");
      if (currentMillis - previousMillis >= blinkinterval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
   if (rightLedState == LOW) {
      rightLedState = HIGH;
    } else {
      rightLedState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(rightLed, rightLedState);
    
    }
    }
     else digitalWrite(rightLed,LOW);
    
    if(bufReceive[3]){
      Serial.println("HEADLIGHT");
      digitalWrite(headLight, HIGH);
    }
    else
      digitalWrite(headLight, LOW);
      
    if(bufReceive[4]){
      Serial.println("BRAKES");
      digitalWrite(brakes, HIGH);
    }
    else
      digitalWrite(brakes, LOW);
  

}


