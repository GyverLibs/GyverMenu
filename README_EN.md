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
- Set of built-in widgets
- Creating your own widgets
- The mechanism for updating the values of widgets on the screen
- Optimizing the number of screen redraws: full screen / line only / changeable only
- Out of the box, this is a text menu, but you can make your own widgets for a graphical display.

https://github.com/user-attachments/assets/41910bcf-d537-4f18-a999-8ff6d2c00438

> [Demo project](https://wokwi.com/projects/436903131317665793)simulation

### Compatibility
Compatible with all platforms

### Dependencies
- [StringN](https://github.com/GyverLibs/StringN)

## Contents
- [Description of classes](#api)
- [Use of use](#usage)
- [Update modes](#refresh-modes)
- [Their widgets.](#custom-widgets)
- [Limitations and features](#limits)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="api"></a>

## Description of classes
### GyverMenu
```cpp
// columns (line length), rows
GyverMenu(uint8_t cols, uint8_t rows);

// Change the menu size and return to the root
void resize(uint8_t cols, uint8_t rows);

// Enable void (const char* str, size t len)
// After finishing the drawing, nullptr will arrive.
void onPrint(gm::Menu::PrintCb cb);

// Enable void (uint8 t col, uint8 t row)
void onCursor(gm::Menu::CursorCb cb);

// void (uint8 t row, bool selected, bool editing)
void onState(gm::Menu::StateCb cb);

// connect the builder void(gm::Builder&b)
void onBuild(gm::Builder::BuildCb cb);

// The size of the marker shifts the menu to the right (silent). 1)
void setMarkerSize(bool offset);

// internal marker symbol (silent '>')
void setMarker(char symb);
char getMarker();

// set the back button
void setBackSign(const char* sign);

// Update the widget associated with the variable
void update(const void* var);

// refresh
void refresh();

// previous menu
void back();

// main menu
void home();

// button
void set();
void up();
void down();
void left();
void right();

// renewal
void setRefreshFull();  // screen
void setRefreshRow();   // line
void setRefreshPart();  // partial

// Get the current widget number
uint8_t getWidgetIndex();

// editing mode
bool isEditing();
void cancelEdit();
```

### gm::Builder
> Under`Str`further referred to`const char*`, `const __FlashStringHelper*`, `const String&`

#### Page
```cpp
// start the page (submenu)
bool PageBegin(Str label); // automatic ID
bool PageBegin(uint8_t id, Str label);

// end the page (call inside the terms by PageBegin). back - output the "back" button
void PageEnd(bool back = true);

// Callback page (instead of PageBegin-PageEnd) back - output the "back" button
template <typename PageCb>
void Page(Str label, const PageCb& page, bool back = true);
template <typename PageCb>
void Page(uint8_t id, Str label, const PageCb& page, bool back = true);
```

#### Widgets
```cpp
// button
bool Button(Str label, void (*cb)() = nullptr);

// text
void Label(Str line);

// display-only
void ValueStr(Str label, const char* var);
void ValueInt(Str label, const T* var, uint8_t base = 10, Str unit = "");
void ValueFloat(Str label, const float* var, uint8_t dec = 2, Str unit = "");

// switcher
bool Switch(Str label, bool* var, void (*cb)(bool v) = nullptr);

// Item selection. opts is a line with a separator ';'
bool Select(Str label, uint8_t* var, Str opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// Selection of the item in the style of "tabs". tabs - a line with a separator ';'
bool Tabs(uint8_t* var, Str tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// editable
template <typename T>
bool EditInt(Str label, T* var, T minv, T maxv, T step, Str unit = "", void (*cb)(T v) = nullptr);

// float-value
bool EditFloat(Str label, float* var, float minv, float maxv, float step, uint8_t dec = 2, Str unit = "", void (*cb)(float v) = nullptr);

// editing a line from an embedded or user alphabet
// var buffer must have a minimum size of maxLen + 1
bool EditStr(Str label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr);
bool EditStr(Str label, char* var, uint8_t maxLen, Str alphabet, void (*cb)(const char* str) = nullptr);

// editing of printed ASCII characters 32..126
// var buffer must have a minimum size of maxLen + 1
bool EditASCII(Str label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr);

// time, T is a structure with fields .second, .minute, .hour or built-in gm::Time
template <typename T>
bool Time(Str label, T* var, void (*cb)(T v) = nullptr);

// D is a structure with fields .day, .month, .year or built-in gm::Date
template <typename D>
bool Date(Str label, D* var, void (*cb)(D v) = nullptr);

// bit mask, bits - number of minor bits, output - MSB on the left
template <typename T>
bool Bitmask(Str label, T* var, uint8_t bits, void (*cb)(T v) = nullptr);
```

#### Systemic
```cpp
// Update the screen after the builder
void refresh();

// Will there be a full refresh after the current action?
bool isRefresh();

// There was an action with one of the widgets above.
bool wasSet();

// dropped the wasSet flag
void clearSet();
```

#### APIs for their widgets
```cpp
// register a widget without checking availability
bool registerWidget();

// register the widget and check if the current pass is related to it
bool beginWidget();

// take current action
Action getAction();

// start rendering the widget; with the label library serves the signature and part
bool beginRender(const void* targetVar = nullptr, const char* label = nullptr);

// align further output to the right on the width of the characters, with a lack of space output "ovf"
bool prepareRight(uint8_t width);

// Raise the change flag (affects wasSet())
void change();

// Get a new id for Page
uint8_t nextId();

// A low-level menu item is available as b.menu
Menu& menu;
```

Basic methods`b.menu`for their widgets:
```cpp
// seal
void menu.print(char c);
void menu.print(const char* str);
void menu.print(const char* str, uint8_t len, uint8_t letters = 0);
void menu.printDec2z(uint8_t v);   // 2 digits with lead zero
void menu.printDec4z(uint16_t v);  // 4 digits with leading zeros

// filling-in
void menu.pad(int8_t n);
void menu.padEnd(int8_t n = 0);

// set the withdrawal position. Need onCursor()
bool menu.setPos(uint8_t col);
bool menu.setPos(uint8_t col, uint8_t row);

// widget
void menu.toggle();
uint8_t menu.currentRow();
bool menu.isActive();
bool menu.isChosen();
bool menu.isVisible();
```

### Define settings
Announced before connecting the library

```cpp
#define GM_MAX_DEPTH 5  // max. the nesting of the menu (no. 5)
#define GM_NO_PAGES     // Disable the attached menu (facilitates the library)
#define GM_NO_PART      // disable partial output of values in Part (facilitates the library)
```

<a id="usage"></a>

## Use of use
### Minimum example
```cpp
#include <GyverMenu.h>

GyverMenu menu(16, 2);
bool sw;
int value;

void setup() {
    menu.onPrint([](const char* str, size_t len) {
        if (str) lcd.Print::write(str, len);
    });

    menu.onCursor([](uint8_t col, uint8_t row) {
        lcd.setCursor(col, row);
    });

    menu.onState([](uint8_t row, bool selected, bool editing) {
        lcd.print(selected && !editing ? '>' : ' ');
    });

    menu.onBuild([](gm::Builder& b) {
        b.Switch("Power", &sw);
        b.EditInt("Value", &value, 0, 100, 1);
    });

    menu.refresh();
}

void loop() {
    // Call menu.up/down/set/left/right from your button or encoder events
}
```

### How it works.
The library stores only the navigation state, and the menu itself is virtual and each time described in the builder. The builder can be executed several times for different tasks: process the action, find the desired widget, redraw a line or the entire screen. Therefore, the set and order of widgets in one menu state should be reproducible, and it is better not to perform heavy work inside the builder.

As entry points, the library has 5 virtual buttons that can be associated with any physical button, joystick or encoder. Processors are used as exit points:
- `onBuild(gm::Builder& b)`- menu-building
- `onPrint(const char* str, size_t len)`output
- `onCursor(uint8_t col, uint8_t row)`- setting the output position
- `onState(uint8_t row, bool selected, bool editing)`- line state design

The library is not tied to a specific display and input method. The text menu can be displayed on the LCD, OLED, terminal or other interface, and for the graphic display you can write your own widgets.

### Buttons
The library provides **virtual** 5 control buttons + individual commands "return to the previous menu" and "home screen", but in practice it can be any other number of buttons - at least one, at least an encoder. The logic of the work is:

- `menu.set()`- central selection button. She presses the widget.`Button`on the screen, switches the widget state`Switch`, handles pressing the return button to the previous menu, and for other widgets - transfers them to change mode
- `menu.up()`Move the cursor to the previous point. If the change mode is active, an increase in value will be caused instead.
- `menu.down()`- move the cursor to the next point. If the change mode is active, a decrease in value will be caused instead.
- `menu.left()`Directly reduces the value of the current widget regardless of the mode of change. Also presses the back button to return to the previous menu.
- `menu.right()`- directly increases the value of the current widget regardless of the mode of change. Also presses the submenu and widget entry button.`Button`
- `menu.back()`- returns to the previous menu
- `menu.home()`- returns to the main menu
- `menu.isEditing()`Check if the current widget is in editing mode
- `menu.cancelEdit()`- withdraw from editing. In`Full`The whole screen will be updated,`Row/Part`- current line

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
Four processors are used for the normal text menu. Only the builder is mandatory.`onCursor`required for addressable positioning and mode`Part`, `onState`It is optional and is used only for the state of the line.

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
    // str - output line
    // str = = = nullptr - the current rendering is finished, you can update the display or move the line to the console
    // len is the length of a string in bytes
}
menu.onPrint(printer);
```

#### Courier
`onCursor`establishes a withdrawal position and is necessary for the regime`Part`:
```cpp
void cursor(uint8_t col, uint8_t row) {
    // output
}
menu.onCursor(cursor);
```

#### Status.
`onState`is called before rendering the line and separately when moving along the lines:
```cpp
void state(uint8_t row, bool selected, bool editing) {
    // row
    // Selected - line selected
    // editing - the selected widget is in change mode
}
menu.onState(state);
```

Before.`onState`The library sets the position at the beginning of the line`onCursor`If it's connected.`row`remains`onState`So that the processor can work independently, for example in Serial or graphic renderer. If the design of the state occupies one column on the left, leave`setMarkerSize(true)`(silence) if it doesn't --`setMarkerSize(false)`.

The geometry of the output remains in`onCursor`.

Example for cursored LCD`>`:
```cpp
menu.onPrint([](const char* str, size_t len) {
    if (str) lcd.Print::write(str, len);
});

menu.onCursor([](uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
});

menu.onState([](uint8_t row, bool selected, bool editing) {
    lcd.print(selected && !editing ? '>' : ' ');
});
```

For OLED, you can not occupy a separate column, but, for example, invert the selected row:

```cpp
menu.setMarkerSize(false);

menu.onCursor([](uint8_t col, uint8_t row) {
    oled.setCursor(col, row);
});

menu.onState([](uint8_t row, bool selected, bool editing) {
    oled.invertText(selected);
});
```

<a id="refresh-modes"></a>

### Update modes
The mode is defined by:
```cpp
menu.setRefreshMode(gm::RefreshMode::Part);
```

or short functions:
```cpp
menu.setRefreshPart();
menu.setRefreshRow();
menu.setRefreshFull();
```

- `Part`- minimal redrawing. When moving along the visible lines, only the cursor design changes, and when repeated changes to the selected widget, the built-in widgets update only the value area. The first change after navigation, input and output from editing output the entire line. Demands.`onCursor`. Partial output of values can be disabled through`GM_NO_PART`To reduce flash, a quick update of the marker during navigation is saved.
- `Row`When moving, the old and new lines are redrawn, the value of the widget is also updated in the line. It's the default mode.
- `Full`After the action, the entire screen is redrawn. Suitable for a terminal, framebuffer, or render where a partial update makes no sense

Usually:
- LCD with addressable cursor -`Part`
- simple OLED text -`Part`or`Row`
- framebuffer/terminal/render in full -`Full`

`menu.update(&var)`Starts a search for the widget associated with this variable and updates only it. Update through`update()`outputs the widget in the usual way without using the previous Part-context.

If the menu structure changes, for example`Switch`Other widgets appear or disappear - call`b.refresh()`from the builder after the change:
```cpp
if (b.Switch("Extra", &showExtra)) b.refresh();
if (showExtra) b.Button("Extra button");
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

<a id="custom-widgets"></a>

### Their widgets.
Built-in values are divided by behavior:`Value*`- just the display,`Edit*`- editing.

System widgets are built on the same API that is available to the user. The minimum widget usually consists of`beginWidget()`processing`getAction()`and`beginRender()`:

- `beginRender(var)`The widget draws a line on its own
- `beginRender(var, label)`The library maintains the label and, where possible, uses it.`Part`field
- `prepareRight(width)`- aligns further output to the right and remembers the width of the area for subsequent`Part`

Example of a simple numerical widget with a 2-character block and support`update(&var)`/`Part`:
```cpp
bool MyValue(gm::Builder& b, const char* label, uint8_t* var) {
    if (!b.beginWidget()) return false;

    bool changed = false;
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
            if (*var < 99) {
                ++*var;
                render = changed = true;
                b.change();
            }
            break;

        case gm::Builder::Action::SetDown:
        case gm::Builder::Action::Left:
            if (*var) {
                --*var;
                render = changed = true;
                b.change();
            }
            break;

        default:
            break;
    }

    if (render && b.beginRender(var, label) && b.prepareRight(2)) {
        b.menu.printDec2z(*var);
    }

    return changed;
}
```

Use:
```cpp
uint8_t value;

menu.onBuild([](gm::Builder& b) {
    MyValue(b, "Value", &value);
});

// Change the value from the program and update only the associated widget
value = 42;
menu.update(&value);
```

The same variable length is used.`beginRender(var, label)`in`prepareRight()`The actual width of the value is transmitted:
```cpp
if (b.beginRender(var, label) && b.prepareRight(valueWidth)) {
    b.menu.print(value);
}
```

For a fully graphical widget, use`beginRender(var)`without`label`And draw directly into your display.`beginRender()`anyway`onState`Therefore, renderer gets relevant`selected/editing`.

<a id="limits"></a>

### Limitations and features
- If the value does not fit into the display, an ellipsis will be displayed instead.
- The builder is declarative and can be called several times per user action. Do not perform delays, network requests and other heavy work.
- The order of widgets determines their indexes and navigation. If the condition changes the set of widgets, after changing the structure, you need to`b.refresh()`
- `menu.update(ptr)`Find the widget by the pointer transmitted to it`beginRender`. For a predictable update, use a unique variable for the updated widget.
- `Part`requires connected`onCursor`. Without it, partial positioning is impossible.
- `beginRender(var, label)`It is assumed that the variable area is after`label`The first output is executed in its entirety, the following changes of the selected widget can only update the value area.
- For right alignment, call in`prepareRight(width)`with the actual width of the output area. If there is a shortage of space, it will be removed.`ovf`
- `markerSize`is equal to one column by default. For renderer without a separate cursor symbol, call`setMarkerSize(false)`
- `Time`The structure must have fields`hour`, `minute`, `second`, `Date` - `day`, `month`, `year`
- `Bitmask`: `bits`limited by the size of the variable type
- `GM_NO_PAGES`Disables pages and reduces library size
- `GM_NO_PART`Disables partial output of the variable widget area. Quick update of the marker when navigating in mode`Part`persistence
- After resizing the display`resize(cols, rows)`drops navigation to the root

### Introduction of text
widget`EditStr`works as follows:
- `EditStr`The library uses a standard alphabet: 0-9 A-Z a-z characters. You can transfer the external alphabet.
- `EditASCII`"lighter" and uses a blank ASCII table, no alphabet storage
- The buffer must have a minimum size.`maxLen + 1`final`\0`
- The length of the line can be changed only on the right - you need to put the cursor on the last position, behind the line:`foo>`. If you press "down" - the line will be shortened by one character, if "up" - a new character will be added in front of the cursor.

#### Tricks.
In widgets with step adjustment, the step can be set dynamically, for example, from the speed of rotation of the encoder. With the EncButton or uEncoder library, it might look like this:

```cpp
int vali;

menu.onBuild([](gm::Builder& b) {
    b.EditInt<int>("EditInt", &vali, -100, 100, encb.fast() ? 10 : 1);
});
```

That is, with a fast rotation of the encoder, the step will be 10 (rough), and with a slow one - 1 (exact). The builder is called at each action of changing the value, so the step will be selected for each click.

### Examples
In a file.`examples`There are some examples for:
- LCD and OLED
- Update regimes
- Pages and navigation
- Variable updates through`update`
- Own text and graphic widgets

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
