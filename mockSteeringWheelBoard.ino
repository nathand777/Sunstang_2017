#include <mcp_can.h>
#include <SPI.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);

const int left = 3;
const int right = 4;
const int headLight = 6;
const int brakes = A7; 
const int horn = 2;

uint32_t tNow = 0;
uint32_t tPrev = 0;

bool headLightState, brakeState, hornState;
bool leftState, rightState;
bool xHeadLight_State(LOW), xBrakeState(LOW), xHornState(LOW);
bool xRight_State(LOW), xLeft_State(LOW);
bool headSig, brakeSig, hornSig;
bool rightSig, leftSig;

long leftDebounce(0), rightDebounce(0), headLightDebounce(0), brakeDebounce(0), hornDebounce(0);
long debounceDelay = 50;

void readButtn(int button, bool &lastButtonState, bool &buttonState, bool &output, long & lastDebounceTime)
{
    // read the state of the switch into a local variable:
  int reading = digitalRead(button);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        output = !output;
      }
    }
 }
 lastButtonState = reading;
} 

void setup() {
  Serial.begin(115200);

  pinMode(left, INPUT);
  pinMode(right, INPUT);
  pinMode(headLight, INPUT);
  pinMode(brakes, INPUT);
  pinMode(horn, INPUT);

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

  CAN.init_Filt(0,0,0xC1);
  CAN.init_Filt(1,0,0xC2);
  CAN.init_Filt(2,0,0xC3);
  CAN.init_Filt(3,0,0xC4);
  CAN.init_Filt(4,0,0xC5);
  CAN.init_Filt(5,0,0xC6);
}

void loop() {

  readButtn(left, xLeft_State, leftState, leftSig, leftDebounce);
  readButtn(right, xRight_State, rightState, rightSig, rightDebounce);
  readButtn(headLight, xHeadLight_State, headLightState, headSig, headLightDebounce);
  readButtn(brakes, xBrakeState, brakeState, brakeSig, brakeDebounce); 
  readButtn(horn, xHornState, hornState, hornSig, hornDebounce);
  
  tNow = millis();
  int sendID = 0x8D;
  if (tNow - tPrev > 50) {
    tNow = tPrev;

    const byte LENGTH_SEND = 5; // the number of bytes (recall: 8 bits = 1 byte) to send
    byte bufSend[LENGTH_SEND];  // the buffer to hold the data we are going to send.
    
    if (leftSig)
      bufSend[0] = 1;
    else
      bufSend[0] = 0;
    if (rightSig)
      bufSend[1] = 1;
    else
      bufSend[1] = 0;
    if (hornSig)
      bufSend[2] = 1;
    else
      bufSend[2] = 0;
    if (brakeSig)
      bufSend[3] = 1;
    else
      bufSend[3] = 0;
    if (hornSig)
      bufSend[4] = 1;
    else
      bufSend[4] = 0;
    Serial.println("sending!");
    CAN.sendMsgBuf(sendID, 0, LENGTH_SEND, bufSend);
  }
}
