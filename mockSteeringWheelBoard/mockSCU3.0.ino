/*
 * Name: Steven Lawrence
 * Date: 18/02/18
 * Descriiption: mock SWCU 3.0, only sends message through CAN when an input changes state instead of constantly sending with cruise control reading
 * 
 */

#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);

//setting pins
const int Horn =2;
const int Left_Signal =3;
const int Right_Signal =4;
const int Head_Lights =5;
const int CC_Toggle =6;
const int CC_Set=7;
const int CC_Up =8;
const int CC_Down =9;

const int Accelerator =A2;
const int Brakes =A5;


//initializing variables

//Horn button variables
bool prevHorn, HornState, HornOut;

//Left and Right turn signals
bool LeftOut;
bool RightOut;

bool LastLeft, LastRight, LastBrake, hornOn, headOn;

//Headlight button variables
bool prevHead, HeadState, HeadOut;

//Cruise Control button variables
bool prevCC_Tog, CC_TogState, CC_TogOut;
bool prevCC_Set, CC_SetState, CC_SetOut;
bool prevCC_Up, CC_UpState, CC_UpOut;
bool prevCC_Down, CC_DownState, CC_DownOut; 

//Acceleration potentiometer variables
int Acc_Raw,AccPot_Min(560),AccPot_Max(995);
float Acc_Float;

//brake signal
bool brakesOut;

//debouncing variables
const int debounceDelay = 50;
long lastHorn(0), lastAcc(0), lastHead(0), lastCC_Tog(0), lastCC_Set(0), lastCC_Up(0), lastCC_Down(0);

void readButtn(int button, bool &lastButtonState, bool &buttonState, bool &output, long & lastDebounceTime)
{
    // read the state of the switch into a local variable:
  int reading = digitalRead(button);
  
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
   
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
  
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) 
    {
     
      buttonState = reading;

         //Change Output to what the reading is
         output = reading;
    }
 }
 lastButtonState=reading;
}


//toggle read button
void togReadButtn(int button, bool &lastButtonState, bool &buttonState, bool &output, long & lastDebounceTime)
{
    // read the state of the switch into a local variable:
  int reading = digitalRead(button);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) 
    {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) 
      {
        output = !output;
      }
    }
 }
  lastButtonState=reading;
} 

// Read the potentiometers
void PotRead(int Pin, int &Raw, float &Float, int &Min, int &Max) {
  Raw = analogRead(Pin);
  
  if (Raw < Min) Raw = Min;
  if (Raw > Max) Raw = Max;
  
  Float = (float)(Raw-Min)/(Max-Min);
}

//message to LHCU
byte bufSend[5];
int LHCUsendID = 0x8D;

void setup() 
{
pinMode(Horn, INPUT);
pinMode(Left_Signal, INPUT);
pinMode(Right_Signal, INPUT);
pinMode(Head_Lights, INPUT);
pinMode(CC_Toggle, INPUT);
pinMode(CC_Set, INPUT);
pinMode(CC_Up, INPUT);
pinMode(CC_Down, INPUT);
pinMode(Accelerator, INPUT);
pinMode(Brakes, INPUT);

Serial.begin(115200);


while(CAN_OK != CAN.begin(CAN_500KBPS))
{
  Serial.println("CAN BUS init fail");
  Serial.println("CAN BUS init fail again");
  delay(100);
}
Serial.println("CAN BUS init ok!");

//inititalize mcp2515 masks and filters
  CAN.init_Mask(0,0,0xff);
  CAN.init_Mask(1,0,0xff);

  CAN.init_Filt(0,0,0xC1);
  CAN.init_Filt(1,0,0xC2);
  CAN.init_Filt(2,0,0xC3);
  CAN.init_Filt(3,0,0xC4);
  CAN.init_Filt(4,0,0xC5);
  CAN.init_Filt(5,0,0xC6);

}

void loop() 
{
//reading all the switches and buttons
readButtn(Horn, prevHorn, HornState, HornOut, lastHorn);  

LeftOut = digitalRead(Left_Signal); 
RightOut = digitalRead(Right_Signal); 

togReadButtn(Head_Lights, prevHead, HeadState, HeadOut, lastHead); 

//read cruise control buttons
/*readButtn(CC_Toggle, prevCC_Tog, CC_TogState, CC_TogOut, lastCC_Tog); 
readButtn(CC_Set, prevCC_Set, CC_SetState, CC_SetOut, lastCC_Set); 
readButtn(CC_Up, prevCC_Up, CC_UpState, CC_UpOut, lastCC_Up ); 
readButtn(CC_Down, prevCC_Down, CC_DownState, CC_DownOut, lastCC_Down ); */

// Read and process the potentiometers
PotRead(Accelerator, Acc_Raw, Acc_Float, AccPot_Min, AccPot_Max);

brakesOut = digitalRead(Brakes);

if(HornOut)
{
  hornOn=HornOut;
  bufSend[0]= HornOut;
  Serial.println("Horn triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();
  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}
if(HornOut!=hornOn)
{
  hornOn=HornOut;
  bufSend[0]=HornOut;
  Serial.println("Horn triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();
  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}


if(LeftOut!=LastLeft)
{
  LastLeft=LeftOut;
  bufSend[1]= LeftOut;
  Serial.println("Left signal triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();

  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}

if(RightOut!=LastRight)
{
  LastRight=RightOut;
  bufSend[2]= RightOut;
  Serial.println("Right signal triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();

  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}
if(HeadOut)
{
  headOn=HeadOut;
  bufSend[3]= HeadOut;
  Serial.println("Headlight triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();

  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}

if(HeadOut!=headOn)
{
  headOn=HeadOut;
  bufSend[3]=HeadOut;
  Serial.println("Head triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();
  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}

if(brakesOut!=LastBrake)
{
  LastBrake=brakesOut;
  bufSend[4]= brakesOut;
  Serial.println("Brakes triggered");
  Serial.print("Sending: ");
for (int i = 0; i < 5; i++)    // print the data
    {
      Serial.print(bufSend[i]);
      Serial.print("\t");
    }
    Serial.println();

  CAN.sendMsgBuf(LHCUsendID,0,5,bufSend);
}
}


