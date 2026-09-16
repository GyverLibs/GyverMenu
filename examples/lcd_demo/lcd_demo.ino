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

bool sw;
uint8_t sel;
int vali;
float valf;
char editstr[9] = "Gyver";
char editascii[9] = "Menu";
gm::Time time;
gm::Date date;
uint8_t flags = 0b00111;

void setup() {
    lcd.init();
    lcd.backlight();

    menu.onPrint([](const char* str, size_t len) {
        if (str) lcd.Print::write(str, len);
    });
    menu.onCursor([](uint8_t col, uint8_t row) {
        lcd.setCursor(col, row);
    });
    menu.onState([](uint8_t row, bool chosen, bool active) {
        lcd.print(chosen && !active ? menu.getMarker() : ' ');
    });

    menu.onBuild([](gm::Builder& b) {
        b.Button("Button", []() { Serial.println("click!"); });
        b.Switch("Switch", &sw, [](bool v) { Serial.println(v); });
        b.ValueStr("ValueStr", "foo");
        b.ValueInt("ValueInt", &vali);
        b.ValueFloat("ValueFloat", &valf, 2, "V");
        b.Label("Some line");
        b.Select("Select", &sel, "abc;123;test", [](uint8_t n, const char* str, uint8_t len) { Serial.write(str, len); });
        b.EditInt<int>("EditInt", &vali, -10, 10, 2, "%", [](int v) { Serial.println(v); });
        b.EditFloat("EditFloat", &valf, -5, 5, 0.25, 3, "mm", [](float v) { Serial.println(v); });
        b.EditStr("EditStr", editstr, 8);
        b.EditASCII("EditASCII", editascii, 8);
        b.Time("Time", &time);
        b.Date("Date", &date);
        b.Bitmask("Flags", &flags, 5);
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