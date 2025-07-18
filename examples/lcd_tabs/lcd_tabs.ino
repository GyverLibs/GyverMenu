#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <StringN.h>

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

uint8_t tab;
int val;
bool sw;

void setup() {
    Serial.begin(115200);

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
        if (b.Tabs(&tab, "btns;labels;ints;switch")) b.refresh();

        switch (tab) {
            case 0:
                for (int i = 0; i < 5; i++) {
                    if (b.Button(String16("Button ") + i)) Serial.println(String16("Button") + i);
                }
                break;

            case 1:
                b.ValueStr("label", "value 1");
                b.ValueStr("label", "value 2");
                b.ValueStr("label", "value 3");
                b.ValueStr("label", "value 4");
                break;

            case 2:
                b.ValueInt("int 1", &val, -10, 10, 2);
                b.ValueInt("int 2", &val, -10, 10, 2);
                break;

            case 3:
                b.Switch("Switch 1", &sw);
                b.Switch("Switch 2", &sw);
                b.Switch("Switch 3", &sw);
                b.Switch("Switch 4", &sw);
                break;
        }
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