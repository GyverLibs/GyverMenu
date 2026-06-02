This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverMenu.svg?color=brightgreen)](https://github.com/GyverLibs/GyverMenu/releases/latest/download/GyverMenu.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverMenu.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverMenu)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverMenu?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverMenu
Dynamic menu system for Arduino
- Assembling the menu in the "builder" with the ability to output widgets under the condition or in the cycle
- The mechanism of nested pages with navigation memory
- Easy API for navigating any number of buttons/joystick/encoder
- Output to any display or port monitor
- Alignment of values on the right edge with support for Cyrillic
- 9 embedded widgets
- Creating your own widgets
- Mechanism for updating widgets
- Optimizing the number of screen redraws (configured)
- Out of the box is a text menu, but you can make your own widgets for a graphical display.

https://github.com/user-attachments/assets/41910bcf-d537-4f18-a999-8ff6d2c00438

> [Demo project](https://wokwi.com/projects/436903131317665793)simulation

### Compatibility
Compatible with all platforms

### Dependencies
- [StringN](https://github.com/GyverLibs/StringN)

## Contents
- [Use of use](#usage)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="usage"></a>

## Description of classes
### GyverMenu
```cpp
// columns (line length), rows
GyverMenu(uint8_t cols, uint8_t rows);

// connect the render type void (const char* str, size t len). The nullptr will arrive after the output is complete.
void onPrint(gm::Menu::PrintCb cb);

// uint8 t (uint8 t row, bool state)
void onCursor(gm::Menu::CursorCb cb);

// connect a builder type void(gm::Builder&b)
void onBuild(gm::Builder::BuildCb cb);

// set the back button
void setBackSign(const char* sign);

// variable
void update(void* var);

// screen
void refresh();

// previous menu
void back();

// main menu
void home();

// pick-button
void set();

// push-button
void up();

// down-button
void down();

// direct
void left();

// direct
void right();

// Update the screen completely, for example, to output to the console (silent. false)
void setFullRefresh(bool full);

// turn on the fast cursor - render only the cursor when changing the line
void setFastCursor(bool fast);
```

### gm::Builder
#### Page
```cpp
// start the page (submenu)
bool PageBegin(uint8_t id, const __FlashStringHelper* label);
bool PageBegin(uint8_t id, const char* label);

// end the page (call inside the terms by PageBegin). back - output the "back" button
void PageEnd(bool back = true);

// Callback page (instead of PageBegin-PageEnd) back - output the "back" button
void Page(uint8_t id, const __FlashStringHelper* label, void (*page)(Builder& b), bool back = true);
void Page(uint8_t id, const char* label, void (*page)(Builder& b), bool back = true);
```

#### Widgets
```cpp
// button
bool Button(const __FlashStringHelper* label, void (*cb)() = nullptr);
bool Button(const char* label, void (*cb)() = nullptr);

// text
void Label(const __FlashStringHelper* line);
void Label(const char* line);

// switcher
bool Switch(const __FlashStringHelper* label, bool* var, void (*cb)(bool v) = nullptr);
bool Switch(const char* label, bool* var, void (*cb)(bool v) = nullptr);

// Item selection. opts is a line with a separator ';'
bool Select(const __FlashStringHelper* label, uint8_t* var, const __FlashStringHelper* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);
bool Select(const char* label, uint8_t* var, const char* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// Selection of the item in the style of "tabs". tabs - a line with a separator ';'
bool Tabs(uint8_t* var, const __FlashStringHelper* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);
bool Tabs(uint8_t* var, const char* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// textuality
void ValueStr(const __FlashStringHelper* label, const char* value);
void ValueStr(const char* label, const char* var);

// value
bool ValueInt(const __FlashStringHelper* label, T* var, T minv, T maxv, T step, uint8_t base, const __FlashStringHelper* unit, void (*cb)(T v) = nullptr);
bool ValueInt(const char* label, T* var, T minv, T maxv, T step, uint8_t base = 10, const char* unit = "", void (*cb)(T v) = nullptr);

// float
bool ValueFloat(const __FlashStringHelper* label, float* var, float minv, float maxv, float step, uint8_t dec, const __FlashStringHelper* unit, void (*cb)(float v) = nullptr);
bool ValueFloat(const char* label, float* var, float minv, float maxv, float step, uint8_t dec = 2, const char* unit = "", void (*cb)(float v) = nullptr);
```

#### Systemic
```cpp
// Update the screen after the builder
void refresh();

// There was an action with one of the widgets above.
bool wasSet();

// Reset the reading flag wasSet
void clearSet();
```

#### API
```cpp
// start the widget. true if permitted
bool beginWidget();

// start removing the widget. true if permitted
// targetVar - pointer to the widget variable
// wCursor - Whether to draw a cursor
bool beginRender(void* targetVar = nullptr, bool wCursor = true);

// widget
Action getAction();

// Raise the change flag (affects wasSet())
void change();

// print
void menu.print(char c);
void menu.print(const char* str);
void menu.print(const char* str, uint8_t len, uint8_t letters = 0);

// fill in
void menu.pad(int8_t n);

// fill in
void menu.pad();

// switch the active state (isActive)
void menu.toggle();

// current-line
uint8_t menu.currentRow();

// The widget is active (set button)
bool menu.isActive();

// widget selected by cursor
bool menu.isChosen();

// widget
bool menu.isVisible();
```

### Define settings
Announced before connecting the library

```cpp
#define GM_MAX_DEPTH 5  // max. the nesting of the menu (no. 5)
#define GM_NO_PAGES     // Disable the attached menu (facilitates the library)
```

## Use of use
> [!NOTE]
> Documentation in development!

### How it works.
The library navigates the virtual menu, which is set in the builder. As entry points, the library has 5 virtual button functions that you need to call from the main program to press physical buttons or other events. As exit points, there are three processing functions: builder, cursor installation and text printing. Thus, the library is not tied to specific displays and input methods and even to the Arduino framework - it can be run theoretically on any platform, the menu itself, for example, output text to the console, and the buttons "up" and "down" make voice input.

### Buttons
The library provides **virtual** 5 control buttons + individual commands "return to the previous menu" and "home screen", but in practice it can be any other number of buttons - at least one, at least an encoder. The logic of the work is:

- `menu.set()`- central selection button. She presses the widget.`Button`on the screen, switches the widget state`Switch`, handles pressing the return button to the previous menu, and for other widgets - transfers them to change mode
- `menu.up()`Move the cursor to the previous point. If the change mode is active, an increase in value will be caused instead.
- `menu.down()`- move the cursor to the next point. If the change mode is active, a decrease in value will be caused instead.
- `menu.left()`Directly reduces the value of the current widget regardless of the mode of change. Also presses the back button to return to the previous menu.
- `menu.right()`- directly increases the value of the current widget regardless of the mode of change. Also presses the submenu and widget entry button.`Button`
- `menu.back()`- returns to the previous menu
- `menu.home()`- returns to the main menu

Thus, navigation scenarios can be:

- 5 buttons or joystick with a central button
- 4 buttons (all but center button) or joystick without center button
- 3 buttons:`верх` + `центр` + `низ`
- 2 buttons:`верх` + `низ` +
  - simultaneous pressing causes`set`
  - Retention`вниз`trigger`set`
  - Retention`вверх`trigger`back`
- Encoder:`up`and`down`spinning,`set`click
- Encoder:`up`and`down`spinning,`set`by click, clamped turn as`left`and`right`
  - Holding the button -`back`. A very long hold--`home`

There are a lot of options that you can use.[EncButton](https://github.com/GyverLibs/EncButton)For processing buttons and encoders and be inspired by Chinese digital soldering irons.

### Handlers
To use the menu, you need to connect 3 handlers. Only the builder is mandatory, the rest may not be used when writing your widgets for your display:

#### builder
```cpp
void builder(gm::Builder& b) {
    // b - object to call widgets
}
menu.onBuild(builder);
```

#### Seal.
```cpp
void printer(const char* str, size_t len) {
    // str - line for display
    // if str = = nullptr, the menu is finished (for cases when you need to update the screen)
    // len - the number of symbols in a line
}
menu.onPrint(printer);
```

#### Courier
```cpp
uint8_t cursor(uint8_t row, bool chosen, bool active) {
    // row - current line of display
    // selected - whether the current widget is selected by the cursor
    // Active – is the current widget in change mode?
    // return the number of symbols (columns) occupied by the cursor (if occupied)
}
menu.onCursor(cursor);
```

Examples of handlers for LCD display with standard library, cursor type`>`lack of cursor - gap` `:

```cpp
menu.onPrint([](const char* str, size_t len) {
    if (str) lcd.Print::write(str, len);
});

menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
    lcd.setCursor(0, row);
    lcd.print(chosen && !active ? '>' : ' ');
    return 1;
});
```

The cursor handler is called not only before the line is drawn, but also when the line is changed. It is for this purpose that it is made separate - when changing the cursor, only the cursor is redrawn, and not the entire menu, which increases the speed and responsiveness of the system. Disable this behavior and always redraw the line completely with the help of`menu.setFastCursor(true)`.

It seems too clever, but flexible. For example, for an OLED display, you can not draw a cursor, but invert the output of text for the current line:

```cpp
menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
    oled.setCursor(0, row);
    oled.invertText(chosen);
    return 0;
});
menu.setFastCursor(true);
```

### Buildinger and widgets
A builder is a processor that is called by the library to:

- Pictures of the current selected widget
- Full screen renderings
- Search and render the widget to update its value from the update

That is, the builder can be called often and for different purposes, so you can not write in it any slow and heavy constructions.

#### Conclusion
To output widgets, you just need to call their functions in the order in which they should be located on the display, for example:

```cpp
menu.onBuild([](gm::Builder& b) {
    b.Button("Button 1");
    b.Button("Button 2");
});
```

Widgets can be created dynamically, such as 5 buttons:

```cpp
menu.onBuild([](gm::Builder& b) {
    for (int i = 0; i < 5; i++) {
        b.Button(String16("Button ") + i);
    }
});
```

Widgets can be displayed under the condition:

```cpp
menu.onBuild([](gm::Builder& b) {
    if (foo) {
        b.Button("Button 1");
        b.Button("Button 2");
    }
});
```

#### Action
Widgets built into the library have three interaction mechanisms:

- The active widget function returns`true`when changing the value (and clicking for the button widget)
- To active widgets, you can connect a processing function that will be called when the value changes (also a new value will be sent to it).
- A variable is connected to widgets with a value, which will be automatically changed by the library.

```cpp
void clickHandler() {
    Serial.println("Btn 2 click");
}

bool sw;

menu.onBuild([](gm::Builder& b) {
    // condition
    if (b.Button("Button 1")) {
        Serial.println("Btn 1 click");
    }

    // handler
    b.Button("Button 2", clickHandler);

    // plug-in
    b.Switch("Switch 1", &sw);
    
    // variable connection + processor
    b.Switch("Switch 2", &sw, [](bool v) { Serial.println(v); });
});
```

By action from the widget, you can update the menu, for example, the switch is responsible for the output of several buttons. Buttons are active, click processing works even when output through the loop:

```cpp
bool btns;

menu.onBuild([](gm::Builder& b) {
    if (b.Switch("Buttons", &btns)) b.refresh();    // click up

    if (btns) {
        for (int i = 0; i < 5; i++) {
            if (b.Button(String16("Button ") + i)) {
                Serial.println(String16("Button") + i);
            }
        }
    }
});
```

#### Tricks.
In widgets with step adjustment, the step can be set dynamically, for example, from the speed of rotation of the encoder. With the EncButton library, it might look like this:

```cpp
int vali;

menu.onBuild([](gm::Builder& b) {
    b.ValueInt<int>("ValueInt", &vali, -100, 100, encb.fast() ? 10 : 1);
});
```

That is, with a fast rotation of the encoder, the step will be 10 (rough), and with a slow one - 1 (exact). The builder is called at each action of changing the value, so the step will be selected for each click.

### Examples
```cpp
```

<a id="versions"></a>

## Versions
- v1.0

<a id="install"></a>
## Installation
- The library can be found under the name **GyverMenu** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/GyverMenu/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
