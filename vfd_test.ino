#include <SPI.h>
//#include <Serial.h>
#include <Streaming.h>

//Pixel Count
const unsigned int X_MAX = 125;
const unsigned int Y_MAX = 63;

//Display Commands
const byte CURSOR_HOME = 0x0B;
const byte SET_AREA = 0x11;
const byte CLEAR_AREA = 0x12;
const byte INVERT_AREA = 0x13;
const byte SET_OUTLINE = 0x14;
const byte CLEAR_OUTLINE = 0x15;
const byte SET_PIXEL = 0x16;
const byte CLEAR_PIXEL = 0x17;
const byte GRAPHIC_WRITE = 0x18;
const byte RESET = 0x19;
const byte WRITE_MODE = 0x1A;
//const byte WRITE_MACR0 = 0x1B;
const byte SET_BRIGHT = 0x1B;
const byte POWER_ON[2] = {0x1B, 0x50};
const byte POWER_OFF[2] = {0x1B, 0x46};

const byte CHAR_WRITE = 0x20;
const byte PROP_FONT = 0x1C;
const byte SMALL_FONT = 0x1D;
const byte LARGE_FONT = 0x1E;

const byte TEST_MODE = 0x0F;

//Display I/O Pins
const int DISP_CS = 22;
const int DISP_RES = 9;

//Debugging
unsigned long temp1;
const boolean DEBUG_1 = 0;  // Low Level - Function output, hardware registers
const boolean DEBUG_2 = 1;  // High Level - Timers, etc.


#include <WiFi.h>
#include "WiFiUdp.h" // not needed for WiFiNINA
#include "NTP.h"

char ssid[]     = "inMusic_Corp";
char password[] = "1nMusicW!F!";

volatile bool isTime = false;

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);


void dispReset()
{
  SPI.begin();
  digitalWrite(DISP_CS, LOW);
  SPI.transfer(RESET);
  delayMicroseconds(200);
  digitalWrite(DISP_CS, HIGH);
  SPI.end();
  Serial << "DISP_RESET" << endl;
}

// As a prefice to all the ugly 'IF' switches in here, the display has a quirk whereby you need to send any instance of 0x60 (Dec 96) twice for it to be recognized as a value, not a command.

void drawBox(int xtopleft, int ytopleft, int xbottomright, int ybottomright, int border)
{
  SPI.begin();
  digitalWrite(DISP_CS, LOW);
  delay(1);

  if (DEBUG_1) {
    Serial << "BOX: {[" << xtopleft << "," << ytopleft << "][" << xbottomright << "," << ybottomright << "]}, Border = " << border << endl;
  }

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  SPI.transfer(SET_AREA);
  delayMicroseconds(200);
  SPI.transfer(xtopleft);
  delayMicroseconds(200);
  if (xtopleft == 96)
  {
    SPI.transfer(xtopleft);
    delayMicroseconds(200);
  }

  SPI.transfer(ytopleft);
  delayMicroseconds(200);
  if (ytopleft == 96) {
    SPI.transfer(ytopleft);
    delayMicroseconds(200);
  }


  SPI.transfer(xbottomright);
  delayMicroseconds(200);
  if (xbottomright == 96) {
    SPI.transfer(xbottomright);
    delayMicroseconds(200);
  }

  SPI.transfer(ybottomright);
  delayMicroseconds(200);
  if (ybottomright == 96) {
    SPI.transfer(ybottomright);
    delayMicroseconds(200);
  }

  //2ms Delay needed after command excecution
  delay(2);

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  SPI.transfer(INVERT_AREA);
  delayMicroseconds(200);
  SPI.transfer(xtopleft + border);
  delayMicroseconds(200);
  if ((xtopleft + border) == 96)
  {
    SPI.transfer(xtopleft + border);
    delayMicroseconds(200);
  }

  SPI.transfer(ytopleft + border);
  delayMicroseconds(200);
  if ((ytopleft + border) == 96) {
    SPI.transfer(ytopleft + border);
    delayMicroseconds(200);
  }


  SPI.transfer(xbottomright - border);
  delayMicroseconds(200);
  if ((xbottomright - border) == 96) {
    SPI.transfer(xbottomright - border);
    delayMicroseconds(200);
  }

  SPI.transfer(ybottomright - border);
  delayMicroseconds(200);
  if ((ybottomright - border) == 96) {
    SPI.transfer(ybottomright - border);
    delayMicroseconds(200);
  }

  //2ms Delay needed after command excecution
  delay(2);
  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  digitalWrite(DISP_CS, HIGH);
  SPI.end();
}

void drawArea(int xtopleft, int ytopleft, int xbottomright, int ybottomright)
{
  SPI.begin();
  digitalWrite(DISP_CS, LOW);
  delay(1);

  if (DEBUG_1) {
    Serial << "AREA: {[" << xtopleft << "," << ytopleft << "][" << xbottomright << "," << ybottomright << "]}" << endl;
  }

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  SPI.transfer(SET_AREA);
  delayMicroseconds(200);
  SPI.transfer(xtopleft);
  delayMicroseconds(200);
  if (xtopleft == 96)
  {
    SPI.transfer(xtopleft);
    delayMicroseconds(200);
  }

  SPI.transfer(ytopleft);
  delayMicroseconds(200);
  if (ytopleft == 96) {
    SPI.transfer(ytopleft);
    delayMicroseconds(200);
  }


  SPI.transfer(xbottomright);
  delayMicroseconds(200);
  if (xbottomright == 96) {
    SPI.transfer(xbottomright);
    delayMicroseconds(200);
  }

  SPI.transfer(ybottomright);
  delayMicroseconds(200);
  if (ybottomright == 96) {
    SPI.transfer(ybottomright);
    delayMicroseconds(200);
  }

  //2ms Delay needed after command excecution
  delay(2);

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  digitalWrite(DISP_CS, HIGH);
  SPI.end();
}

void clearArea(int xtopleft, int ytopleft, int xbottomright, int ybottomright)
{
  SPI.begin();
  digitalWrite(DISP_CS, LOW);
  delay(1);

  if (DEBUG_1) {
    Serial << "CLEAR AREA: {[" << xtopleft << "," << ytopleft << "][" << xbottomright << "," << ybottomright << "]}" << endl;
  }

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  SPI.transfer(CLEAR_AREA);
  delayMicroseconds(200);
  SPI.transfer(xtopleft);
  delayMicroseconds(200);
  if (xtopleft == 96)
  {
    SPI.transfer(xtopleft);
    delayMicroseconds(200);
  }

  SPI.transfer(ytopleft);
  delayMicroseconds(200);
  if (ytopleft == 96) {
    SPI.transfer(ytopleft);
    delayMicroseconds(200);
  }


  SPI.transfer(xbottomright);
  delayMicroseconds(200);
  if (xbottomright == 96) {
    SPI.transfer(xbottomright);
    delayMicroseconds(200);
  }

  SPI.transfer(ybottomright);
  delayMicroseconds(200);
  if (ybottomright == 96) {
    SPI.transfer(ybottomright);
    delayMicroseconds(200);
  }

  //2ms Delay needed after command excecution
  delay(2);

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  digitalWrite(DISP_CS, HIGH);
  SPI.end();
}


void setup()
{
  // Pin Settings
  pinMode(DISP_CS, OUTPUT);
  pinMode(DISP_RES, OUTPUT);
  digitalWrite(DISP_CS, HIGH);
  digitalWrite(DISP_RES, HIGH);

  // SPI Settings
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(0);                      // CPOL = 0, CPHA = 0.
  SPI.setClockDivider(SPI_CLOCK_DIV8);     // 2MHz Clock

  dispReset();

  Serial.begin(115200);
  while (!Serial)                          // Wait for Serial to open.
  {
    ;
  }


  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting ...");
    delay(500);
  }
  Serial.println("Connected");
  ntp.ruleDST("CEST", Last, Sun, Mar, 2, 120); // last sunday in march 2:00, timetone +120min (+1 GMT + 1h summertime offset)
  ntp.ruleSTD("CET", Last, Sun, Oct, 3, 60); // last sunday in october 3:00, timezone +60min (+1 GMT)
  ntp.begin();
  Serial.println("start NTP");

  Serial << "System Init." << endl;
}

void loop()
{
  if (Serial.available() > 0)
  {
    char incoming = Serial.read();
    Serial << "Key Pressed: " << incoming << endl;

    switch (incoming)
    {
      case '1':
        dispReset();
        break;

      case '2':
        //26 milliseconds
        temp1 = micros();
        drawBox(0, 0, X_MAX, Y_MAX, 2);
        drawBox(5, 5, 120, 58, 2);
        drawBox(10, 10, 115, 53, 2);
        if (DEBUG_2) {
          Serial << "Time Taken: " << micros() - temp1 << " microseconds." << endl;
        }
        break;

      case '3':
        //BarGraph Speed Test - 1.078 Seconds
        temp1 = micros();
        drawBox(0, 0, X_MAX, 31, 2);
        drawBox(0, 32, X_MAX, Y_MAX, 2);

        for (int j = 4, k = 121; j <= 121 && k >= 4; j += 1, k -= 1)
        {
          drawArea(4, 4, j, 27);
        }

        for (int j = 121, k = 4; j >= 4 && k <= 121; j -= 1, k += 1)
        {
          clearArea(121, 4, j, 27);
        }

        if (DEBUG_2) {
          Serial << "Time Taken: " << micros() - temp1 << " microseconds." << endl;
        }
        break;



      case '4':
        // If this gets run flat-out, it generates a pretty cool animation.
        temp1 = micros();
        drawBox(0, 0, X_MAX, 31, 2);
        drawBox(0, 32, X_MAX, Y_MAX, 2);
        if (DEBUG_2) {
          Serial << "Time Taken: " << micros() - temp1 << " microseconds." << endl;
        }
        break;

    }
  }
        temp1 = micros();
        drawBox(0, 0, X_MAX, 31, 2);
        drawBox(0, 32, X_MAX, Y_MAX, 2);

        for (int j = 4, k = 121; j <= 121 && k >= 4; j += 1, k -= 1)
        {
          drawArea(4, 4, j, 27);
        }

        for (int j = 121, k = 4; j >= 4 && k <= 121; j -= 1, k += 1)
        {
          clearArea(121, 4, j, 27);
        }

        if (DEBUG_2) {
          Serial << "Time Taken: " << micros() - temp1 << " microseconds." << endl;
        }
}
