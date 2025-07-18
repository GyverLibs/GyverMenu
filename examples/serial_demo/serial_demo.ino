#include <Arduino.h>

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
    Serial.begin(115200);

    menu.onPrint([](const char* str, size_t len) {
        if (str) Serial.write(str, len);
        else Serial.println();
    });
    menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
        Serial.println();
        Serial.print(chosen && !active ? '>' : ' ');
        return 1;
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

    menu.refresh();
    menu.setFullRefresh(true);
}

void loop() {
    if (center) menu.set();
    if (up) menu.up();
    if (down) menu.down();
    if (left) menu.left();
    if (right) menu.right();
}