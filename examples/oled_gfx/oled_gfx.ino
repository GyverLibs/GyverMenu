#include <Arduino.h>
#include <GyverOLED.h>

GyverOLED<SSD1306_128x64, OLED_BUFFER> oled;

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
GyverMenu menu(20, 3);

bool sw1, sw2;
uint8_t sldr1, sldr2;

bool MySlider(gm::Builder& b, const char* label, uint8_t maxv, uint8_t* var) {
    if (!b.beginWidget()) return false;

    bool changed = false;
    bool render = false;

    switch (b.getAction()) {
        case gm::Builder::Action::Refresh:
            render = true;
            break;

        case gm::Builder::Action::Set:
            b.menu.toggle();
            render = changed = true;
            break;

        case gm::Builder::Action::SetUp:
        case gm::Builder::Action::Right:
            if (*var < maxv) {
                ++*var;
                b.change();
                render = changed = true;
            }
            break;

        case gm::Builder::Action::SetDown:
        case gm::Builder::Action::Left:
            if (*var) {
                --*var;
                b.change();
                render = changed = true;
            }
            break;

        default: break;
    }

    if (render && b.beginRender()) {
        oled.rect(8, b.menu.currentRow() * 16, 127, (b.menu.currentRow() + 1) * 16 - 1, OLED_CLEAR);
        oled.setCursor(12, b.menu.currentRow() * 2);
        oled.print(label);
        if (b.menu.isActive()) oled.print(':');
        oled.roundRect(127 - 50, b.menu.currentRow() * 16, 127, b.menu.currentRow() * 16 + 15, OLED_STROKE);
        oled.roundRect(127 - 50, b.menu.currentRow() * 16, 127 - 50 + map(*var, 0, maxv, 5, 50), b.menu.currentRow() * 16 + 15, OLED_FILL);
    }
    return changed;
}

bool MySwitch(gm::Builder& b, const char* label, bool* var) {
    if (!b.beginWidget()) return false;

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

    if (render && b.beginRender()) {
        oled.rect(8, b.menu.currentRow() * 16, 127, (b.menu.currentRow() + 1) * 16 - 1, OLED_CLEAR);
        oled.setCursor(12, b.menu.currentRow() * 2);
        oled.print(label);
        oled.roundRect(127 - 10, b.menu.currentRow() * 16 + 2, 127, b.menu.currentRow() * 16 + 2 + 10, OLED_CLEAR);
        oled.roundRect(127 - 10, b.menu.currentRow() * 16 + 2, 127, b.menu.currentRow() * 16 + 2 + 10, *var ? OLED_FILL : OLED_STROKE);
    }
    return changed;
}

void setup() {
    oled.init();
    oled.setScale(2);

    menu.onBuild([](gm::Builder& b) {
        MySwitch(b, "SW1", &sw1);
        MySlider(b, "SLD1", 16, &sldr1);
        MySwitch(b, "SW2", &sw2);
        MySlider(b, "SLD2", 5, &sldr2);
        MySwitch(b, "SW3", &sw2);
    });

    menu.onPrint([](const char* str, size_t len) {
        if (!str) oled.update();
    });
    menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
        oled.setCursor(0, row * 2);

        if (chosen && !active) {
            oled.rect(0, row * 16 + 5, 8, row * 16 + 10, OLED_FILL);
        } else {
            oled.rect(0, row * 16, 8, row * 16 + 16, OLED_CLEAR);
        }
        return 1;
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