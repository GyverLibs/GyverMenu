#pragma once
#include <sbuild.h>
#include <stdlib.h>

#include "Menu.h"

#ifndef GM_MIN_YEAR
#define GM_MIN_YEAR 2000
#endif

#define GM_INT_BUFFER 22
#define GM_FLT_BUFFER 18
#define GM_NEXT __COUNTER__

namespace gm {

// MARK: Alphabet
static const char _gmInputAlphabet[] GM_PROGMEM =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";

// MARK: time
struct Time {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
};

struct Date {
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

// MARK: builder
class Builder {
   public:
    typedef void (*BuildCb)(gm::Builder&);

    enum class Action : uint8_t {
        None,
        Refresh,
        Set,
        SetDown,
        SetUp,
        Left,
        Right,
    };

    // =================== CTR ===================
    // создать билдер для указанного меню
    Builder(Menu& menu) : menu(menu) {}

    // выполнить полный проход обновления экрана
    void buildRefresh(BuildCb cb) {
        buildRow(cb, -1);
        if (isRefresh()) buildRow(cb, -1);
        if (isRefresh()) buildRow(cb, -1);
        menu.endRender();
    }

    // выполнить проход для отрисовки одной строки, -1 для всех видимых
    void buildRow(BuildCb cb, int8_t targetRow) {
        menu.resetPart();
        _init();
        _action = Action::Refresh;
        _targetRow = targetRow;
        _build(cb);
        if (targetRow == -1) menu.clearBottom();
    }

    // найти и обновить виджет, связанный с переменной
    void buildUpdate(BuildCb cb, const void* targetVar) {
        menu.resetPart();
        _init();
        _action = Action::Refresh;
        _targetVar = targetVar;
        _build(cb);
        if (_refresh) menu.endRender();
    }

    // выполнить проход обработки действия
    void buildAction(BuildCb cb, Action action, bool skipRender) {
        _init();
        _action = action;
        _refresh = skipRender;
        _build(cb);
    }

#ifndef GM_NO_PAGES
// =================== PAGE ===================
// MARK: page
#ifdef ARDUINO
    bool PageBegin(const __FlashStringHelper* label) {
        GM_READ_PGM(label, label_s);
        return PageBegin(label_s);
    }

    bool PageBegin(uint8_t id, const __FlashStringHelper* label) {
        GM_READ_PGM(label, label_s);
        return PageBegin(id, label_s);
    }

    bool PageBegin(const String& label) {
        return PageBegin(label.c_str());
    }

    bool PageBegin(uint8_t id, const String& label) {
        return PageBegin(id, label.c_str());
    }

#endif
    // начать страницу (подменю) с автоматическим id
    bool PageBegin(const char* label) {
        return PageBegin(nextId(), label);
    }

    // начать страницу (подменю)
    bool PageBegin(uint8_t id, const char* label) {
        if (registerWidget()) {
            if (!_reachable()) return false;

            switch (getAction()) {
                case Action::Refresh:
                    if (beginRender()) {
                        menu.print(label);
                        menu.padEnd(1);
                        menu.print(menu.marker);
                    }
                    break;

                case Action::Set:
                case Action::Right:
                    menu.pages.open(id);
                    menu.resetPart();
                    refresh();
                    break;

                default: break;
            }
            return false;
        }
        return menu.pages.enter(id);
    }

    // закончить страницу (вызывать внутри условия по PageBegin). back - выводить кнопку "назад"
    void PageEnd(bool back = true) {
        if (back && registerWidget() && _reachable()) {
            switch (getAction()) {
                case Action::Refresh:
                    if (beginRender()) {
                        menu.print(menu.backSign);
                        menu.padEnd();
                    }
                    break;

                case Action::Set:
                case Action::Left:
                    menu.pages.close();
                    menu.resetPart();
                    refresh();
                    break;

                default: break;
            }
        }
        if (menu.pages.target) _achieved = true;
        menu.pages.leave();
    }

#ifdef ARDUINO
    template <typename PageCb>
    void Page(const __FlashStringHelper* label, const PageCb& page, bool back = true) {
        GM_READ_PGM(label, label_s);
        Page(label_s, page, back);
    }

    template <typename PageCb>
    void Page(uint8_t id, const __FlashStringHelper* label, const PageCb& page, bool back = true) {
        GM_READ_PGM(label, label_s);
        Page(id, label_s, page, back);
    }

    template <typename PageCb>
    void Page(const String& label, const PageCb& page, bool back = true) {
        Page(label.c_str(), page, back);
    }

    template <typename PageCb>
    void Page(uint8_t id, const String& label, const PageCb& page, bool back = true) {
        Page(id, label.c_str(), page, back);
    }
#endif
    // Захват внешних переменных для лямбда-функции элемента Page
    template <typename PageCb>
    void Page(const char* label, const PageCb& page, bool back = true) {
        Page(nextId(), label, page, back);
    }

    template <typename PageCb>
    void Page(uint8_t id, const char* label, const PageCb& page, bool back = true) {
        if (PageBegin(id, label)) {
            page(*this);
            PageEnd(back);
        }
    }
#endif

    // =================== BUTTON ===================
    // MARK: button
#ifdef ARDUINO
    bool Button(const __FlashStringHelper* label, void (*cb)() = nullptr) {
        GM_READ_PGM(label, label_s);
        return Button(label_s, cb);
    }

    bool Button(const String& label, void (*cb)() = nullptr) {
        return Button(label.c_str(), cb);
    }
#endif

    bool Button(const char* label, void (*cb)() = nullptr) {
        if (!beginWidget()) return false;

        switch (getAction()) {
            case Action::Refresh:
                if (beginRender()) {
                    menu.print(label);
                    menu.padEnd();
                }
                break;

            case Action::Set:
            case Action::Right:
                if (cb) cb();
                return true;

            default: break;
        }

        return false;
    }

    // =================== LABEL ===================
    // MARK: label
#ifdef ARDUINO
    void Label(const __FlashStringHelper* line) {
        GM_READ_PGM(line, line_s);
        Label(line_s);
    }

    void Label(const String& line) {
        Label(line.c_str());
    }
#endif

    void Label(const char* line) {
        if (beginWidget() && getAction() == Action::Refresh && beginRender()) {
            menu.print(line);
            menu.padEnd();
        }
    }

    // =================== VALUE STR ===================
    // MARK: value
#ifdef ARDUINO
    void ValueStr(const __FlashStringHelper* label, const char* value) {
        GM_READ_PGM(label, label_s);
        ValueStr(label_s, value);
    }

    void ValueStr(const String& label, const char* value) {
        ValueStr(label.c_str(), value);
    }
#endif

    void ValueStr(const char* label, const char* var) {
        if (beginWidget() && getAction() == Action::Refresh && beginRender(var, label)) {
            uint8_t letters;
            uint8_t len = gmutil::strlenU(var, &letters);
            if (prepareRight(letters)) menu.print(var, len, letters);
        }
    }

    // =================== VALUE INT ===================
#ifdef ARDUINO
    template <typename T>
    void ValueInt(const __FlashStringHelper* label, const T* var, uint8_t base, const __FlashStringHelper* unit) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        ValueInt(label_s, var, base, unit_s);
    }

    template <typename T>
    void ValueInt(const String& label, const T* var, uint8_t base, const String& unit) {
        ValueInt(label.c_str(), var, base, unit.c_str());
    }
#endif

    template <typename T>
    void ValueInt(const char* label, const T* var, uint8_t base = 10, const char* unit = "") {
        if (beginWidget() && getAction() == Action::Refresh && beginRender(var, label)) _printVar(var, base, unit);
    }

// =================== VALUE FLOAT ===================
#ifdef ARDUINO
    void ValueFloat(const __FlashStringHelper* label, const float* var, uint8_t dec, const __FlashStringHelper* unit) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        ValueFloat(label_s, var, dec, unit_s);
    }

    void ValueFloat(const String& label, const float* var, uint8_t dec, const String& unit) {
        ValueFloat(label.c_str(), var, dec, unit.c_str());
    }
#endif

    void ValueFloat(const char* label, const float* var, uint8_t dec = 2, const char* unit = "") {
        if (beginWidget() && getAction() == Action::Refresh && beginRender(var, label)) _printVar(var, dec, unit);
    }

// =================== SWITCH ===================
// MARK: switch
#ifdef ARDUINO
    bool Switch(const __FlashStringHelper* label, bool* var, void (*cb)(bool v) = nullptr) {
        GM_READ_PGM(label, label_s);
        return Switch(label_s, var, cb);
    }

    bool Switch(const String& label, bool* var, void (*cb)(bool v) = nullptr) {
        return Switch(label.c_str(), var, cb);
    }
#endif

    bool Switch(const char* label, bool* var, void (*cb)(bool v) = nullptr) {
        if (!beginWidget()) return false;
        bool changed = false;
        bool render = false;

        switch (getAction()) {
            case Action::Refresh:
                render = true;
                break;

            case Action::Set:
                *var = !*var;
                render = changed = true;
                change();
                break;

            case Action::SetUp:
            case Action::Right:
                if (!*var) {
                    *var = true;
                    render = changed = true;
                    change();
                }
                break;

            case Action::SetDown:
            case Action::Left:
                if (*var) {
                    *var = false;
                    render = changed = true;
                    change();
                }
                break;

            default: break;
        }

        if (changed && cb) cb(*var);

        if (render && beginRender(var, label) && prepareRight(3)) {
            menu.print(*var ? "[x]" : "[ ]", 3);
        }

        return changed;
    }

// =================== SELECT ===================
// MARK: select
#ifdef ARDUINO
    bool Select(const __FlashStringHelper* label, uint8_t* var, const __FlashStringHelper* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(opts, opts_s);
        return Select(label_s, var, opts_s, cb);
    }

    bool Select(const String& label, uint8_t* var, const String& opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        return Select(label.c_str(), var, opts.c_str(), cb);
    }
#endif

    bool Select(const char* label, uint8_t* var, const char* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        if (!beginWidget()) return false;
        bool changed = false;
        bool render = false;

        switch (getAction()) {
            case Action::Refresh:
                render = true;
                break;

            case Action::Set:
                menu.toggle();
                render = true;
                break;

            case Action::SetUp:
            case Action::Right:
                if (*var < gmutil::countSub(opts, ';') - 1) {
                    ++*var;
                    render = changed = true;
                    change();
                }
                break;

            case Action::SetDown:
            case Action::Left:
                if (*var) {
                    --*var;
                    render = changed = true;
                    change();
                }
                break;

            default: break;
        }

        const char* start = nullptr;
        uint8_t len = 0;
        if (changed || render) len = gmutil::getSub(opts, ';', *var, &start);

        if (changed && cb) cb(*var, start, len);

        if (render && beginRender(var, label)) {
            uint8_t letters;
            gmutil::strlenU(start, &letters, len);

            if (prepareRight(letters + 2)) {
                menu.print('<');
                menu.print(start, len, letters);
                menu.print(menu.marker);
            }
        }

        return changed;
    }

// =================== TABS ===================
// MARK: tabs
#ifdef ARDUINO
    bool Tabs(uint8_t* var, const __FlashStringHelper* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        GM_READ_PGM(tabs, tabs_s);
        return Tabs(var, tabs_s, cb);
    }

    bool Tabs(uint8_t* var, const String& tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        return Tabs(var, tabs.c_str(), cb);
    }
#endif

    bool Tabs(uint8_t* var, const char* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        if (!beginWidget()) return false;
        bool changed = false;
        bool render = false;

        switch (getAction()) {
            case Action::Refresh:
                render = true;
                break;

            case Action::Set:
                menu.toggle();
                render = true;
                break;

            case Action::SetUp:
            case Action::Right:
                if (*var < gmutil::countSub(tabs, ';') - 1) {
                    ++*var;
                    render = changed = true;
                    change();
                }
                break;

            case Action::SetDown:
            case Action::Left:
                if (*var) {
                    --*var;
                    render = changed = true;
                    change();
                }
                break;

            default: break;
        }

        const char* start = nullptr;
        uint8_t slen = 0;
        if (changed || render) slen = gmutil::getSub(tabs, ';', *var, &start);

        if (changed && cb) cb(*var, start, slen);

        if (render && beginRender(var)) {
            int16_t start_i = (menu.left / 2) - ((start - tabs) + slen / 2);
            int16_t len = strlen(tabs);
            int16_t i = 0;
            while (menu.left) {
                if (i >= start_i && i <= start_i + len) {
                    const char* t = tabs + i - start_i;
                    if (t == start) menu.print(menu.isActive() ? '<' : '[');
                    else if (t == start + slen) menu.print(menu.isActive() ? '>' : ']');
                    menu.print((!*t || *t == ';') ? ' ' : *t);
                } else {
                    menu.print(' ');
                }
                i++;
            }
        }

        return changed;
    }

    // =================== TIME ===================
    // MARK: time

#ifdef ARDUINO
    template <typename T>
    bool Time(const __FlashStringHelper* label, T* var, void (*cb)(T v) = nullptr) {
        GM_READ_PGM(label, label_s);
        return Time(label_s, var, cb);
    }

    template <typename T>
    bool Time(const String& label, T* var, void (*cb)(T v) = nullptr) {
        return Time(label.c_str(), var, cb);
    }
#endif

    template <typename T>
    bool Time(const char* label, T* var, void (*cb)(T v) = nullptr) {
        if (!beginWidget()) return false;

        bool changed = false;
        int8_t dir = 0;
        int8_t curdir = 0;

        _editDir(dir, curdir);
        _moveActive(curdir, 3);

        if (dir && menu.active) {
            switch (menu.active) {
                case 1: var->hour = _clampRing((int)var->hour + dir, 0, 23); break;
                case 2: var->minute = _clampRing((int)var->minute + dir, 0, 59); break;
                case 3: var->second = _clampRing((int)var->second + dir, 0, 59); break;
            }

            changed = true;
            change();
            if (cb) cb(*var);
        }

        if (beginRender(var, label) && prepareRight(9)) {
            bool act = menu.isActive();
            menu.print(act && menu.active == 1 ? menu.marker : ' ');
            menu.printDec2z(var->hour);
            menu.print(act && menu.active == 2 ? menu.marker : ':');
            menu.printDec2z(var->minute);
            menu.print(act && menu.active == 3 ? menu.marker : ':');
            menu.printDec2z(var->second);
        }

        return changed;
    }

    // =================== DATE ===================
    // MARK: date

#ifdef ARDUINO
    template <typename D>
    bool Date(const __FlashStringHelper* label, D* var, void (*cb)(D v) = nullptr) {
        GM_READ_PGM(label, label_s);
        return Date(label_s, var, cb);
    }

    template <typename D>
    bool Date(const String& label, D* var, void (*cb)(D v) = nullptr) {
        return Date(label.c_str(), var, cb);
    }
#endif

    template <typename D>
    bool Date(const char* label, D* var, void (*cb)(D v) = nullptr) {
        if (!beginWidget()) return false;

        bool changed = false;
        int8_t dir = 0;
        int8_t curdir = 0;

        _editDir(dir, curdir);
        _moveActive(curdir, 3);

        if (dir && menu.active) {
            switch (menu.active) {
                case 1: var->day = _clampRing((int)var->day + dir, 1, 31); break;
                case 2: var->month = _clampRing((int)var->month + dir, 1, 12); break;
                case 3:
                    if (var->year < GM_MIN_YEAR) {
                        var->year = GM_MIN_YEAR;
                    } else if (dir > 0 && var->year < 9999) {
                        ++var->year;
                    } else if (dir < 0 && var->year > GM_MIN_YEAR) {
                        --var->year;
                    }
                    break;
            }

            changed = true;
            change();
            if (cb) cb(*var);
        }

        if (beginRender(var, label) && prepareRight(11)) {
            bool act = menu.isActive();
            menu.print(act && menu.active == 1 ? menu.marker : ' ');
            menu.printDec2z(var->day);
            menu.print(act && menu.active == 2 ? menu.marker : '.');
            menu.printDec2z(var->month);
            menu.print(act && menu.active == 3 ? menu.marker : '.');
            menu.printDec4z(var->year);
        }

        return changed;
    }

    // =================== BITMASK ===================
    // MARK: bitmask

#ifdef ARDUINO
    template <typename T>
    bool Bitmask(const __FlashStringHelper* label, T* var, uint8_t bits, void (*cb)(T v) = nullptr) {
        GM_READ_PGM(label, label_s);
        return Bitmask(label_s, var, bits, cb);
    }

    template <typename T>
    bool Bitmask(const String& label, T* var, uint8_t bits, void (*cb)(T v) = nullptr) {
        return Bitmask(label.c_str(), var, bits, cb);
    }
#endif

    template <typename T>
    bool Bitmask(const char* label, T* var, uint8_t bits, void (*cb)(T v) = nullptr) {
        if (!beginWidget()) return false;
        if (!bits) return false;

        uint8_t maxBits = sizeof(T) * 8;
        if (bits > maxBits) bits = maxBits;

        bool changed = false;
        int8_t dir = 0;
        int8_t curdir = 0;

        _editDir(dir, curdir);
        _moveActive(curdir, bits);

        if (dir && menu.isActive()) {
            uint8_t bit = bits - menu.active;
            T mask = ((T)1 << bit);

            T prev = *var;

            if (dir > 0) {
                *var |= mask;
            } else {
                *var &= ~mask;
            }

            if (*var != prev) {
                changed = true;
                change();
                if (cb) cb(*var);
            }
        }

        if (beginRender(var, label) && prepareRight(bits + 1)) {
            bool act = menu.isActive();
            if (!act) menu.print(' ');

            for (uint8_t i = 0; i < bits; i++) {
                if (act && menu.active == i + 1) menu.print(menu.marker);

                uint8_t bit = bits - 1 - i;
                menu.print((*var & ((T)1 << bit)) ? '1' : '0');
            }
        }

        return changed;
    }

    // =================== EDIT STR ===================
    // MARK: edit str

#ifdef ARDUINO

    // default AB
    bool EditStr(const __FlashStringHelper* label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr) {
        GM_READ_PGM(label, label_s);
        return EditStr(label_s, var, maxLen, cb);
    }
    bool EditStr(const String& label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr) {
        return EditStr(label.c_str(), var, maxLen, cb);
    }

    // custom AB
    bool EditStr(const __FlashStringHelper* label, char* var, uint8_t maxLen, const __FlashStringHelper* alphabet, void (*cb)(const char* str) = nullptr) {
        GM_READ_PGM(label, label_s);
        return EditStr(label_s, var, maxLen, alphabet, cb);
    }
    bool EditStr(const String& label, char* var, uint8_t maxLen, const __FlashStringHelper* alphabet, void (*cb)(const char* str) = nullptr) {
        return EditStr(label.c_str(), var, maxLen, alphabet, cb);
    }
    bool EditStr(const char* label, char* var, uint8_t maxLen, const __FlashStringHelper* alphabet, void (*cb)(const char* str) = nullptr) {
        return _editStr(label, var, maxLen, (const char*)alphabet, cb);
    }
#else
    bool EditStr(const char* label, char* var, uint8_t maxLen, const char* alphabet, void (*cb)(const char* str) = nullptr) {
        return _editStr(label, var, maxLen, alphabet, cb);
    }
#endif

    bool EditStr(const char* label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr) {
        return _editStr(label, var, maxLen, _gmInputAlphabet, cb);
    }

    // =================== EDIT ASCII ===================
    // MARK: edit ascii

#ifdef ARDUINO
    bool EditASCII(const __FlashStringHelper* label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr) {
        GM_READ_PGM(label, label_s);
        return EditASCII(label_s, var, maxLen, cb);
    }

    bool EditASCII(const String& label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr) {
        return EditASCII(label.c_str(), var, maxLen, cb);
    }
#endif

    bool EditASCII(const char* label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr) {
        if (!beginWidget()) return false;
        if (!maxLen) return false;

        uint8_t len = 0;
        while (len < maxLen && var[len]) ++len;
        var[len] = '\0';

        bool changed = false;
        int8_t dir = 0;
        int8_t curdir = 0;

        // active:
        // 0       - редактирование выключено
        // 1..len  - символы строки
        // len + 1 - техническая позиция справа

        _editDir(dir, curdir);
        _moveActive(curdir, len + 1);

        if (dir && menu.active) {
            uint8_t pos = menu.active - 1;

            if (pos < len) {
                // ASCII: 32 (' ') ... 126 ('~')
                uint8_t c = (uint8_t)var[pos];

                if (c < 32 || c > 126) {
                    c = dir > 0 ? 32 : 126;
                } else if (dir > 0) {
                    c = (c < 126) ? c + 1 : 32;
                } else {
                    c = (c > 32) ? c - 1 : 126;
                }

                if (var[pos] != (char)c) {
                    var[pos] = (char)c;
                    changed = true;
                }
            } else {
                if (dir > 0) {
                    if (len < maxLen) {
                        var[len++] = 'A';
                        var[len] = '\0';
                        changed = true;
                    }
                } else {
                    if (len) {
                        var[--len] = '\0';
                        menu.active = len + 1;
                        changed = true;
                    }
                }
            }

            if (changed) {
                change();
                if (cb) cb(var);
            }
        }

        if (beginRender(var, label)) {
            uint8_t width = len + (menu.isActive() ? 1 : 0);

            if (prepareRight(width)) {
                if (menu.isActive()) {
                    uint8_t pos = menu.active - 1;

                    for (uint8_t i = 0; i < len; i++) {
                        if (i == pos) menu.print(menu.marker);
                        menu.print(var[i]);
                    }

                    if (pos == len) menu.print(menu.marker);
                } else {
                    menu.print(var);
                }
            }
        }

        return changed;
    }

    // =================== EDIT_INT ===================
    // MARK: edit int

#ifdef ARDUINO
    template <typename T>
    bool EditInt(const __FlashStringHelper* label, T* var, T minv, T maxv, T step, const __FlashStringHelper* unit, void (*cb)(T v) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        return _editNum(label_s, var, minv, maxv, step, 10, unit_s, cb);
    }

    template <typename T>
    bool EditInt(const String& label, T* var, T minv, T maxv, T step, const String& unit, void (*cb)(T v) = nullptr) {
        return _editNum(label.c_str(), var, minv, maxv, step, 10, unit.c_str(), cb);
    }
#endif

    template <typename T>
    bool EditInt(const char* label, T* var, T minv, T maxv, T step, const char* unit = "", void (*cb)(T v) = nullptr) {
        return _editNum(label, var, minv, maxv, step, 10, unit, cb);
    }

    // =================== EDIT_FLOAT ===================
    // MARK: edit float

#ifdef ARDUINO
    bool EditFloat(const __FlashStringHelper* label, float* var, float minv, float maxv, float step, uint8_t dec, const __FlashStringHelper* unit, void (*cb)(float v) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        return _editNum(label_s, var, minv, maxv, step, dec, unit_s, cb);
    }

    bool EditFloat(const String& label, float* var, float minv, float maxv, float step, uint8_t dec, const String& unit, void (*cb)(float v) = nullptr) {
        return _editNum(label.c_str(), var, minv, maxv, step, dec, unit.c_str(), cb);
    }
#endif

    bool EditFloat(const char* label, float* var, float minv, float maxv, float step, uint8_t dec = 2, const char* unit = "", void (*cb)(float v) = nullptr) {
        return _editNum(label, var, minv, maxv, step, dec, unit, cb);
    }

    // =================== MISC ===================

    // обновить экран после работы билдера
    void refresh() {
        switch (_action) {
            case Action::Set:
            case Action::SetUp:
            case Action::SetDown:
            case Action::Right:
            case Action::Left:
                _refresh = true;
                break;

            default: break;
        }
    }

    // будет обновление после вызова
    bool isRefresh() {
        return _refresh;
    }

    // было действие с каким-то из виджетов выше
    bool wasSet() {
        return _changed;
    }

    // сбросить флаг чтения wasSet
    void clearSet() {
        _changed = false;
    }

    // ===================== API =====================
    // MARK: api

    // зарегистрировать виджет без проверки доступности
    bool registerWidget() {
        return menu.registerWidget();
    }

    // зарегистрировать виджет и проверить, относится ли проход к нему
    bool beginWidget() {
        return registerWidget() && _reachable();
    }

    // получить текущее действие билдера
    Action getAction() {
        return _action;
    }

    // начать вывод виджета, с label - изменяемой части после него
    bool beginRender(const void* targetVar = nullptr, const char* label = nullptr) {
        if (_action != Action::Refresh && _refresh) return false;
        if (_targetRow >= 0 && _targetRow != menu.row()) return false;

        if (_targetVar) {
            if (_targetVar != targetVar) return false;
            _refresh = true;
        }

#ifndef GM_NO_PART
        if (label && _action != Action::Refresh && menu.prevLen >= 0 && menu.usePart()) {
            uint8_t letters;
            gmutil::strlenU(label, &letters);
            menu.setPos(menu.markerSize + letters + menu.isActive());
            return true;
        }
#endif

        menu.beginRow();
        if (label) {
            menu.print(label);
            if (menu.isActive()) menu.print(':');
        }
        return true;
    }

    // подготовить правое выравнивание, вывести ovf если места нет
    bool prepareRight(uint8_t width) {
        if (menu.left < width) {
            menu.padEnd(2);
            menu.print('.');
            menu.print('.');
#ifndef GM_NO_PART
            if (_action != Action::Refresh) menu.prevLen = -1;
#endif
            return false;
        }

#ifndef GM_NO_PART
        if (_action != Action::Refresh && menu.usePart()) {
            if (menu.prevLen >= 0) menu.setPos(menu.cols - (menu.prevLen > width ? menu.prevLen : width));
            menu.prevLen = width;
        }
#endif

        menu.padEnd(width);
        return true;
    }

    // поднять флаг изменения (влияет на wasSet())
    void change() {
        _changed = true;
    }

#ifndef GM_NO_PAGES
    // получить новый id для Page
    uint8_t nextId() {
        return _nextPageId++;
    }
#endif

    // низкоуровневый API меню для своих виджетов
    Menu& menu;

    // MARK: private
   private:
    const void* _targetVar = nullptr;
    Action _action;
    int8_t _targetRow;  // -1: нет привязки к строке
#ifndef GM_NO_PAGES
    uint8_t _nextPageId;
    bool _achieved = false;
#endif
    bool _refresh = false;
    bool _changed = false;

    // ограничить значение диапазоном с циклическим переходом
    static int _clampRing(int v, int minv, int maxv) {
        return v < minv ? maxv : (v > maxv ? minv : v);
    }

    // относится ли текущий проход к зарегистрированному виджету
    bool _reachable() {
        if (_action == Action::None) return false;
        if (_action == Action::Refresh) return menu.isVisible();
        return menu.isChosen();
    }

    // преобразовать действие в изменение значения и позиции редактирования
    void _editDir(int8_t& dir, int8_t& curdir) {
        switch (_action) {
            case Action::Set:
            case Action::Right: curdir = 1; break;
            case Action::Left: curdir = -1; break;
            case Action::SetUp: dir = 1; break;
            case Action::SetDown: dir = -1; break;
            default: break;
        }
    }

    // переместить внутреннюю позицию редактирования
    void _moveActive(int8_t dir, uint8_t max) {
        bool editing = menu.active;
        if (dir > 0) {
            menu.active = menu.active >= max ? 0 : menu.active + 1;
        } else if (dir < 0 && menu.active) {
            menu.active = menu.active <= 1 ? 0 : menu.active - 1;
        }
        if (editing != (bool)menu.active) menu.resetPart();
    }

    // сбросить состояние перед новым проходом билдера
    void _init() {
        _refresh = _changed = false;
#ifndef GM_NO_PAGES
        _achieved = menu.openRoot();
        _nextPageId = 1;
#else
        menu.openRoot();
#endif
        _targetVar = nullptr;
        _targetRow = -1;
    }

    // выполнить пользовательский билдер и проверить состояние навигации
    void _build(BuildCb cb) {
        cb(*this);

        ++menu.len;

#ifndef GM_NO_PAGES
        if (!_achieved) {
            menu.home();
            _refresh = true;
        }
#endif

        if (menu.checkOverflow()) {
            _refresh = true;
        }
    }

    // вывести целочисленное значение с единицей измерения
    template <typename T>
    void _printVar(const T* var, uint8_t base, const char* unit) {
        char buf[GM_INT_BUFFER];
        uint8_t len = sbuild::addInt(*var, base, buf);
        _printNumRow(buf, len, unit);
    }

    // вывести float значение с единицей измерения
    void _printVar(const float* var, uint8_t dec, const char* unit) {
        char buf[GM_FLT_BUFFER];
        uint8_t len = sbuild::addFloat(*var, dec, buf);
        _printNumRow(buf, len, unit);
    }

    // вывести готовое число с единицей измерения справа
    void _printNumRow(const char* buf, uint8_t len, const char* unit) {
        uint8_t ulen = strlen(unit);
        if (!prepareRight(len + ulen)) return;
        menu.print(buf, len);
        menu.print(unit, ulen);
    }

// MARK: _editNum
#ifdef ARDUINO
    template <typename T>
    bool _editNum(const __FlashStringHelper* label, T* var, T minv, T maxv, T step, uint8_t dec_base, const __FlashStringHelper* unit, void (*cb)(T v) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        return _editNum<T>(label_s, var, minv, maxv, step, dec_base, unit_s, cb);
    }
#endif

    template <typename T>
    bool _editNum(const char* label, T* var, T minv, T maxv, T step, uint8_t dec_base, const char* unit = "", void (*cb)(T v) = nullptr) {
        if (!beginWidget()) return false;
        bool changed = false;
        bool render = false;

        switch (getAction()) {
            case Action::Refresh:
                render = true;
                break;

            case Action::Set:
                menu.toggle();
                render = true;
                break;

            case Action::SetUp:
            case Action::Right:
                if (step > 0 && *var < maxv) {
                    T delta = maxv - *var;
                    *var = (delta < step) ? maxv : *var + step;
                    render = changed = true;
                    change();
                }
                break;

            case Action::SetDown:
            case Action::Left:
                if (step > 0 && *var > minv) {
                    T delta = *var - minv;
                    *var = (delta < step) ? minv : *var - step;
                    render = changed = true;
                    change();
                }
                break;

            default: break;
        }

        if (changed && cb) cb(*var);
        if (render && beginRender(var, label)) _printVar(var, dec_base, unit);

        return changed;
    }

    // MARK: _editStr
    bool _editStr(const char* label, char* var, uint8_t maxLen, const char* alphabet, void (*cb)(const char* str)) {
        if (!beginWidget()) return false;
        if (!maxLen || !alphabet) return false;

        uint8_t len = 0;
        while (len < maxLen && var[len]) ++len;
        var[len] = '\0';

        bool changed = false;
        int8_t dir = 0;
        int8_t curdir = 0;

        _editDir(dir, curdir);

        // active:
        // 0       - редактирование выключено
        // 1..len  - реальные символы
        // len + 1 - техническая позиция справа

        _moveActive(curdir, len + 1);

        // change
        if (dir && menu.active) {
            uint8_t pos = menu.active - 1;

            if (pos < len) {
                char next = _inputNext(var[pos], dir, alphabet);

                if (next != var[pos]) {
                    var[pos] = next;
                    changed = true;
                }
            } else {
                if (dir > 0) {
                    if (len < maxLen) {
                        char c = GM_PGM_READ(alphabet);

                        if (c) {
                            var[len++] = c;
                            var[len] = '\0';
                            changed = true;
                        }
                    }
                } else {
                    if (len) {
                        var[--len] = '\0';
                        menu.active = len + 1;
                        changed = true;
                    }
                }
            }

            if (changed) {
                change();
                if (cb) cb(var);
            }
        }

        // ================= RENDER =================
        if (beginRender(var, label)) {
            uint8_t width = len + (menu.isActive() ? 1 : 0);  // + marker

            if (prepareRight(width)) {
                if (menu.isActive()) {
                    uint8_t pos = menu.active - 1;

                    for (uint8_t i = 0; i < len; i++) {
                        if (i == pos) menu.print(menu.marker);
                        menu.print(var[i]);
                    }

                    if (pos == len) {
                        menu.print(menu.marker);
                    }

                } else {
                    menu.print(var);
                }
            }
        }

        return changed;
    }

    // получить последний символ алфавита ввода
    static char _inputLast(const char* alphabet) {
        size_t len = GM_PGM_LEN(alphabet);
        return len ? GM_PGM_READ(alphabet + len - 1) : '\0';
    }

    // получить следующий или предыдущий символ алфавита
    static char _inputNext(char current, int8_t dir, const char* alphabet) {
        char first = GM_PGM_READ(alphabet);

        if (!first) {
            return current;
        }

        const char* ptr = GM_PGM_FIND(alphabet, current);

        if (!ptr) {
            return dir > 0 ? first : _inputLast(alphabet);
        }

        if (dir > 0) {
            char next = GM_PGM_READ(ptr + 1);
            return next ? next : first;
        }

        if (ptr != alphabet) {
            return GM_PGM_READ(ptr - 1);
        }

        return _inputLast(alphabet);
    }
};

}  // namespace gm
