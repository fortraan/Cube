#include <SeeedTFT.h>
#include <string.h>
#include <TouchScreen.h>

#define YP A2   // y+ | must be an analog pin, use "An" notation!
#define XM A1   // x- | must be an analog pin, use "An" notation!
#define YM 14   // y- | can be a digital pin, this is A0
#define XP 17   // x+ | can be a digital pin, this is A3 

#define TS_MINX 140
#define TS_MAXX 900

#define TS_MINY 120
#define TS_MAXY 940

#define FOREGROUND WHITE
#define BACKGROUND BLACK

typedef enum {
  F,
  R,
  U,
  L,
  B,
  D
} Face;

typedef enum {
  CR,
  CCR,
  DOUBLE
} Modifier;

const char* faceNames = "FRULBD";
const char* modifierNames = " '2";
bool aff[6];

TouchScreen ts(XP, YP, XM, YM, 300);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Tft.init();

  memset(aff, false, 6);

  generateSequence();

  rect(10, 120, 60, 310, FOREGROUND);
  Tft.drawString("NEW", 42, 192, 2, FOREGROUND);
}

void loop() {
  // put your main code here, to run repeatedly:
  TSPoint p = ts.getPoint();

  p.x = map(p.x, TS_MINX, TS_MAXX, 240, 0);
  p.y = map(p.y, TS_MINY, TS_MAXY, 320, 0);

  if (ts.pressure() > 20 && p.x > 10 && p.x < 60 && p.y > 120 && p.y < 310) {
    generateSequence();
  }
}

void generateSequence() {
  randomSeed(analogRead(A5));
  randomSeed(random(1023) * analogRead(A5));
  Tft.fillRectangle(62, 0, 240, 320, BACKGROUND);
  Tft.fillRectangle(0, 0, 62, 118, BACKGROUND);
  
  char str[73];
  char lines[4][19];
  memset(str, ' ', 73);
  str[72] = '\0';
  for (int line = 0; line < 4; line++) {
    memset(lines[line], ' ', 19);
    lines[line][18] = '\0';
  }

  Face lastFace;
  for (uint8_t i = 0; i < 20; i++) {
    Face face;
    Modifier modifier;
    do {
      face = (Face) random(6);
    } while (i > 0 && (!affected(face) || face == lastFace));
    modifier = (Modifier) random(3);
    affect(face);
    lastFace = face;
    
    /*switch (face) {
      case F:
        str[3 * i] = 'F';
        break;
      case R:
        str[3 * i] = 'R';
        break;
      case U:
        str[3 * i] = 'U';
        break;
      case L:
        str[3 * i] = 'L';
        break;
      case B:
        str[3 * i] = 'B';
        break;
      case D:
        str[3 * i] = 'D';
        break;
    }
    switch (modifier) {
      case CR:
        break;
      case CCR:
        str[3 * i + 1] = '\'';
        break;
      case DOUBLE:
        str[3 * i + 1] = '2';
        break;
    }*/
    str[3 * i] = faceNames[(uint8_t) face];
    str[3 * i + 1] = modifierNames[(uint8_t) modifier];
    str[3 * i + 2] = ' ';
  }
  Serial.println(str);

  Tft.setDisplayDirect(UP2DOWN);
  for (int i = 0; i < 4; i++) {
    memcpy(lines[i], str + 18 * i, 18);
    Tft.drawString(lines[i], 200 - 50 * i, 25, 2, FOREGROUND);
  }
}

// if i wanted to get fancy, i could store
// the affect flags in contiguous blocks and
// use pointer arithmetic
bool affected(Face face) {
  return *(aff + (uint8_t) face);
}

void setAffected(Face face) {
  *(aff + (uint8_t) face) = true;
}

void resetAffected(Face face) {
  *(aff + (uint8_t) face) = false;
}

void affect(Face face) {
  switch (face) {
    case F:
    case B:
      setAffected(U);
      setAffected(R);
      setAffected(D);
      setAffected(L);
      break;
    case U:
    case D:
      setAffected(B);
      setAffected(R);
      setAffected(F);
      setAffected(L);
      break;
    case L:
    case R:
      setAffected(U);
      setAffected(F);
      setAffected(D);
      setAffected(B);
      break;
  }
}

void rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint32_t color) {
  Tft.drawLine(x0, y0, x1, y0, color);
  Tft.drawLine(x1, y0, x1, y1, color);
  Tft.drawLine(x1, y1, x0, y1, color);
  Tft.drawLine(x0, y1, x0, y0, color);
}
