#include <Arduino.h>
#include <GyverOLED.h>
GyverOLED<SSD1306_128x64, OLED_NO_BUFFER> oled;

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

bool sw;
uint8_t sel;
int vali;
float valf;

void setup() {
    oled.init();

    menu.onPrint([](const char* str, size_t len) {
        if (str) oled.Print::write(str, len);
        else oled.update();
    });
    menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
        oled.setCursor(0, row);
        oled.invertText(chosen);
        return 0;
    });

    menu.onBuild([](gm::Builder& b) {
        b.Button("Button", []() { Serial.println("click!"); });
        b.Switch("Switch", &sw, [](bool v) { Serial.println(v); });
        b.ValueStr("ValueStr", "foo");
        b.Label("Some line");
        b.Select("Select", &sel, "abc;123;test", [](uint8_t n, const char* str, uint8_t len) { Serial.write(str, len); });
        b.ValueInt<int>("ValueInt", &vali, -10, 10, 2, DEC, "%", [](int v) { Serial.println(v); });
        b.ValueFloat("ValueFloat", &valf, -5, 5, 0.25, 3, "mm", [](float v) { Serial.println(v); });
    });

    menu.setFastCursor(false);
    menu.refresh();
}

void loop() {
    if (center) menu.set();
    if (up) menu.up();
    if (down) menu.down();
    if (left) menu.left();
    if (right) menu.right();
}