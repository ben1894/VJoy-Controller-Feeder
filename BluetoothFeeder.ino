#include <SoftwareSerial.h>
SoftwareSerial EEBlue(10, 11); // RX | TX
int x;
//byte x1;
byte x10;
byte x100;

int y;
//byte y1;
byte y10;
byte y100;

byte buttonStates = 0;

int buttonArray[] = {5,6,7};
int numOfButtons = sizeof(buttonArray)/sizeof(buttonArray[0]);

//0000 0001
//0000 0000 || = 1
//0000 0001

//0000 0001
//0000 0001 && = 0
//0000 0000

void setup()
{
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  //Serial.begin(9600); //38400
  EEBlue.begin(115200); 
}

void loop()
{
  //Each button state can be represented by one bit in a byte
  //Repeats for every button recording the states in byte buttonStates
  for(int x = 0; x < numOfButtons; x++)
  {
    buttonStates = buttonStates << 1;
    buttonStates |= !digitalRead(buttonArray[x]);
  }


  short x = map(analogRead(A1), 0, 1023, 0, 999);
  short y = map(analogRead(A0), 0, 1023, 0, 999);
  
  x100 = x/100;              
  x10  = x - (x100*100);       

  y100 = y/100;              
  y10  = y - (y100*100);       

  EEBlue.write(255);

  EEBlue.write(buttonStates);
  
  EEBlue.write(x100);
  EEBlue.write(x10);

  EEBlue.write(y100);
  EEBlue.write(y10);

  buttonStates = 0;
  delay(19);
}

  /*uint16_t number = 5703;               // 0001 0110 0100 0111
  uint16_t mask = B11111111;          // 0000 0000 1111 1111
  uint8_t first_half = number >> 8;   // >>>> >>>> 0001 0110
  uint8_t sencond_half = number & mask; // ____ ____ 0100 0111

  Serial.write(first_half);
  Serial.write(sencond_half);*/
