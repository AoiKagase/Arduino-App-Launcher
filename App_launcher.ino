/* an alternative approach.   swap the #if 1 / 0 values to try it

*/
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define YP A2
#define XM A3
#define YM 8
#define XP 9

#define TS_MINX 130
#define TS_MAXX 905

#define TS_MINY 75
#define TS_MAXY 930

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MAX_BUTTONS 6
#define MAX_BTN_CHR 10

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button buttons[MAX_BUTTONS];
char btnCharacter[MAX_BTN_CHR] = {'\0'};

int pixel_x;
int pixel_y;

int TFT_MAX_W = 0;
int TFT_MAX_H = 0;

bool initialized = false;

int adjust_x(int x)
{
    return constrain(map(x, TS_MAXX, TS_MINX, 0, TFT_MAX_W), 0, TFT_MAX_W);
}

int adjust_y(int y)
{
    return constrain(map(y, TS_MAXY, TS_MINY, 0, TFT_MAX_H), 0, TFT_MAX_H);
}

bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();

    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);
    digitalWrite(XM, HIGH);

    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);

    if (pressed)
    {
        pixel_x = adjust_x(p.x);
        pixel_y = adjust_y(p.y);
    }

    return pressed;
}

void setup(void)
{
    Serial.begin(9600);

    uint16_t ID = tft.readID();
  
    // Serial.println("TFT ID = 0x" + String(ID, HEX));
    // Serial.println("Calibrate for your Touch Panel");

    // write-only shield
    if (ID == 0xD3D3)
        ID =  0x9486;
    
    tft.begin(ID);
    tft.setRotation(3);            //LANDSCAPE
    tft.fillScreen(BLACK);

    TFT_MAX_W = tft.width()  - 1;
    TFT_MAX_H = tft.height() - 1;
}

void loop(void)
{
    if (!initialized)
    {
        if(Serial.available())
        {
            int i = 0;
            String ret = Serial.readStringUntil(',');

            while(ret.length() > 0)
            {
                Serial.println("0000");

                memset(btnCharacter, '\0', MAX_BTN_CHR);
                ret.toCharArray(btnCharacter, ret.length()+1);
                buttons[i].initButton(&tft, 130 + (220 * (i / 3)), 60 + (100 * (i % 3)), 200, 80, WHITE, BLACK, CYAN, btnCharacter, 2);
                buttons[i].drawButton(false);
                i++;

                delay(100);
                ret = Serial.readStringUntil(',');
            }
            initialized = true;
        }
    } else
    {
        bool down = Touch_getXY();

        for(int i = 0; i < MAX_BUTTONS; i++)
        {
            buttons[i].press(down && buttons[i].contains(pixel_x, pixel_y));

            if (buttons[i].justReleased())
            {
                buttons[i].drawButton(false);
            }
            if (buttons[i].justPressed())
            {
                buttons[i].drawButton(true);
                Serial.println("0002" + String(i));
            }
        }
    }
    delay(1);
}
