[![latest](https://img.shields.io/github/v/release/GyverLibs/GyverMenu.svg?color=brightgreen)](https://github.com/GyverLibs/GyverMenu/releases/latest/download/GyverMenu.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/GyverMenu.svg)](https://registry.platformio.org/libraries/gyverlibs/GyverMenu)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/GyverMenu?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# GyverMenu
Динамическая система меню для Arduino
- Сборка меню в "билдере" с возможностью вывода виджетов по условию или в цикле
- Механизм вложенных страниц с памятью навигации
- Удобный API для навигации любым количеством кнопок/джойстиком/энкодером
- Вывод на любой дисплей или в монитор порта
- Выравнивание значений по правому краю с поддержкой кириллицы
- Набор встроенных виджетов
- Создание своих виджетов
- Механизм обновления значений виджетов на экране
- Оптимизация количества перерисовок экрана: полный экран / только строка / только изменяемое
- Из коробки это текстовое меню, но можно делать свои виджеты для графического дисплея

https://github.com/user-attachments/assets/41910bcf-d537-4f18-a999-8ff6d2c00438

> [Демо проект](https://wokwi.com/projects/436903131317665793) в симуляторе

### Совместимость
Совместима со всеми платформами

### Зависимости
- [StringN](https://github.com/GyverLibs/StringN)

## Содержание
- [Описание классов](#api)
- [Использование](#usage)
- [Режимы обновления](#refresh-modes)
- [Свои виджеты](#custom-widgets)
- [Ограничения и особенности](#limits)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="api"></a>

## Описание классов
### GyverMenu
```cpp
// столбцов (длина строки), строк
GyverMenu(uint8_t cols, uint8_t rows);

// изменить размер меню и вернуться в корень
void resize(uint8_t cols, uint8_t rows);

// подключить вывод текста void(const char* str, size_t len)
// после окончания отрисовки придёт nullptr
void onPrint(gm::Menu::PrintCb cb);

// подключить установку позиции void(uint8_t col, uint8_t row)
void onCursor(gm::Menu::CursorCb cb);

// оформить состояние строки void(uint8_t row, bool selected, bool editing)
void onState(gm::Menu::StateCb cb);

// подключить билдер void(gm::Builder& b)
void onBuild(gm::Builder::BuildCb cb);

// размер маркера, сдвигает меню вправо (умолч. 1)
void setMarkerSize(bool offset);

// символ внутреннего маркера (умолч. '>')
void setMarker(char symb);
char getMarker();

// установить текст кнопки "назад"
void setBackSign(const char* sign);

// обновить виджет, связанный с переменной
void update(const void* var);

// обновить весь экран
void refresh();

// на предыдущее меню
void back();

// в главное меню
void home();

// кнопки управления
void set();
void up();
void down();
void left();
void right();

// режим обновления
void setRefreshFull();  // весь экран
void setRefreshRow();   // вся строка
void setRefreshPart();  // частичное обновление

// получить текущий номер виджета
uint8_t getWidgetIndex();

// режим редактирования
bool isEditing();
void cancelEdit();
```

### gm::Builder
> Под `Str` далее имеются в виду `const char*`, `const __FlashStringHelper*`, `const String&`

#### Страница
```cpp
// начать страницу (подменю)
bool PageBegin(Str label); // автоматический ID
bool PageBegin(uint8_t id, Str label);

// закончить страницу (вызывать внутри условия по PageBegin). back - выводить кнопку "назад"
void PageEnd(bool back = true);

// страница с коллбэком (вместо PageBegin-PageEnd). back - выводить кнопку "назад"
template <typename PageCb>
void Page(Str label, const PageCb& page, bool back = true);
template <typename PageCb>
void Page(uint8_t id, Str label, const PageCb& page, bool back = true);
```

#### Виджеты
```cpp
// кнопка
bool Button(Str label, void (*cb)() = nullptr);

// просто текст
void Label(Str line);

// значения только для отображения
void ValueStr(Str label, const char* var);
void ValueInt(Str label, const T* var, uint8_t base = 10, Str unit = "");
void ValueFloat(Str label, const float* var, uint8_t dec = 2, Str unit = "");

// выключатель
bool Switch(Str label, bool* var, void (*cb)(bool v) = nullptr);

// выбор пункта. opts - строка с разделителем ';'
bool Select(Str label, uint8_t* var, Str opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// выбор пункта в стиле "вкладок". tabs - строка с разделителем ';'
bool Tabs(uint8_t* var, Str tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// редактируемое int значение
template <typename T>
bool EditInt(Str label, T* var, T minv, T maxv, T step, Str unit = "", void (*cb)(T v) = nullptr);

// редактируемое float значение
bool EditFloat(Str label, float* var, float minv, float maxv, float step, uint8_t dec = 2, Str unit = "", void (*cb)(float v) = nullptr);

// редактирование строки из встроенного или пользовательского алфавита
// буфер var должен иметь размер минимум maxLen + 1
bool EditStr(Str label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr);
bool EditStr(Str label, char* var, uint8_t maxLen, Str alphabet, void (*cb)(const char* str) = nullptr);

// редактирование строки по всей таблице ASCII
// буфер var должен иметь размер минимум maxLen + 1
bool EditASCII(Str label, char* var, uint8_t maxLen, void (*cb)(const char* str) = nullptr);

// время, T - структура с полями .second, .minute, .hour или встроенная gm::Time
template <typename T>
bool Time(Str label, T* var, void (*cb)(T v) = nullptr);

// дата, D - структура с полями .day, .month, .year или встроенная gm::Date
template <typename D>
bool Date(Str label, D* var, void (*cb)(D v) = nullptr);

// битовая маска, bits - кол-во младших бит, вывод - MSB слева
template <typename T>
bool Bitmask(Str label, T* var, uint8_t bits, void (*cb)(T v) = nullptr);
```

#### Системное
```cpp
// обновить экран после работы билдера
void refresh();

// будет ли полный refresh после текущего действия
bool isRefresh();

// было действие с каким-то из виджетов выше
bool wasSet();

// сбросить флаг wasSet
void clearSet();
```

#### API для своих виджетов
```cpp
// зарегистрировать виджет без проверки доступности
bool registerWidget();

// зарегистрировать виджет и проверить, относится ли текущий проход к нему
bool beginWidget();

// получить текущее действие
Action getAction();

// начать рендер виджета; с label библиотека обслуживает подпись и Part
bool beginRender(const void* targetVar = nullptr, const char* label = nullptr);

// выровнять дальнейший вывод вправо на width символов, при нехватке места вывести "ovf"
bool prepareRight(uint8_t width);

// поднять флаг изменения (влияет на wasSet())
void change();

// получить новый id для Page
uint8_t nextId();

// объект низкоуровневого меню доступен как b.menu
Menu& menu;
```

Основные методы `b.menu` для своих виджетов:
```cpp
// печать
void menu.print(char c);
void menu.print(const char* str);
void menu.print(const char* str, uint8_t len, uint8_t letters = 0);
void menu.printDec2z(uint8_t v);   // 2 цифры с ведущим нулём
void menu.printDec4z(uint16_t v);  // 4 цифры с ведущими нулями

// заполнение пробелами
void menu.pad(int8_t n);
void menu.padEnd(int8_t n = 0);

// установить позицию вывода. Нужен onCursor()
bool menu.setPos(uint8_t col);
bool menu.setPos(uint8_t col, uint8_t row);

// состояние виджета
void menu.toggle();
uint8_t menu.currentRow();
bool menu.isActive();
bool menu.isChosen();
bool menu.isVisible();
```

### Дефайны настроек
Объявляется перед подключением библиотеки

```cpp
#define GM_MAX_DEPTH 5  // макс. вложенность меню (умолч. 5)
#define GM_NO_PAGES     // отключить вложенные меню (облегчает библиотеку)
#define GM_NO_PART      // отключить частичный вывод значений в Part (облегчает библиотеку)
```

<a id="usage"></a>

## Использование
### Минимальный пример
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
    // вызывайте menu.up/down/set/left/right по событиям своих кнопок или энкодера
}
```

### Как это работает
Библиотека хранит только состояние навигации, а само меню является виртуальным и каждый раз описывается в билдере. Билдер может выполняться несколько раз для разных задач: обработать действие, найти нужный виджет, перерисовать строку или весь экран. Поэтому набор и порядок виджетов в одном состоянии меню должен быть воспроизводимым, а тяжёлую работу внутри билдера лучше не выполнять.

В качестве точек входа библиотека имеет 5 виртуальных кнопок, которые можно связать с любыми физическими кнопками, джойстиком или энкодером. В качестве точек выхода используются обработчики:
- `onBuild(gm::Builder& b)` - построение меню
- `onPrint(const char* str, size_t len)` - вывод текста
- `onCursor(uint8_t col, uint8_t row)` - установка позиции вывода
- `onState(uint8_t row, bool selected, bool editing)` - оформление состояния строки

Библиотека не привязана к конкретному дисплею и способу ввода. Текстовое меню можно вывести на LCD, OLED, терминал или другой интерфейс, а для графического дисплея можно написать свои виджеты.

### Кнопки
Библиотека предусматривает **виртуальных** 5 кнопок управления + отдельные команды "вернуться на предыдущее меню" и "на главный экран", но на практике это может быть любое другое количество кнопок - хоть одна, хоть энкодер. Логика работы такая:

- `menu.set()` - центральная кнопка выбора. Она нажимает виджет `Button` на экране, переключает состояние виджета `Switch`, обрабатывает нажатие по кнопке возврата в предыдущее меню, а для остальных виджетов - переводит их в режим изменения
- `menu.up()` - переместить курсор на предыдущий пункт. Если активен режим изменения - вместо этого будет вызвано увеличение значения
- `menu.down()` - переместить курсор на следующий пункт. Если активен режим изменения - вместо этого будет вызвано уменьшение значения
- `menu.left()` - напрямую уменьшает значение текущего виджета независимо от режима изменения. Также нажимает кнопку "назад" для возврата в  предыдущее меню
- `menu.right()` - напрямую увеличивает значение текущего виджета независимо от режима изменения. Также нажимает кнопку входа в подменю и виджет `Button`
- `menu.back()` - возвращает на предыдущее меню
- `menu.home()` - возвращает в главное меню
- `menu.isEditing()` - проверить, находится ли текущий виджет в режиме редактирования
- `menu.cancelEdit()` - выйти из редактирования. В `Full` обновится весь экран, в `Row/Part` - текущая строка

Таким образом, сценарии навигации могут быть:

- 5 кнопок или джойстик с центральной кнопкой
- 4 кнопки (все кроме центральной) или джойстик без центральной кнопки
- 3 кнопки: `верх` + `центр` + `низ`
- 2 кнопки: `верх` + `низ` +
  - Одновременное нажатие вызывает `set`
  - Удержание `вниз` вызывает `set`
  - Удержание `вверх` вызывает `back`
- Энкодер: `up` и `down` на вращение, `set` на клик
- Энкодер: `up` и `down` на вращение, `set` на клик, зажатый поворот как `left` и `right`
  - Удержание кнопки - `back`. Очень долгое удержание - `home`

В общем можно придумать очень много вариантов, используйте [EncButton](https://github.com/GyverLibs/EncButton) для обработки кнопок и энкодеров и вдохновляйтесь китайскими цифровыми паяльниками.

### Обработчики
Для обычного текстового меню используются четыре обработчика. Обязательным является только билдер. `onCursor` нужен для адресуемого позиционирования и режима `Part`, `onState` необязателен и используется только для оформления состояния строки.

#### Билдер
```cpp
void builder(gm::Builder& b) {
    // b - объект для вызова виджетов
}
menu.onBuild(builder);
```

#### Печать
```cpp
void printer(const char* str, size_t len) {
    // str - строка для вывода
    // str == nullptr - текущая отрисовка закончена, можно обновить дисплей или перенести строку в консоли
    // len - длина строки в байтах
}
menu.onPrint(printer);
```

#### Курсор
`onCursor` устанавливает позицию вывода и нужен для режима `Part`:
```cpp
void cursor(uint8_t col, uint8_t row) {
    // установить позицию вывода
}
menu.onCursor(cursor);
```

#### Состояние
`onState` вызывается перед отрисовкой строки и отдельно при перемещении по строкам:
```cpp
void state(uint8_t row, bool selected, bool editing) {
    // row - строка дисплея
    // selected - строка выбрана
    // editing - выбранный виджет находится в режиме изменения
}
menu.onState(state);
```

Перед `onState` библиотека устанавливает позицию в начало строки через `onCursor`, если он подключён. `row` остаётся в `onState`, чтобы обработчик мог работать независимо, например в Serial или графическом renderer. Если оформление состояния занимает один столбец слева, оставьте `setMarkerSize(true)` (умолчание), если не занимает - `setMarkerSize(false)`.

Геометрия вывода при этом остаётся в `onCursor`.

Пример для LCD с курсором `>`:
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

Для OLED можно не занимать отдельный столбец, а например инвертировать выбранную строку:

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

### Режимы обновления
Режим задаётся через:
```cpp
menu.setRefreshMode(gm::RefreshMode::Part);
```

или короткими функциями:
```cpp
menu.setRefreshPart();
menu.setRefreshRow();
menu.setRefreshFull();
```

- `Part` - минимальная перерисовка. При перемещении по видимым строкам меняется только оформление курсора, а при повторных изменениях выбранного виджета встроенные виджеты обновляют только область значения. Первое изменение после навигации, вход и выход из редактирования выводят строку целиком. Требует `onCursor`. Частичный вывод значений можно отключить через `GM_NO_PART` для уменьшения flash, быстрое обновление маркера при навигации при этом сохраняется
- `Row` - при перемещении перерисовываются старая и новая строки, значение виджета также обновляется в строке. Это режим по умолчанию
- `Full` - после действия перерисовывается весь экран. Подходит для терминала, framebuffer или рендера, где частичное обновление не имеет смысла

Обычно:
- LCD с адресуемым курсором - `Part`
- простой OLED текстом - `Part` или `Row`
- framebuffer/терминал/рендер кадром целиком - `Full`

`menu.update(&var)` запускает поиск виджета, связанного с этой переменной, и обновляет только его. Обновление через `update()` выводит виджет обычным способом без использования предыдущего Part-контекста.

Если меняется структура меню - например по `Switch` появляются или исчезают другие виджеты - вызовите `b.refresh()` из билдера после изменения:
```cpp
if (b.Switch("Extra", &showExtra)) b.refresh();
if (showExtra) b.Button("Extra button");
```

### Билдер и виджеты
Билдер - обработчик, который вызывается библиотекой для:

- Отрисовки текущего выбранного виджета
- Отрисовки всего экрана
- Поиска и отрисовки виджета для обновления его значения из апдейта

Т.е. билдер может вызываться часто и для разных целей, поэтому нельзя писать в нём какие-то медленные и тяжёлые конструкции.

#### Вывод
Для вывода виджетов нужно просто вызвать их функции в том порядке, в котором они должны располагаться на дисплее, например:

```cpp
menu.onBuild([](gm::Builder& b) {
    b.Button("Button 1");
    b.Button("Button 2");
});
```

Виджеты можно создавать динамически, например 5 кнопок:

```cpp
menu.onBuild([](gm::Builder& b) {
    for (int i = 0; i < 5; i++) {
        b.Button(String16("Button ") + i);
    }
});
```

Виджеты можно выводить по условию:

```cpp
menu.onBuild([](gm::Builder& b) {
    if (foo) {
        b.Button("Button 1");
        b.Button("Button 2");
    }
});
```

#### Действия
Встроенные в библиотеку виджеты имеют три механизма взаимодействия:

- Функция активного виджета возвращает `true` при изменении значения (и клик для виджета кнопки)
- К активным виджетам можно подключить функцию-обработчик, которая будет вызвана при изменении значения (также в неё будет отправлено новое значение)
- К виджетам со значением подключается переменная, которая будет автоматически изменяться библиотекой

```cpp
void clickHandler() {
    Serial.println("Btn 2 click");
}

bool sw;

menu.onBuild([](gm::Builder& b) {
    // условие
    if (b.Button("Button 1")) {
        Serial.println("Btn 1 click");
    }

    // обработчик
    b.Button("Button 2", clickHandler);

    // подключение переменной
    b.Switch("Switch 1", &sw);
    
    // подключение переменной + обработчик
    b.Switch("Switch 2", &sw, [](bool v) { Serial.println(v); });
});
```

По действию с виджета можно обновить меню, например выключатель отвечает за вывод нескольких кнопок. Кнопки активные, обработка клика работает даже при выводе через цикл:

```cpp
bool btns;

menu.onBuild([](gm::Builder& b) {
    if (b.Switch("Buttons", &btns)) b.refresh();    // обновить при клике

    if (btns) {
        for (int i = 0; i < 5; i++) {
            if (b.Button(String16("Button ") + i)) {
                Serial.println(String16("Button") + i);
            }
        }
    }
});
```

<a id="limits"></a>

### Ограничения и особенности
- Если значение не вмещается в дисплей - вместо него будет выведено многоточие
- `EditInt` внутри работает с `int32_t` значением, поэтому нельзя редактировать числа больше ~2.1 миллиона
- Билдер является декларативным и может вызываться несколько раз на одно пользовательское действие. Не выполняйте в нём задержки, сетевые запросы и другую тяжёлую работу
- Порядок виджетов определяет их индексы и навигацию. Если условие меняет набор виджетов, после изменения структуры нужен `b.refresh()`
- `menu.update(ptr)` находит виджет по указателю, переданному в его `beginRender`. Для предсказуемого обновления используйте уникальную переменную для обновляемого виджета
- `Part` требует подключённый `onCursor`. Без него частичное позиционирование невозможно
- `beginRender(var, label)` предполагает, что изменяемая область находится после `label`; первый вывод выполняется целиком, следующие изменения выбранного виджета могут обновлять только область значения
- Для правого выравнивания вызывайте `prepareRight(width)` с актуальной шириной выводимой области. При нехватке места будет выведено `ovf`
- `markerSize` равен одному столбцу по умолчанию. Для renderer без отдельного символа курсора вызовите `setMarkerSize(false)`
- `Time`: структура должна иметь поля `hour`, `minute`, `second`, `Date` - `day`, `month`, `year`
- `Bitmask`: `bits` ограничивается размером типа переменной
- `GM_NO_PAGES` отключает страницы и уменьшает размер библиотеки
- `GM_NO_PART` отключает частичный вывод изменяемой области виджетов. Быстрое обновление маркера при навигации в режиме `Part` сохраняется
- После изменения размеров дисплея `resize(cols, rows)` сбрасывает навигацию в корень

### Ввод текста
Виджет `EditStr` работает следующим образом:
- `EditStr` использует стандартный алфавит библиотеки: 0-9 A-Z a-z символы. Можно передать внешний алфавит
- `EditASCII` "легче" и использует чистую таблицу ASCII, без хранения алфавита
- Буфер должен иметь размер минимум `maxLen + 1` для завершающего `\0`
- Длину строки можно менять только справа - нужно поставить курсор на последнюю позицию, за строку: `foo>`. Если нажать "вниз" - строка укоротится на один символ, если "вверх" - будет добавлен новый символ перед курсором

#### Трюки
У виджетов с настройкой шага изменения шаг можно задавать динамически, например от скорости вращения энкодера. С библиотекой EncButton или uEncoder это может выглядеть так:

```cpp
int vali;

menu.onBuild([](gm::Builder& b) {
    b.EditInt<int>("EditInt", &vali, -100, 100, encb.fast() ? 10 : 1);
});
```

Т.е. при быстром вращении энкодера шаг будет 10 (грубый), а при медленном - 1 (точный). Билдер вызывается на каждом действии изменения значения, поэтому шаг будет выбираться для каждого клика.

<a id="custom-widgets"></a>

### Свои виджеты
Встроенные значения разделены по поведению: `Value*` - только отображение, `Edit*` - редактирование.

Системные виджеты построены на том же API, который доступен пользователю. Минимальный виджет обычно состоит из `beginWidget()`, обработки `getAction()` и `beginRender()`:

- `beginRender(var)` - виджет рисует строку самостоятельно
- `beginRender(var, label)` - библиотека обслуживает label и при возможности использует `Part` для области значения
- `prepareRight(width)` - выравнивает дальнейший вывод вправо и запоминает ширину области для последующего `Part`

Пример простого числового виджета с блоком шириной 2 символа и поддержкой `update(&var)`/`Part`:
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

Использование:
```cpp
uint8_t value;

menu.onBuild([](gm::Builder& b) {
    MyValue(b, "Value", &value);
});

// изменить value из программы и обновить только связанный виджет
value = 42;
menu.update(&value);
```

Для значения переменной длины используется тот же `beginRender(var, label)`, а в `prepareRight()` передаётся актуальная ширина значения:
```cpp
if (b.beginRender(var, label) && b.prepareRight(valueWidth)) {
    b.menu.print(value);
}
```

Для полностью графического виджета используйте `beginRender(var)` без `label` и рисуйте напрямую в свой дисплей. `beginRender()` всё равно вызывает `onState`, поэтому renderer получает актуальные `selected/editing`.

### Примеры
В папке `examples` есть отдельные примеры для:
- LCD и OLED
- Режимов обновления
- Страниц и навигации
- Обновления переменной через `update`
- Собственных текстовых и графических виджетов

<a id="versions"></a>

## Версии
- v1.0

<a id="install"></a>

## Установка
- Библиотеку можно найти по названию **GyverMenu** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/GyverMenu/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
