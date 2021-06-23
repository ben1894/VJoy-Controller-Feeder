#include <SoftwareSerial.h>
SoftwareSerial EEBlue(10, 11); // RX | TX

unsigned int buttonStates = 0;

byte x100;
byte x10;
byte y100;
byte y10;

int xJoy;
int yJoy;

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
  //EEBlue.begin(115200); 
  EEBlue.begin(57600); 

}

void loop()
{
  //Each button state can be represented by one bit
  //Repeats for every button recording the states in byte buttonStates
  for(int x = 0; x < numOfButtons; x++)
  {
      buttonStates = buttonStates << 1;
      buttonStates |= !digitalRead(buttonArray[x]);
  }

  xJoy = analogRead(A1);
  yJoy = analogRead(A2);
  
  x100 = xJoy / 100;
  x10 = xJoy - (x100 * 100);
  y100 = yJoy / 100;
  y10 = yJoy - (y100 * 100);
   
  EEBlue.write((byte)255);

  EEBlue.write((byte)buttonStates);
  EEBlue.write((byte)(buttonStates >> 8));
  
  EEBlue.write((byte)x10);
  EEBlue.write(x100);
  EEBlue.write(y10);
  EEBlue.write(y100);

  buttonStates = 0;

  delay(10);
}

  /*uint16_t number = 5703;               // 0001 0110 0100 0111
  uint16_t mask = B11111111;          // 0000 0000 1111 1111
  uint8_t first_half = number >> 8;   // >>>> >>>> 0001 0110
  uint8_t sencond_half = number & mask; // ____ ____ 0100 0111

  Serial.write(first_half);
  Serial.write(sencond_half);*/
