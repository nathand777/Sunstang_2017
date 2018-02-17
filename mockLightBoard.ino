#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);

const int leftLed = 2;
const int rightLed = 3;
const int headLight = 5;
const int brakes = 6;
const int horn = 4;

void setup() {
  Serial.begin(115200);

  pinMode(leftLed, OUTPUT);
  pinMode(rightLed, OUTPUT);
  pinMode(headLight, OUTPUT);
  pinMode(brakes, OUTPUT);
  pinMode(horn, OUTPUT);

START_INIT:

  if (CAN_OK == CAN.begin(CAN_500KBPS)) // init can bus : baudrate = 500k
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

  CAN.init_Mask(0,0,0xff);
  CAN.init_Mask(1,0,0xff);

  CAN.init_Filt(0,0,0x8A);
  CAN.init_Filt(1,0,0x8B);
  CAN.init_Filt(2,0,0x8C);
  CAN.init_Filt(3,0,0xBD);
  CAN.init_Filt(4,0,0x8E);
  CAN.init_Filt(5,0,0x8F);
}

void loop() {
  byte lenReceive = 0;
  byte bufReceive[8];
  unsigned char canId=0;

  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBuf(&lenReceive, bufReceive);    // read data,  len: data length, buf: data buffer

    CAN.getCanId(); // read the message id associated with this CAN message (also called a CAN frame)

    // Print the data from the can message, including the ID it was associated with
    Serial.print("Sensor Value from ID: ");
    Serial.print(canId);
    Serial.print(" = ");

    for (int i = 0; i < lenReceive; i++)    // print the data
    {
      Serial.print(bufReceive[i]);
      Serial.print("\t");
    }
    Serial.println();

    if(bufReceive[0]){
      Serial.println("LEFT");
      digitalWrite(leftLed, HIGH);
    }
    else
      digitalWrite(leftLed, LOW);
    if(bufReceive[1]){
      Serial.println("RIGHT");
      digitalWrite(rightLed, HIGH);
    }
    else 
      digitalWrite(rightLed, LOW);
    if(bufReceive[2]){
      Serial.println("HEADLIGHT");
      digitalWrite(headLight, HIGH);
    }
    else
      digitalWrite(headLight, LOW);
    if(bufReceive[3]){
      Serial.println("BRAKES");
      digitalWrite(brakes, HIGH);
    }
    else
      digitalWrite(brakes, LOW);
    if(bufReceive[4]){
      Serial.println("HORN");
      digitalWrite(horn, HIGH);
    }
    else
      digitalWrite(horn, LOW);
  }

}

