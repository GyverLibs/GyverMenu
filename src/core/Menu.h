#pragma once
#include "./utils.h"

#ifndef GM_MAX_DEPTH
#define GM_MAX_DEPTH 5
#endif

namespace gm {

enum class RefreshMode : uint8_t {
    Part,
    Row,
    Full,
};

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

        if (len > left) len = left;
        if (letters > left) letters = left;

        printer(str, len);
        left -= letters;
    }

    // печатать в onPrint
    void print(const char* str) {
        uint8_t letters;
        uint8_t len = gmutil::strlenU(str, &letters);
        print(str, len, letters);
    }

    // вывести число двумя цифрами с ведущим нулём
    void printDec2z(uint8_t v) {
        if (v >= 100) v = 0;
        uint8_t d = v / 10;
        print('0' + d);
        print('0' + (v - d * 10));
    }

    // вывести число четырьмя цифрами с ведущими нулями
    void printDec4z(uint16_t v) {
        if (v >= 10000) v = 0;
        uint16_t d = v / 100;
        printDec2z(d);
        printDec2z(v - d * 100);
    }

    // заполнить пробелами от начала
    void pad(int8_t n) {
        if (n < 0) return;
        if (n > left) n = left;
        left -= n;
        char space = ' ';
        while (n--) printer(&space, 1);
    }

    // заполнить пробелами от конца
    void padEnd(int8_t n = 0) {
        pad(left - n);
    }

    // переключить активное состояние (isActive)
    void toggle() {
        active = !active;
        resetPart();
    }

    // текущая строка меню
    uint8_t currentRow() {
        return len - scroll();
    }

    // виджет виден и активен (кнопкой set)
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

    // получить текущий номер виджета
    uint8_t getWidgetIndex() {
        return len;
    }

    // =====================================================
    // ==================== PRIVATE API ====================
    // =====================================================
    // MARK: private
    typedef void (*PrintCb)(const char* str, size_t len);
    typedef void (*CursorCb)(uint8_t col, uint8_t row);
    typedef void (*StateCb)(uint8_t row, bool selected, bool editing);

    PrintCb printer = gmutil::_dummyRender;
    CursorCb cursor = nullptr;
    StateCb state = nullptr;

    const char* backSign = "Back";
    RefreshMode updateMode = RefreshMode::Row;
    char marker = '>';
    int8_t left = 0;
    uint8_t cols, rows;
    uint8_t len = 0;
    uint8_t active = 0;
    bool markerSize = true;
#ifndef GM_NO_PART
    int8_t prevLen = -1;
#endif

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

        // текущая открытая страница
        Page& current() {
            return stack[len - 1];
        }

        // открыта корневая страница
        bool isRoot() {
            return len == 1;
        }

        // войти в страницу при проходе билдера
        bool enter(uint8_t id) {
            if (depth >= len) return false;
            if (stack[depth].id != id) return false;
            ++depth;
            if (depth == len) target = true;
            return true;
        }

        // выйти из страницы после прохода билдера
        void leave() {
            --depth;
            target = false;
        }

        // открыть новую страницу и добавить её в стек
        bool open(uint8_t id) {
            if (len >= maxDepth) return false;
            stack[len++] = Page{id, 0, 0};
            return true;
        }

        // закрыть текущую страницу
        void close() {
            if (len > 1) --len;
        }

        // сбросить стек к корневой странице
        void home() {
            len = 1;
            stack[0] = Page{0, 0, 0};
        }

        // начать проход билдера с корневой страницы
        void openRoot() {
            target = false;
            depth = 0;
            enter(0);
        }
    };
    PageStack<GM_MAX_DEPTH> pages;

    // ссылка на scroll текущей страницы
    uint8_t& scroll() {
        return pages.current().scroll;
    }

    // ссылка на выбранную строку текущей страницы
    uint8_t& row() {
        return pages.current().row;
    }

    // зарегистрировать виджет текущей страницы
    bool registerWidget() {
        if (!pages.target) return false;
        ++len;
        return true;
    }

    // вернуться в корень и выйти из режима редактирования
    void home() {
        pages.home();
        active = 0;
        resetPart();
    }

    // подготовить корневую страницу к проходу билдера
    bool openRoot() {
        pages.openRoot();
        len = -1;
        return pages.isRoot();
    }

    // закрыть текущую страницу и выйти из режима редактирования
    void back() {
        pages.close();
        active = 0;
        resetPart();
    }
#else
    uint8_t _scroll = 0, _row = 0;

    // ссылка на текущий scroll
    uint8_t& scroll() {
        return _scroll;
    }

    // ссылка на текущую выбранную строку
    uint8_t& row() {
        return _row;
    }

    // зарегистрировать очередной виджет
    bool registerWidget() {
        ++len;
        return true;
    }

    // сбросить навигацию и выйти из режима редактирования
    void home() {
        _scroll = _row = 0;
        active = 0;
        resetPart();
    }

    // подготовить проход билдера
    bool openRoot() {
        len = -1;
        return true;
    }

    // выйти из режима редактирования
    void back() {
        active = 0;
        resetPart();
    }
#endif

    // оформить строку и установить позицию в начало контента
    void setCursor(uint8_t row, bool selected, bool editing = false) {
        if (state) {
            setPos(0, row);
            state(row, selected, editing);
            if (!markerSize) return;
        }
        setPos(markerSize, row);
    }

    // оформить текущую строку с актуальным selected/editing
    void beginRow() {
        setCursor(currentRow(), isChosen(), isActive());
    }

    // установить позицию вывода и пересчитать оставшееся место
    bool setPos(uint8_t col, uint8_t row) {
        if (row >= rows) return false;
        if (col > cols) col = cols;

        left = cols - col;
        if (!cursor) return false;

        cursor(col, row);
        return true;
    }

    // установить позицию в текущей строке
    bool setPos(uint8_t col) {
        return setPos(col, currentRow());
    }

    // доступен ли частичный рендер
    bool usePart() {
        return updateMode == RefreshMode::Part && cursor;
    }

    // сбросить контекст частичного рендера
    void resetPart() {
#ifndef GM_NO_PART
        prevLen = -1;
#endif
    }

    // включён ли полный рендер экрана
    bool useFull() {
        return updateMode == RefreshMode::Full;
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
            setCursor(rows - clr, false);
            padEnd();
            --clr;
        }
    }

    // проверка выхода индекса за список (вызов в конце билдера)
    bool checkOverflow() {
        if (index() < len) return false;
        scroll() = row() = 0;
        resetPart();
        return true;
    }

    // двигать строку. Вернёт true, если сдвинулся экран
    bool move(bool up) {
        if (up) {
            if (index()) {
                resetPart();
                if (row()) --row();
                else return --scroll(), true;
            }
        } else {
            if (index() != len - 1) {
                resetPart();
                if (row() < rows - 1) ++row();
                else return ++scroll(), true;
            }
        }
        return false;
    }
};

}  // namespace gm