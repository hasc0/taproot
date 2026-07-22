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

typedef enum {
    FULL_DRY,
    PART_DRY,
    NEUTRAL,
    PART_WET,
    FULL_WET
} moist_t;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_TCS, TFT_DC, TFT_RST);
Adafruit_seesaw soil;

void setup() {
    Serial.begin(115200);
    tft.init(240, 240);
    soil.begin(SOIL_ADDR);

    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);

    tft.print("Welcome to taproot!");
    delay(500);
}

void loop() {
    float temp = soil.getTemp();
    uint16_t cap = soil.touchRead(0);

    tft.fillRect(0, 20, 240, 40, ST77XX_BLACK);

    tft.setCursor(0, 20);
    tft.print(temp);

    tft.setCursor(0, 40);
    tft.print(cap);

    delay(100);
}
