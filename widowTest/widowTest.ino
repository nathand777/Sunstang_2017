#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);

const int supp = 2;
const int dc = 3;
const int switched = 4;

void setup() {
  Serial.begin(9600);
  pinMode(supp, OUTPUT);
  pinMode(dc, OUTPUT);
  pinMode(switched, INPUT);
  digitalWrite(supp, HIGH);

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

  CAN.init_Filt(0,0,0xD1);
  CAN.init_Filt(1,0,0xD2);
  CAN.init_Filt(2,0,0xD3);
  CAN.init_Filt(3,0,0xD4);
  CAN.init_Filt(4,0,0xD5);
  CAN.init_Filt(5,0,0xD6);

  
}

void loop() {
    if(digitalRead(switched)){
      digitalWrite(dc, HIGH);
      delay(500);
      digitalWrite(supp, LOW);
    }
    else{
      digitalWrite(dc, LOW);
      digitalWrite(supp, HIGH);
    }

    byte lenReceive = 0;
  // must always be 8 bytes long, since that is the maximum length a CAN message can be.
  // Since we don't know what the length of incoming can messages will be beforehand, we cannot
  // set the message buffer to anything less than the maximum size of a can message (which is 8 bytes).
  byte bufReceive[8];

  if (CAN_MSGAVAIL == CAN.checkReceive()) // check if data has arrived on the CAN bus
  {
    CAN.readMsgBuf(&lenReceive, bufReceive);    // read data,  len: data length, buf: data buffer

    unsigned char canId = CAN.getCanId(); // read the message id associated with this CAN message (also called a CAN frame)

    //setLeds(canId, bufReceive[0]);

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
  }
}
