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

bool btns, lbls;

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
        b.Label("   ---Settings---");
        
        // Важно: `GM_NEXT` разворачивается во время компиляции и в теле `for` даёт один и тот же ID для каждой итерации.
        // Для динамического создания страниц используйте `Page(label, cb)` / `PageBegin(label)` с автоматическим ID
        // или назначайте вручную с помощью `nextId()`.

        b.Page("Subs", [](gm::Builder& b) {
            for (uint8_t i = 0; i < 3; i++)
            {
                String fieldLabel = "Sub " + String(i + 1);
                b.Page(fieldLabel.c_str(), [&i](gm::Builder& b) { // Захват внешних переменных для лямбда-функции элемента Page
                    for (uint8_t j = 0; j < 5; j++)
                    {
                        String btnLabel = "Subsub " + String(char('A' + j));
                        b.Page(btnLabel.c_str(), [&i, &j](gm::Builder& b) { // Захват внешних переменных для лямбда-функции элемента Page
                             b.Label("i = " + String(i));
                             b.Label("j = " + String(j));
                        });
                    }
                });
            }
        });
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