#include <mcp_can.h>
#include <SPI.h>
#include <SD.h>
const int chipSelect = 9;
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

unsigned long scuID = 0x8d;

void setup(){
  Serial.begin(9600);
  while (!Serial){
    ;
  }

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

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

  CAN.init_Mask(0,0,0xff);
  CAN.init_Mask(1,0,0xff);

  CAN.init_Filt(0,0,0x8d);
  CAN.init_Filt(1,0,0xa1);
  CAN.init_Filt(2,0,0xa1);
  CAN.init_Filt(3,0,0xa1);
  CAN.init_Filt(4,0,0xa1);
  CAN.init_Filt(5,0,0xa1);
}

unsigned char lenReceive = 0;
unsigned char bufReceive[5]={0,0,0,0,0};
unsigned char canId=0;

int leftCount = 0, rightCount = 0, brakeCount = 0, headCount = 0;

void loop(){
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

    //check which input was received
  if(bufReceive[1]){
      Serial.println("LEFT ON");
      leftCount++;
  }
  if(bufReceive[2]){
      Serial.println("RIGHT ON");
      rightCount++;
  }
  if(bufReceive[3]){
    Serial.println("HEADLIGHT ON");
    headCount++; 
  }
  if(bufReceive[4]){
      Serial.println("BRAKES ON");
      brakeCount++;
  }
    
    File dataFile = SD.open("COUNT.csv", FILE_WRITE);
    if (dataFile){
      dataFile.print(leftCount);
      dataFile.print(",");
      dataFile.print(rightCount);
      dataFile.print(",");
      dataFile.print(headCount);
      dataFile.print(",");
      dataFile.println(brakeCount);
      dataFile.close();
    }
    else{
      Serial.println("ERROR");
    }
  }  
}
