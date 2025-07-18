#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

class uButton {
   public:
    uButton(uint8_t pin) : pin(pin) {
        pinMode(pin, INPUT_PULLUP);
    }

    operator bool() {
        if (f != !digitalRead(pin)) {
            f ^= 1;
            delay(10);
            if (f) return true;
        }
        return false;
    }

   private:
    uint8_t pin;
    bool f = 0;
};

uButton center(6);
uButton left(5);
uButton right(4);
uButton up(3);
uButton down(2);

// #define GM_NO_PAGES
#include <GyverMenu.h>
GyverMenu menu(20, 4);

uint8_t sel;
int vali;

void setup() {
    lcd.init();
    lcd.backlight();

    menu.onPrint([](const char* str, size_t len) {
        if (str) lcd.Print::write(str, len);
    });
    menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
        lcd.setCursor(0, row);
        lcd.print(chosen && !active ? '>' : ' ');
        return 1;
    });

    menu.onBuild([](gm::Builder& b) {
        b.ValueInt<int>("ValueInt", &vali, -10, 10, 2, DEC, "%");
        b.Select("Select", &sel, "abc;123;test", [](uint8_t n, const char* str, uint8_t len) { Serial.write(str, len); });
    });

    menu.refresh();
}

void loop() {
    if (center) menu.set();
    if (up) menu.up();
    if (down) menu.down();
    if (left) menu.left();
    if (right) menu.right();

    static uint32_t tmr;
    if (millis() - tmr >= 2000) {
        tmr = millis();
        vali = random(10);
        menu.update(&vali);

        sel = random(3);
        menu.update(&sel);
    }
}