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

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_TCS, TFT_DC, TFT_RST);
Adafruit_seesaw soil;

volatile moist_t state;
volatile lite_t brite = HI;

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
    tft.setTextSize(2);

    tft.setCursor(0, 0);
    tft.print("Welcome to taproot!");

    delay(500);

    tft.fillScreen(ST77XX_BLACK);
}

void loop() {
    if (state == ERR) {
        if (!soil.begin(SOIL_ADDR)) {
            Serial.println("Error: Soil sensor failed to reinitialize.");
        } else {
            Serial.println("Success: Soil sensor reinitialized successfully.");
            tft.fillScreen(ST77XX_BLACK);
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
    tft.setCursor(0, 0);

    switch (state) {
        case DRY:
            tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
            tft.print("Soil is Dry ");
            break;
        case MED:
            tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
            tft.print("Soil is Fine");
            break;
        case WET:
            tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
            tft.print("Soil is Wet ");
            break;
        case ERR:
            tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
            tft.print("Sensor Error");
            break;
    }

    tft.setCursor(0, 20);

    if (state != ERR) {
        tft.print(temp);
    } else {
        tft.print("Reinitializing...");
    }
}
