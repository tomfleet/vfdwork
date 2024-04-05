
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
const byte GRAPHIC_WRITE_AREA = 0x1F;
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
void printScreen()
{
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000,MSBFIRST,SPI_MODE0));
  digitalWrite(DISP_CS, LOW);
  delay(1);

  SPI.transfer(CURSOR_HOME);
  delayMicroseconds(200);

  SPI.transfer(WRITE_MODE);
  SPI.transfer(0x00);
  delayMicroseconds(100);

  if (1)
  {
    for (int line = 0; line < 64; line += 1)
    {

      //1) set line position
      SPI.transfer(0x10);
      SPI.transfer(0x00);
      SPI.transfer(line);
      delayMicroseconds(500);
      SPI.transfer(GRAPHIC_WRITE);
      SPI.transfer(16);
      delayMicroseconds(500);
      
      //now we want to send 16 bytes out of ucBackBuffer2, increment line, send again...
      for (int col = 0; col < 16; col += 1)
      {
        uint16_t byteNum = (line * 16) + col;
        uint8_t data = ucBackBuffer2[byteNum];
        if (data != 0)
        {
          Serial << "line = " << line << ", col = " << col << ", byteNum = " << byteNum << ", data = " << ucBackBuffer2[byteNum] << "." << endl;

        }
        SPI.transfer(ucBackBuffer2[byteNum]);
        delayMicroseconds(500);
      }
    }
  }

//  if (0)
//  {
//    SPI.transfer(GRAPHIC_WRITE_AREA);
//    SPI.transfer(0x00);
//    SPI.transfer(0x00);
//    SPI.transfer(0x80);
//    SPI.transfer(0x40);
//    delayMicroseconds(1000);
//    for (int i = 0; i < sizeof(ucBackBuffer2) / sizeof(ucBackBuffer2[0]); i += 1)
//    {
//      SPI.transfer(ucBackBuffer2[i]);
//      delayMicroseconds(350);
//
//    }
//  }
  delay(1);
  digitalWrite(DISP_CS, HIGH);
  delay(1);
  SPI.endTransaction();
  SPI.end();
}
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
