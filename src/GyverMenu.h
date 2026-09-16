#pragma once
#include "./core/Builder.h"

class GyverMenu {
   public:
    // столбцов (длина строки), строк
    GyverMenu(uint8_t cols, uint8_t rows) {
        _menu.cols = cols;
        _menu.rows = rows;
    }

    // изменить размер меню и вернуться в корень
    void resize(uint8_t cols, uint8_t rows) {
        _menu.cols = cols;
        _menu.rows = rows;
        _menu.home();
    }

    // подключить рендер вида void(const char* str, size_t len). Придёт nullptr после окончания вывода
    void onPrint(gm::Menu::PrintCb cb) {
        _menu.printer = cb;
    }

    // установить позицию вывода void(uint8_t col, uint8_t row)
    void onCursor(gm::Menu::CursorCb cb) {
        _menu.cursor = cb;
    }

    // оформить состояние строки void(uint8_t row, bool selected, bool editing)
    void onState(gm::Menu::StateCb cb) {
        _menu.state = cb;
    }

    // подключить билдер вида void(gm::Builder& b)
    void onBuild(gm::Builder::BuildCb cb) {
        _build_cb = cb;
    }

    // размер маркера, сдвигает меню вправо (умолч. 1)
    void setMarkerSize(bool size) {
        _menu.markerSize = size;
    }

    // установить текст кнопки "назад"
    void setBackSign(const char* sign) {
        _menu.backSign = sign;
    }

    // обновить строку с переменной
    void update(const void* var) {
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

    // установить режим обновления
    void setRefreshMode(gm::RefreshMode mode) {
        _menu.updateMode = mode;
        _menu.resetPart();
    }

    // обновлять экран полностью
    void setRefreshFull() {
        setRefreshMode(gm::RefreshMode::Full);
    }

    // обновлять изменённые строки целиком
    void setRefreshRow() {
        setRefreshMode(gm::RefreshMode::Row);
    }

    // обновлять только изменяемую часть, если это возможно
    void setRefreshPart() {
        setRefreshMode(gm::RefreshMode::Part);
    }

    // получить текущий номер виджета
    uint8_t getWidgetIndex() {
        return _menu.getWidgetIndex();
    }

    // находится ли текущий виджет в режиме редактирования
    bool isEditing() const {
        return _menu.active;
    }

    // выйти из режима редактирования и обновить экран
    void cancelEdit() {
        if (!_menu.active || !_build_cb) return;
        _menu.active = 0;

        if (_menu.useFull()) {
            refresh();
        } else {
            gm::Builder b(_menu);
            b.buildRow(_build_cb, _menu.row());
            _menu.endRender();
        }
    }

    // установить символ внутреннего маркера (умолч. '>')
    void setMarker(char symb) {
        _menu.marker = symb;
    }

    // получить символ внутреннего маркера
    char getMarker() const {
        return _menu.marker;
    }

    // обновлять экран полностью, например для вывода в консоль (умолч. false)
    void setFullRefresh(bool full) __attribute__((deprecated("Use setRefreshMode() instead."))) {
        setRefreshMode(full ? gm::RefreshMode::Full : gm::RefreshMode::Row);
    }

    // включить быстрый курсор - рендерить только курсор при смене строки (умолч. true)
    void setFastCursor(bool fast) __attribute__((deprecated("Use setRefreshMode() instead."))) {
        setRefreshMode(fast ? gm::RefreshMode::Part : gm::RefreshMode::Row);
    }

    // MARK: private
   private:
    gm::Menu _menu;
    gm::Builder::BuildCb _build_cb = nullptr;

    // обработать действие текущего виджета
    void _action(gm::Builder::Action action) {
        if (!_build_cb) return;

        gm::Builder b(_menu);
        b.buildAction(_build_cb, action, _menu.useFull());
        if (b.isRefresh()) b.buildRefresh(_build_cb);  // + end render
        else _menu.endRender();
    }

    // переместить выбранную строку и обновить экран по текущему режиму
    void _scroll(bool up) {
        if (!_build_cb) return;

        uint8_t prow = _menu.row();

        if (_menu.move(up)) {
            refresh();
        } else if (prow != _menu.row()) {
            switch (_menu.updateMode) {
                case gm::RefreshMode::Part:
                    _menu.setCursor(prow, false);
                    _menu.setCursor(_menu.row(), true);
                    break;

                case gm::RefreshMode::Row: {
                    gm::Builder b(_menu);
                    b.buildRow(_build_cb, prow);
                    b.buildRow(_build_cb, _menu.row());
                } break;

                case gm::RefreshMode::Full:
                    refresh();
                    return;
            }

            _menu.endRender();
        }
    }
};