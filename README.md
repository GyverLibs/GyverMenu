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
- 9 встроенных виджетов
- Создание своих виджетов
- Механизм обновления виджетов
- Оптимизация количества перерисовок экрана (настраивается)
- Из коробки это текстовое меню, но можно сделать свои виджеты для графического дисплея

https://github.com/user-attachments/assets/41910bcf-d537-4f18-a999-8ff6d2c00438

> [Демо проект](https://wokwi.com/projects/436903131317665793) в симуляторе

### Совместимость
Совместима со всеми платформами

### Зависимости
- [StringN](https://github.com/GyverLibs/StringN)

## Содержание
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Описание классов
### GyverMenu
```cpp
// столбцов (длина строки), строк
GyverMenu(uint8_t cols, uint8_t rows);

// подключить рендер вида void(const char* str, size_t len). Придёт nullptr после окончания вывода
void onPrint(gm::Menu::PrintCb cb);

// подключить установку курсора вида uint8_t(uint8_t row, bool state)
void onCursor(gm::Menu::CursorCb cb);

// подключить билдер вида void(gm::Builder& b)
void onBuild(gm::Builder::BuildCb cb);

// установить текст кнопки "назад"
void setBackSign(const char* sign);

// обновить строку с переменной
void update(void* var);

// обновить экран
void refresh();

// на предыдущее меню
void back();

// в главное меню
void home();

// кнопка выбора
void set();

// кнопка вверх
void up();

// кнопка вниз
void down();

// уменьшить напрямую
void left();

// увеличить напрямую
void right();

// обновлять экран полностью, например для вывода в консоль (умолч. false)
void setFullRefresh(bool full);

// включить быстрый курсор - рендерить только курсор при смене строки (умолч. true)
void setFastCursor(bool fast);
```

### gm::Builder
#### Страница
```cpp
// начать страницу (подменю)
bool PageBegin(uint8_t id, const __FlashStringHelper* label);
bool PageBegin(uint8_t id, const char* label);

// закончить страницу (вызывать внутри условия по PageBegin). back - выводить кнопку "назад"
void PageEnd(bool back = true);

// страница с коллбэком (вместо PageBegin-PageEnd). back - выводить кнопку "назад"
void Page(uint8_t id, const __FlashStringHelper* label, void (*page)(Builder& b), bool back = true);
void Page(uint8_t id, const char* label, void (*page)(Builder& b), bool back = true);
```

#### Виджеты
```cpp
// кнопка
bool Button(const __FlashStringHelper* label, void (*cb)() = nullptr);
bool Button(const char* label, void (*cb)() = nullptr);

// просто текст
void Label(const __FlashStringHelper* line);
void Label(const char* line);

// выключатель
bool Switch(const __FlashStringHelper* label, bool* var, void (*cb)(bool v) = nullptr);
bool Switch(const char* label, bool* var, void (*cb)(bool v) = nullptr);

// выбор пункта. opts - строка с разделителем ';'
bool Select(const __FlashStringHelper* label, uint8_t* var, const __FlashStringHelper* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);
bool Select(const char* label, uint8_t* var, const char* opts, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// выбор пункта в стиле "вкладок". tabs - строка с разделителем ';'
bool Tabs(uint8_t* var, const __FlashStringHelper* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);
bool Tabs(uint8_t* var, const char* tabs, void (*cb)(uint8_t n, const char* str, uint8_t len) = nullptr);

// текстовое значение
void ValueStr(const __FlashStringHelper* label, const char* value);
void ValueStr(const char* label, const char* var);

// int значение
bool ValueInt(const __FlashStringHelper* label, T* var, T minv, T maxv, T step, uint8_t base, const __FlashStringHelper* unit, void (*cb)(T v) = nullptr);
bool ValueInt(const char* label, T* var, T minv, T maxv, T step, uint8_t base = 10, const char* unit = "", void (*cb)(T v) = nullptr);

// float значение
bool ValueFloat(const __FlashStringHelper* label, float* var, float minv, float maxv, float step, uint8_t dec, const __FlashStringHelper* unit, void (*cb)(float v) = nullptr);
bool ValueFloat(const char* label, float* var, float minv, float maxv, float step, uint8_t dec = 2, const char* unit = "", void (*cb)(float v) = nullptr);
```

#### Системное
```cpp
// обновить экран после работы билдера
void refresh();

// было действие с каким-то из виджетов выше
bool wasSet();

// сбросить флаг чтения wasSet
void clearSet();
```

#### API
```cpp
// начать виджет. true если разрешено
bool beginWidget();

// начать вывод виджета. true если разрешено
// targetVar - указатель на переменную виджета
// wCursor - рисовать ли курсор
bool beginRender(void* targetVar = nullptr, bool wCursor = true);

// получить действие виджета
Action getAction();

// поднять флаг изменения (влияет на wasSet())
void change();

// печатать в onPrint
void menu.print(char c);
void menu.print(const char* str);
void menu.print(const char* str, uint8_t len, uint8_t letters = 0);

// заполнить пробелами
void menu.pad(int8_t n);

// заполнить пробелами до конца
void menu.pad();

// переключить активное состояние (isActive)
void menu.toggle();

// текущая строка меню
uint8_t menu.currentRow();

// виджет активен (кнопкой set)
bool menu.isActive();

// виджет выбран курсором
bool menu.isChosen();

// виджет в видимой области экрана
bool menu.isVisible();
```

### Дефайны настроек
Объявляется перед подключением библиотеки

```cpp
#define GM_MAX_DEPTH 5  // макс. вложенность меню (умолч. 5)
#define GM_NO_PAGES     // отключить вложенные меню (облегчает библиотеку)
```

## Использование
> [!NOTE]
> Документация в разработке!

### Как это работает
Библиотека осуществляет навигацию по виртуальному меню, которое задаётся в билдере. В качестве точек входа библиотека имеет 5 функций виртуальных кнопок, которые нужно вызывать из основной программы по нажатию физических кнопок или другим событиям. В качестве точек выхода - три функции-обработчика: билдер, установка курсора и печать текста. Таким образом, библиотека вообще не привязана к конкретным дисплеям и способам ввода и даже к Arduino фреймворку - её можно запустить теоретически на любой платформе, само меню например выводить текстом в консоль, а кнопки "вверх" и "вниз" сделать голосовым вводом.

### Кнопки
Библиотека предусматривает **виртуальных** 5 кнопок управления + отдельные команды "вернуться на предыдущее меню" и "на главный экран", но на практике это может быть любое другое количество кнопок - хоть одна, хоть энкодер. Логика работы такая:

- `menu.set()` - центральная кнопка выбора. Она нажимает виджет `Button` на экране, переключает состояние виджета `Switch`, обрабатывает нажатие по кнопке возврата в предыдущее меню, а для остальных виджетов - переводит их в режим изменения
- `menu.up()` - переместить курсор на предыдущий пункт. Если активен режим изменения - вместо этого будет вызвано увеличение значения
- `menu.down()` - переместить курсор на следующий пункт. Если активен режим изменения - вместо этого будет вызвано уменьшение значения
- `menu.left()` - напрямую уменьшает значение текущего виджета независимо от режима изменения. Также нажимает кнопку "назад" для возврата в  предыдущее меню
- `menu.right()` - напрямую увеличивает значение текущего виджета независимо от режима изменения. Также нажимает кнопку входа в подменю и виджет `Button`
- `menu.back()` - возвращает на предыдущее меню
- `menu.home()` - возвращает в главное меню

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
Для использования меню нужно подключить 3 обработчика. Обязательным является только билдер, остальные могут не использоваться при написании своих виджетов под свой дисплей:

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
    // str - строка для вывода на дисплей
    // если str == nullptr - отрисовка меню закончена (для случаев когда нужно обновить экран)
    // len - кол-во символов в строке
}
menu.onPrint(printer);
```

#### Курсор
```cpp
uint8_t cursor(uint8_t row, bool chosen, bool active) {
    // row - текущая строка дисплея
    // chosen - выбран ли текущий виджет курсором
    // active - находится ли текущий виджет в режиме изменения
    // return количество символов (столбцов), которое занял курсор (если занял)
}
menu.onCursor(cursor);
```

Примеры обработчиков для LCD дисплея со стандартной библиотекой, курсор вида `>`, отсутствие курсора - пробел ` `:

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

Обработчик курсора вызывается не только перед началом отрисовки строки, но и при смене строки. Именно для этого он сделан отдельным - при смене курсора перерисовывается только курсор, а не всё меню, что повышает скорость и отзывчивость системы. Отключить такое поведение и всегда перерисовывать строку полностью можно при помощи `menu.setFastCursor(true)`.

Кажется что это слишком хитро - но зато гибко. Например, для OLED дисплея можно не рисовать курсор, а инвертировать вывод текста для текущей строки:

```cpp
menu.onCursor([](uint8_t row, bool chosen, bool active) -> uint8_t {
    oled.setCursor(0, row);
    oled.invertText(chosen);
    return 0;
});
menu.setFastCursor(true);
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

#### Трюки
У виджетов с настройкой шага изменения шаг можно задавать динамически, например от скорости вращения энкодера. С библиотекой EncButton это может выглядеть так:

```cpp
int vali;

menu.onBuild([](gm::Builder& b) {
    b.ValueInt<int>("ValueInt", &vali, -100, 100, encb.fast() ? 10 : 1);
});
```

Т.е. при быстром вращении энкодера шаг будет 10 (грубый), а при медленном - 1 (точный). Билдер вызывается на каждом действии изменения значения, поэтому шаг будет выбираться для каждого клика.

### Примеры
```cpp
```

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
