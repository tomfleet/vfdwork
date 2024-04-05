#include <SPI.h>
//#include <Serial.h>
#include <Streaming.h>

#include <OneBitDisplay.h>
static uint8_t ucBackBuffer[1024];
static uint8_t ucBackBuffer2[1024];
uint8_t *pOut;
OBDISP obVirt;

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


#include "dispfunc.h"

int rc;

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



  obdCreateVirtualDisplay(&obVirt, 128, 64, ucBackBuffer);
  obdFill(&obVirt, OBD_WHITE, 1);
  obdWriteString(&obVirt, 0, 0, 0, "Hello World! 123ABC", FONT_16x32, OBD_BLACK, 1);
  int a = obdCopy(&obVirt, /*OBD_ROTATE_90 |*/ OBD_MSB_FIRST | OBD_HORZ_BYTES /* | OBD_FLIP_VERT | OBD_FLIP_HORZ*/, ucBackBuffer2);
  if (a != 0)
    Serial << "obCopyError" << endl;
  printScreen();
  while (1)
  {
    ;;
  }
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
