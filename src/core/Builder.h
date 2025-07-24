#pragma once
#include <sbuild.h>
#include <stdlib.h>

#include "Menu.h"

#define GM_INT_BUFFER 22
#define GM_FLT_BUFFER 18
#define GM_NEXT __COUNTER__

namespace gm {

class Builder {
   public:
    typedef void (*BuildCb)(gm::Builder&);

    enum class Action {
        None,
        Refresh,
        Set,
        SetDown,
        SetUp,
        Left,
        Right,
    };

    // =================== CTR ===================
    Builder(Menu& menu) : menu(menu) {}

    void buildRefresh(BuildCb cb) {
        buildRow(cb, -1);
        if (isRefresh()) buildRow(cb, -1);
        if (isRefresh()) buildRow(cb, -1);
        menu.endRender();
    }

    void buildRow(BuildCb cb, int8_t targetRow) {
        _init();
        _action = Action::Refresh;
        _targetRow = targetRow;
        _build(cb);
        if (targetRow == -1) menu.clearBottom();
    }

    void buildUpdate(BuildCb cb, void* targetVar) {
        _init();
        _action = Action::Refresh;
        _targetVar = targetVar;
        _build(cb);
        if (_refresh) menu.endRender();
    }

    void buildAction(BuildCb cb, Action action, bool skipRender) {
        _init();
        _action = action;
        _refresh = skipRender;
        _build(cb);
    }

#ifndef GM_NO_PAGES
// =================== PAGE ===================
#ifdef ARDUINO
    bool PageBegin(uint8_t id, const __FlashStringHelper* label) {
        GM_READ_PGM(label, label_s);
        return PageBegin(id, label_s);
    }
#endif

    // начать страницу (подменю)
    bool PageBegin(uint8_t id, const char* label) {
        if (beginWidget()) {
            switch (getAction()) {
                case Action::Refresh:
                    if (beginRender()) {
                        menu.print(label);
                        menu.pad(menu.left - 1);
                        menu.print('>');
                    }
                    break;

                case Action::Set:
                case Action::Right:
                    menu.pages.open(id);
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
        if (back && beginWidget()) {
            switch (getAction()) {
                case Action::Refresh:
                    if (beginRender()) {
                        menu.print(menu.backSign);
                        menu.pad();
                    }
                    break;

                case Action::Set:
                case Action::Left:
                    menu.pages.close();
                    refresh();
                    break;

                default: break;
            }
        }
        if (menu.pages.target) _achieved = true;
        menu.pages.leave();
    }

#ifdef ARDUINO
    void Page(uint8_t id, const __FlashStringHelper* label, void (*page)(Builder& b), bool back = true) {
        GM_READ_PGM(label, label_s);
        Page(id, label_s, page, back);
    }
#endif

    void Page(uint8_t id, const char* label, void (*page)(Builder& b), bool back = true) {
        if (PageBegin(id, label)) {
            page(*this);
            PageEnd(back);
        }
    }
#endif

    // =================== BUTTON ===================
#ifdef ARDUINO
    bool Button(const __FlashStringHelper* label, void (*cb)() = nullptr) {
        GM_READ_PGM(label, label_s);
        return Button(label_s, cb);
    }
#endif

    bool Button(const char* label, void (*cb)() = nullptr) {
        if (!beginWidget()) return false;

        switch (getAction()) {
            case Action::Refresh:
                if (beginRender()) {
                    menu.print(label);
                    menu.pad();
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

    // =================== LINE ===================
#ifdef ARDUINO
    void Label(const __FlashStringHelper* line) {
        GM_READ_PGM(line, line_s);
        Label(line_s);
    }
#endif

    void Label(const char* line) {
        if (!beginWidget()) return;

        if (getAction() == Action::Refresh && beginRender()) {
            menu.print(line);
            menu.pad();
        }
    }

    // =================== LABEL ===================
#ifdef ARDUINO
    void ValueStr(const __FlashStringHelper* label, const char* value) {
        GM_READ_PGM(label, label_s);
        ValueStr(label_s, value);
    }
#endif

    void ValueStr(const char* label, const char* var) {
        if (!beginWidget()) return;

        if (getAction() == Action::Refresh && beginRender()) {
            uint8_t letters;
            uint8_t len = mutil::strlenU(var, &letters);
            menu.print(label);
            menu.pad(menu.left - letters);
            menu.print(var, len, letters);
        }
    }

// =================== SWITCH ===================
#ifdef ARDUINO
    bool Switch(const __FlashStringHelper* label, bool* var, void (*cb)(bool v) = nullptr) {
        GM_READ_PGM(label, label_s);
        return Switch(label_s, var, cb);
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

        if (render && beginRender(var)) {
            menu.print(label);
            menu.pad(menu.left - 3);
            menu.print(*var ? "[x]" : "[ ]", 3);
        }

        return changed;
    }

// =================== SELECT ===================
#ifdef ARDUINO
    bool Select(const __FlashStringHelper* label, uint8_t* var, const __FlashStringHelper* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(opts, opts_s);
        return Select(label_s, var, opts_s, cb);
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
                if (*var < mutil::countSub(opts, ';') - 1) {
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

        if (changed && cb) {
            const char* start;
            uint8_t slen = mutil::getSub(opts, ';', *var, &start);
            cb(*var, start, slen);
        }

        if (render && beginRender(var)) {
            const char* start;
            uint8_t len = mutil::getSub(opts, ';', *var, &start);
            uint8_t letters;
            mutil::strlenU(start, &letters, len);

            menu.print(label);
            if (menu.isActive()) menu.print(':');
            menu.pad(menu.left - (letters + 2));
            menu.print('<');
            menu.print(start, len, letters);
            menu.print('>');
        }

        return changed;
    }

// =================== TABS ===================
#ifdef ARDUINO
    bool Tabs(uint8_t* var, const __FlashStringHelper* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr) {
        GM_READ_PGM(tabs, tabs_s);
        return Tabs(var, tabs_s, cb);
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
                if (*var < mutil::countSub(tabs, ';') - 1) {
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

        if (changed && cb) {
            const char* start;
            uint8_t slen = mutil::getSub(tabs, ';', *var, &start);
            cb(*var, start, slen);
        }

        if (render && beginRender(var)) {
            const char* start;
            uint8_t slen = mutil::getSub(tabs, ';', *var, &start);
            int16_t start_i = (menu.left / 2) - ((start - tabs) + slen / 2);
            int16_t len = strlen(tabs);
            int8_t i = 0;
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

// =================== VALUE_INT ===================
#ifdef ARDUINO
    template <typename T>
    bool ValueInt(const __FlashStringHelper* label, T* var, T minv, T maxv, T step, uint8_t base, const __FlashStringHelper* unit, void (*cb)(T v) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        return _value(label_s, var, minv, maxv, step, base, unit_s, cb);
    }
#endif

    template <typename T>
    bool ValueInt(const char* label, T* var, T minv, T maxv, T step, uint8_t base = 10, const char* unit = "", void (*cb)(T v) = nullptr) {
        return _value(label, var, minv, maxv, step, base, unit, cb);
    }

// =================== VALUE_FLOAT ===================
#ifdef ARDUINO
    bool ValueFloat(const __FlashStringHelper* label, float* var, float minv, float maxv, float step, uint8_t dec, const __FlashStringHelper* unit, void (*cb)(float v) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        return _value(label_s, var, minv, maxv, step, dec, unit_s, cb);
    }
#endif

    bool ValueFloat(const char* label, float* var, float minv, float maxv, float step, uint8_t dec = 2, const char* unit = "", void (*cb)(float v) = nullptr) {
        return _value(label, var, minv, maxv, step, dec, unit, cb);
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

    // начать виджет. true если разрешено
    inline bool beginWidget() {
        return menu.beginWidget();
    }

    // начать вывод виджета. true если разрешено
    // targetVar - указатель на переменную виджета
    // wCursor - рисовать ли курсор
    bool beginRender(void* targetVar = nullptr, bool wCursor = true) {
        if (_action != Action::Refresh && _refresh) return false;
        if (_targetRow >= 0 && _targetRow != menu.row()) return false;
        if (_targetVar) {
            if (_targetVar != targetVar || !menu.isVisible()) return false;
            _refresh = true;
        }
        if (wCursor) menu.setCursor();
        return true;
    }

    // получить действие виджета
    Action getAction() {
        switch (_action) {
            case Action::Set:
            case Action::SetUp:
            case Action::SetDown:
            case Action::Left:
            case Action::Right:
                if (menu.isChosen()) return _action;
                break;

            case Action::Refresh:
                if (menu.isVisible()) return _action;
                break;

            default: break;
        }
        return Action::None;
    }

    // поднять флаг изменения (влияет на wasSet())
    void change() {
        _changed = true;
    }

    Menu& menu;

   private:
    void* _targetVar = nullptr;
    Action _action;
    int8_t _targetRow;
    bool _refresh = false;
    bool _changed = false;
    bool _achieved = false;

    void _init() {
        _refresh = _changed = false;
        _achieved = menu.openRoot();
        _targetVar = nullptr;
        _targetRow = -1;
    }
    void _build(BuildCb cb) {
        cb(*this);

        ++menu.len;

        if (!_achieved) {
            menu.home();
            _refresh = true;
        }

        if (menu.checkOverflow()) {
            _refresh = true;
        }
    }

    // =================== VALUE ===================
#ifdef ARDUINO
    template <typename T>
    bool _value(const __FlashStringHelper* label, T* var, T minv, T maxv, T step, uint8_t dec, const __FlashStringHelper* unit, void (*cb)(T v) = nullptr) {
        GM_READ_PGM(label, label_s);
        GM_READ_PGM(unit, unit_s);
        return _value<T>(label_s, var, minv, maxv, step, dec, unit, cb);
    }
#endif

    template <typename T>
    bool _value(const char* label, T* var, T minv, T maxv, T step, uint8_t dec, const char* unit = "", void (*cb)(T v) = nullptr) {
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
                if (*var < maxv) {
                    *var += step;
                    if (*var > maxv) *var = maxv;
                    render = changed = true;
                    change();
                }
                break;

            case Action::SetDown:
            case Action::Left:
                if (*var > minv) {
                    *var -= step;
                    if (*var < minv) *var = minv;
                    render = changed = true;
                    change();
                }
                break;

            default: break;
        }

        if (changed && cb) cb(*var);
        if (render && beginRender(var)) {
            _printVar(var, dec, label, unit);
        }

        return changed;
    }

    template <typename T>
    void _printVar(T* var, uint8_t base, const char* label, const char* unit) {
        char buf[GM_INT_BUFFER];
        // ltoa(*var, buf, base);
        uint8_t len = sbuild::addInt(*var, base, buf);
        _printNumRow(label, buf, len, unit);
    }
    void _printVar(float* var, uint8_t dec, const char* label, const char* unit) {
        char buf[GM_FLT_BUFFER];
        // dtostrf(*var, dec ? dec + 2 : 1, dec, buf);
        uint8_t len = sbuild::addFloat(*var, dec, buf);
        _printNumRow(label, buf, len, unit);
    }
    void _printNumRow(const char* label, const char* buf, uint8_t len, const char* unit) {
        uint8_t ulen = strlen(unit);
        menu.print(label);
        if (menu.isActive()) menu.print(':');
        menu.pad(menu.left - (len + ulen));
        menu.print(buf, len);
        menu.print(unit, ulen);
    }
};

}  // namespace gm