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
        b.Label("root");

        b.Page(GM_NEXT, "Submenu", [](gm::Builder& b) {
            b.Label("in sub 1");

            b.Page(GM_NEXT, "sub 2", [](gm::Builder& b) {
                b.Label("in sub 2");
            });
        });

        // или так
        // if (b.PageBegin(GM_NEXT, "Submenu")) {
        //     b.Label("in sub 1");

        //     if (b.PageBegin(GM_NEXT, "sub 2")) {
        //         b.Label("in sub 2");

        //         b.PageEnd();
        //     }

        //     b.PageEnd();
        // }
    });

    menu.refresh();
}

void loop() {
    if (center) menu.set();
    if (up) menu.up();
    if (down) menu.down();
    if (left) menu.left();
    if (right) menu.right();
}