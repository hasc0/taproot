#include <Arduino.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#include <Adafruit_seesaw.h>

#define TFT_SCK D13
#define TFT_MOSI D11
#define TFT_MISO D12
#define TFT_DC D8
#define TFT_RST D7
#define TFT_TCS D6
#define TFT_SDCS D5
#define TFT_LITE D4

#define SOIL_SDA A4
#define SOIL_SCL A5
#define SOIL_ADDR 0x36

#define BTN_LITE D2

#define DEBOUNCE 100

typedef enum {
    DRY,
    MED,
    WET,
    ERR
} moist_t;

typedef enum {
    LO,
    MD,
    HI
} lite_t;

void draw(float temp);

void disp_dry();
void disp_med();
void disp_wet();
void disp_err();

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_TCS, TFT_DC, TFT_RST);
Adafruit_seesaw soil;

volatile moist_t state;
volatile lite_t brite = HI;

volatile moist_t last_state;
volatile bool last_brite = HIGH;

void setup() {
    Serial.begin(115200);

    tft.init(240, 240);
    pinMode(TFT_LITE, OUTPUT);
    analogWrite(TFT_LITE, 192);

    // TODO: Proper Error Handling
    if (!soil.begin(SOIL_ADDR)) {
        Serial.println("Error: Soil sensor failed to initialize.");
    } else {
        Serial.println("Success: Soil sensor initialized successfully.");
    }

    pinMode(BTN_LITE, INPUT_PULLUP);

    tft.fillScreen(ST77XX_BLACK);

    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    tft.setTextSize(5);

    tft.setCursor(0, 100);
    tft.print("taproot!");

    delay(3000);

    tft.fillScreen(ST77XX_BLACK);

    tft.setTextSize(2);
}

void loop() {
    if (state == ERR) {
        if (!soil.begin(SOIL_ADDR)) {
            Serial.println("Error: Soil sensor failed to reinitialize.");
        } else {
            Serial.println("Success: Soil sensor reinitialized successfully.");
        }
    }

    float temp_c = soil.getTemp();
    float temp_f = (temp_c * (9.0 / 5.0)) + 32.0;

    uint16_t cap = soil.touchRead(0);

    uint32_t now = millis();

    bool read_brite = digitalRead(BTN_LITE);

    if (read_brite != last_brite) {
        if (now > DEBOUNCE) {
            if (read_brite == LOW) {
                switch (brite) {
                    case LO:
                        analogWrite(TFT_LITE, 64);
                        brite = MD;
                        break;
                    case MD:
                        analogWrite(TFT_LITE, 128);
                        brite = HI;
                        break;
                    case HI:
                        analogWrite(TFT_LITE, 192);
                        brite = LO;
                        break;
                }
            }
        }
    }

    last_brite = read_brite;

    if (cap <= 600) {
        state = DRY;
    } else if (cap <= 800) {
        state = MED;
    } else if (cap <= 1023) {
        state = WET;
    } else {
        state = ERR;
    }

    draw(temp_f);
}

void draw(float temp) {
    if (state != last_state || !last_state) {
        switch (state) {
            case DRY:
                disp_dry();
                break;
            case MED:
                disp_med();
                break;
            case WET:
                disp_wet();
                break;
            case ERR:
                disp_err();
                break;
        }

        last_state = state;
    }

    tft.setCursor(0, 220);

    if (state != ERR) {
        int round = static_cast<int>(temp);
        String out = " Temperature: ";
        out += round;

        if (round < 10) {
            out += "  *F ";
        } else if (round >= 10 && round < 100) {
            out += " *F ";
        } else {
            out += "*F ";
        }

        tft.print(out);
    } else {
        tft.print("    Sensor Error    ");
    }
}

void disp_dry() {
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);

    tft.setCursor(0, 0);
    tft.print("<------------------>");

    tft.setCursor(0, 20);
    tft.print("|                  |");

    tft.setCursor(0, 40);
    tft.print("|                  |");

    tft.setCursor(0, 60);
    tft.print("|                  |");

    tft.setCursor(0, 80);
    tft.print("|                  |");

    tft.setCursor(0, 100);
    tft.print("|                  |");

    tft.setCursor(0, 120);
    tft.print("|                  |");

    tft.setCursor(0, 140);
    tft.print("|                  |");

    tft.setCursor(0, 160);
    tft.print("|                  |");

    tft.setCursor(0, 180);
    tft.print("|                  |");

    tft.setCursor(0, 200);
    tft.print("<------------------>");
}

void disp_med() {
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);

    tft.setCursor(0, 0);
    tft.print("<------------------>");

    tft.setCursor(0, 20);
    tft.print("|                  |");

    tft.setCursor(0, 40);
    tft.print("|                  |");

    tft.setCursor(0, 60);
    tft.print("|                  |");

    tft.setCursor(0, 80);
    tft.print("|                  |");

    tft.setCursor(0, 100);
    tft.print("|                  |");

    tft.setCursor(0, 120);
    tft.print("|                  |");

    tft.setCursor(0, 140);
    tft.print("|                  |");

    tft.setCursor(0, 160);
    tft.print("|                  |");

    tft.setCursor(0, 180);
    tft.print("|                  |");

    tft.setCursor(0, 200);
    tft.print("<------------------>");
}

void disp_wet() {
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);

    tft.setCursor(0, 0);
    tft.print("<------------------>");

    tft.setCursor(0, 20);
    tft.print("|                  |");

    tft.setCursor(0, 40);
    tft.print("|                  |");

    tft.setCursor(0, 60);
    tft.print("|                  |");

    tft.setCursor(0, 80);
    tft.print("|                  |");

    tft.setCursor(0, 100);
    tft.print("|                  |");

    tft.setCursor(0, 120);
    tft.print("|                  |");

    tft.setCursor(0, 140);
    tft.print("|                  |");

    tft.setCursor(0, 160);
    tft.print("|                  |");

    tft.setCursor(0, 180);
    tft.print("|                  |");

    tft.setCursor(0, 200);
    tft.print("<------------------>");
}

void disp_err() {
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);

    tft.setCursor(0, 0);
    tft.print("<------------------>");

    tft.setCursor(0, 20);
    tft.print("|                  |");

    tft.setCursor(0, 40);
    tft.print("|                  |");

    tft.setCursor(0, 60);
    tft.print("|                  |");

    tft.setCursor(0, 80);
    tft.print("|                  |");

    tft.setCursor(0, 100);
    tft.print("|                  |");

    tft.setCursor(0, 120);
    tft.print("|                  |");

    tft.setCursor(0, 140);
    tft.print("|                  |");

    tft.setCursor(0, 160);
    tft.print("|                  |");

    tft.setCursor(0, 180);
    tft.print("|                  |");

    tft.setCursor(0, 200);
    tft.print("<------------------>");
}
