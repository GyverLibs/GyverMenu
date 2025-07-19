#pragma once
#include "./utils.h"

#ifndef GM_MAX_DEPTH
#define GM_MAX_DEPTH 5
#endif

namespace gm {

class Menu {
   public:
    // ==================== PUBLIC API ====================

    // печатать в onPrint
    void print(char c) {
        if (!left) return;

        printer(&c, 1);
        --left;
    }

    // печатать в onPrint
    void print(const char* str, uint8_t len, uint8_t letters = 0) {
        if (!left) return;

        if (!letters) letters = len;
        printer(str, len);
        left -= letters;
        if (left < 0) left = 0;
    }

    // печатать в onPrint
    void print(const char* str) {
        uint8_t letters;
        uint8_t len = mutil::strlenU(str, &letters);
        print(str, len, letters);
    }

    // заполнить пробелами
    void pad(int8_t n) {
        if (!left || n < 0) return;

        if (n > left) n = left;
        char buf[n];
        memset(buf, ' ', n);
        printer(buf, n);
        left -= n;
    }

    // заполнить пробелами до конца
    void pad() {
        pad(left);
    }

    // переключить активное состояние (isActive)
    void toggle() {
        active ^= 1;
    }

    // текущая строка меню
    uint8_t currentRow() {
        return len - scroll();
    }

    // виджет активен (кнопкой set)
    bool isActive() {
        return isChosen() && active;
    }

    // виджет выбран курсором
    bool isChosen() {
        return index() == len;
    }

    // виджет в видимой области экрана
    bool isVisible() {
        return len >= scroll() && len < scroll() + rows;
    }

    // =====================================================
    // ==================== PRIVATE API ====================
    // =====================================================
    typedef void (*PrintCb)(const char* str, size_t len);
    typedef uint8_t (*CursorCb)(uint8_t row, bool state, bool active);

    PrintCb printer = mutil::_dummyRender;
    CursorCb cursor = nullptr;
    const char* backSign = "Back";
    uint8_t cols, rows;
    uint8_t len = 0;
    int8_t left = 0;
    bool active = false;

#ifndef GM_NO_PAGES
    struct Page {
        uint8_t id;
        uint8_t scroll;
        uint8_t row;
    };

    template <uint8_t maxDepth>
    struct PageStack {
        Page stack[maxDepth] = {Page{0, 0, 0}};
        uint8_t len = 1;
        uint8_t depth = 0;
        bool target = false;

        Page& current() {
            return stack[len - 1];
        }
        bool isRoot() {
            return len == 1;
        }

        bool enter(uint8_t id) {
            if (depth >= len) return false;
            if (stack[depth].id != id) return false;
            ++depth;
            if (depth == len) target = true;
            return true;
        }
        void leave() {
            --depth;
            target = false;
        }

        bool open(uint8_t id) {
            if (len >= maxDepth) return false;
            stack[len++] = Page{id, 0, 0};
            return true;
        }
        void close() {
            if (len > 1) --len;
        }

        void home() {
            len = 1;
            stack[0] = Page{0, 0, 0};
        }

        void openRoot() {
            target = false;
            depth = 0;
            enter(0);
        }
    };
    PageStack<GM_MAX_DEPTH> pages;

    uint8_t& scroll() {
        return pages.current().scroll;
    }
    uint8_t& row() {
        return pages.current().row;
    }
    bool beginWidget() {
        if (!pages.target) return false;
        ++len;
        return true;
    }
    void home() {
        pages.home();
        active = false;
    }
    bool openRoot() {
        pages.openRoot();
        len = -1;
        return pages.isRoot();
    }
    void back() {
        pages.close();
        active = false;
    }
#else
    uint8_t _scroll = 0, _row = 0;

    uint8_t& scroll() {
        return _scroll;
    }
    uint8_t& row() {
        return _row;
    }
    bool beginWidget() {
        ++len;
        return true;
    }
    void home() {
        _scroll = _row = 0;
        active = false;
    }
    bool openRoot() {
        len = -1;
        return true;
    }
    void back() {
        active = false;
    }
#endif

    // установить курсор
    void setCursor(uint8_t row, bool state, bool active = false) {
        left = cols - (cursor ? cursor(row, state, active) : 0);
    }
    void setCursor() {
        setCursor(currentRow(), isChosen(), active);
    }

    // вызвать onPrint(0)
    void endRender() {
        printer(0, 0);
    }

    // порядковый номер текущего виджета в списке
    uint8_t index() {
        return scroll() + row();
    }

    // очистка нижних линий (вызов в конце билдера)
    void clearBottom() {
        uint8_t clr = 0;
        if (len < rows) {
            clr = rows - len;
        }
        if (scroll() > len - rows) {  // <0 ok!
            clr = scroll() - (len - rows);
        }

        while (clr) {
            setCursor(rows - clr, 0);
            pad();
            --clr;
        }
    }

    // проверка выхода индекса за список (вызов в конце билдера)
    bool checkOverflow() {
        if (index() < len) return false;
        scroll() = row() = 0;
        return true;
    }

    // двигать строку. Вернёт true, если сдвинулся экран
    bool move(bool up) {
        if (up) {
            if (index()) {
                if (row()) --row();
                else return --scroll(), true;
            }
        } else {
            if (index() != len - 1) {
                if (row() < rows - 1) ++row();
                else return ++scroll(), true;
            }
        }
        return false;
    }
};

}  // namespace gm