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

void MySlider(gm::Builder& b, const char* label, uint8_t* var, void (*cb)(uint8_t) = nullptr) {
    if (!b.menu.beginWidget()) return;

    bool render = false;

    switch (b.getAction()) {
        case gm::Builder::Action::Refresh:
            render = true;
            break;

        case gm::Builder::Action::Set:
            b.menu.toggle();
            render = true;
            break;

        case gm::Builder::Action::SetUp:
        case gm::Builder::Action::Right:
            if (*var < 5) {
                ++*var;
                if (cb) cb(*var);
                b.change();
                render = true;
            }
            break;

        case gm::Builder::Action::SetDown:
        case gm::Builder::Action::Left:
            if (*var) {
                --*var;
                if (cb) cb(*var);
                b.change();
                render = true;
            }
            break;

        default: break;
    }

    if (render && b.beginRender()) {
        b.menu.print(label);
        if (b.menu.isActive()) b.menu.print(':');
        b.menu.pad(b.menu.left - 7);

        b.menu.print('[');
        for (uint8_t i = 0; i < 5; i++) {
            b.menu.print(i < *var ? '=' : ' ');
        }
        b.menu.print(']');
    }
}

bool MySwitch(gm::Builder& b, const char* label, bool* var, void (*cb)(uint8_t) = nullptr) {
    if (!b.menu.beginWidget()) return false;

    bool changed = false;
    bool render = false;

    switch (b.getAction()) {
        case gm::Builder::Action::Refresh:
            render = true;
            break;

        case gm::Builder::Action::Set:
            *var = !*var;
            render = changed = true;
            b.change();
            break;

        case gm::Builder::Action::SetUp:
        case gm::Builder::Action::Right:
            if (!*var) {
                *var = true;
                render = changed = true;
                b.change();
            }
            break;

        case gm::Builder::Action::SetDown:
        case gm::Builder::Action::Left:
            if (*var) {
                *var = false;
                render = changed = true;
                b.change();
            }
            break;

        default: break;
    }

    if (changed && cb) cb(*var);

    if (render && b.beginRender()) {
        b.menu.print(label);
        b.menu.pad(b.menu.left - 2);
        if (*var) b.menu.print(1), b.menu.print(2);
        else b.menu.print(2), b.menu.print(3);
    }

    return changed;
}

bool sw1, sw2;
uint8_t sldr;

void setup() {
    Serial.begin(115200);
    lcd.init();
    lcd.backlight();

    uint8_t sw_left[] = {0x00, 0x07, 0x08, 0x10, 0x10, 0x08, 0x07, 0x00};
    uint8_t sw_handle[] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E};
    uint8_t sw_right[] = {0x00, 0x1C, 0x02, 0x01, 0x01, 0x02, 0x1C, 0x00};

    lcd.createChar(1, sw_left);
    lcd.createChar(2, sw_handle);
    lcd.createChar(3, sw_right);

    menu.onPrint([](const char* str, size_t len) {
        if (str) lcd.Print::write(str, len);
    });
    menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
        lcd.setCursor(0, row);
        lcd.print(chosen && !active ? '>' : ' ');
        return 1;
    });

    menu.onBuild([](gm::Builder& b) {
        MySwitch(b, "Switch 1", &sw1);
        MySwitch(b, "Switch 2", &sw2);
        MySlider(b, "Slider", &sldr, [](uint8_t v) { Serial.println(v); });
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