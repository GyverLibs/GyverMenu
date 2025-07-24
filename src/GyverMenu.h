#pragma once
#include "./core/Builder.h"

class GyverMenu {
   public:
    // столбцов (длина строки), строк
    GyverMenu(uint8_t cols, uint8_t rows) {
        _menu.cols = cols;
        _menu.rows = rows;
    }

    // подключить рендер вида void(const char* str, size_t len). Придёт nullptr после окончания вывода
    void onPrint(gm::Menu::PrintCb cb) {
        _menu.printer = cb;
    }

    // подключить установку курсора вида uint8_t(uint8_t row, bool state)
    void onCursor(gm::Menu::CursorCb cb) {
        _menu.cursor = cb;
    }

    // подключить билдер вида void(gm::Builder& b)
    void onBuild(gm::Builder::BuildCb cb) {
        _build_cb = cb;
    }

    // установить текст кнопки "назад"
    void setBackSign(const char* sign) {
        _menu.backSign = sign;
    }

    // обновить строку с переменной
    void update(void* var) {
        if (_build_cb) gm::Builder(_menu).buildUpdate(_build_cb, var);
    }

    // обновить экран
    void refresh() {
        if (_build_cb) gm::Builder(_menu).buildRefresh(_build_cb);
    }

    // на предыдущее меню
    void back() {
        _menu.back();
        refresh();
    }

    // в главное меню
    void home() {
        _menu.home();
        refresh();
    }

    // кнопка выбора
    void set() {
        _action(gm::Builder::Action::Set);
    }

    // кнопка вверх
    void up() {
        _menu.active ? _action(gm::Builder::Action::SetUp) : _scroll(true);
    }

    // кнопка вниз
    void down() {
        _menu.active ? _action(gm::Builder::Action::SetDown) : _scroll(false);
    }

    // уменьшить напрямую
    void left() {
        _action(gm::Builder::Action::Left);
    }

    // увеличить напрямую
    void right() {
        _action(gm::Builder::Action::Right);
    }

    // обновлять экран полностью, например для вывода в консоль (умолч. false)
    void setFullRefresh(bool full) {
        _fullRefresh = full;
    }

    // включить быстрый курсор - рендерить только курсор при смене строки (умолч. true)
    void setFastCursor(bool fast) {
        _fastCursor = fast;
    }

   private:
    gm::Menu _menu;
    gm::Builder::BuildCb _build_cb = nullptr;
    bool _fullRefresh = false;
    bool _fastCursor = true;

    void _action(gm::Builder::Action action) {
        if (!_build_cb) return;

        gm::Builder b(_menu);
        b.buildAction(_build_cb, action, _fullRefresh);
        if (b.isRefresh()) b.buildRefresh(_build_cb);  // + end render
        else _menu.endRender();
    }

    void _scroll(bool up) {
        if (!_build_cb) return;

        uint8_t prow = _menu.row();

        if (_menu.move(up)) {
            refresh();
        } else if (prow != _menu.row()) {
            if (_fullRefresh) {
                refresh();
            } else {
                if (_fastCursor) {
                    _menu.setCursor(prow, false);
                    _menu.setCursor(_menu.row(), true);
                } else {
                    gm::Builder b(_menu);
                    b.buildRow(_build_cb, prow);
                    b.buildRow(_build_cb, _menu.row());
                }
                _menu.endRender();
            }
        }
    }
};