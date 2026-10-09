# Отчёт к лабораторной работе № 2

## Дерево объектов Qt и автоматическое удаление виджетов

**Учебное заведение:** Северо-Кавказский федеральный университет  
**Институт:** перспективной инженерии  
**Направление:** 09.03.04 «Программная инженерия»  
**Студент:** Залин Артём Антонович  
**Группа:** ПИЖ-б-о-24-1  
**Год:** 2026

## 1. Цель работы

Изучить механизм дерева объектов Qt, принцип владения объектами через родителя, автоматическое удаление дочерних объектов при уничтожении родителя, а также разобраться в корректном и некорректном использовании `setParent()` в приложениях Qt Widgets.

## 2. Задачи работы

1. Изучить понятие дерева объектов `QObject`.
2. Понять, как родитель владеет дочерними объектами.
3. Научиться создавать виджеты с явным родителем.
4. Реализовать окно минимум с пятью виджетами.
5. Отследить создание и уничтожение объектов через `qDebug()` в деструкторах.
6. Продемонстрировать автоматическое удаление дочерних виджетов при удалении родителя.
7. Исследовать поведение `setParent()` при смене владельца виджета.
8. Разобрать типичные ошибки, связанные с `setParent()`, удалением объектов и висячими указателями.
9. Оформить отчёт и загрузить проект в Git-репозиторий.

## 3. Постановка задачи

Разработать приложение Qt Widgets `ObjectTreeDemo`, демонстрирующее дерево объектов и управление временем жизни виджетов. Приложение должно выводить сообщения при создании и уничтожении объектов, позволять перемещать виджет между родителями, удалять панель вместе с дочерними элементами и демонстрировать отвязку объекта с помощью `setParent(nullptr)`.

Интерфейс создаётся программно, без файла `.ui`.

## 4. Теоретическое обоснование

### 4.1. Дерево объектов `QObject`

Объект `QObject` может иметь одного родителя и произвольное количество дочерних объектов. Так образуется дерево объектов. Родитель владеет своими дочерними объектами: при уничтожении родителя Qt автоматически уничтожает его детей. Это помогает управлять временем жизни объектов и уменьшает необходимость вручную удалять каждый дочерний объект.

### 4.2. Родители виджетов `QWidget`

Для `QWidget` родитель определяет не только владение объектом, но и визуальную иерархию. Дочерний виджет отображается внутри родительского виджета и обычно располагается в его layout. Поэтому создание виджета с родителем, например `new QPushButton("OK", parent)`, позволяет Qt автоматически удалить кнопку вместе с родителем.

### 4.3. Метод `setParent()`

Метод `setParent(newParent)` изменяет родителя объекта. Для виджета это меняет владельца и визуальную иерархию. При переносе между layout нужно удалить виджет из старого layout, вызвать `setParent()`, добавить виджет в новый layout и показать его.

Вызов `setParent(nullptr)` отвязывает объект от прежнего родителя. Такой объект больше не будет автоматически удалён вместе с прежним родителем, поэтому приложение должно самостоятельно позаботиться о его удалении.

### 4.4. `delete` и `deleteLater()`

Оператор `delete` уничтожает объект немедленно. При работе с сигналами, слотами и обработчиками событий немедленное удаление может быть небезопасным. Метод `deleteLater()` откладывает уничтожение до обработки соответствующего события циклом событий Qt.

### 4.5. `QPointer`

`QPointer<T>` — отслеживающий указатель на объект Qt. Если объект уничтожается, `QPointer` автоматически становится пустым (`nullptr`). Это позволяет проверить, существует ли объект, прежде чем обращаться к нему, и снижает риск использования висячих указателей.

### 4.6. Конструкторы, деструкторы и `qDebug()`

В проекте определены производные классы `LogLabel`, `LogButton`, `LogLineEdit` и `LogPanel`. Их конструкторы выводят сообщение `[CREATE]`, а деструкторы — `[DESTROY]`. По этим сообщениям можно наблюдать жизненный цикл объектов. Диагностические сообщения сделаны англоязычными, чтобы избежать проблем с отображением русских символов в выводе отладки; тексты пользовательского интерфейса остаются на русском языке.

## 5. Структура проекта

```text
ObjectTreeDemo/
├── ObjectTreeDemo.pro
├── main.cpp
├── mainwindow.h
├── mainwindow.cpp
├── widgets.h
├── README.md
└── .gitignore
```

- `ObjectTreeDemo.pro` — конфигурация qmake: модуль Qt Widgets, стандарт C++17 и список исходных файлов.
- `main.cpp` — создание `QApplication`, главного окна и запуск цикла событий.
- `mainwindow.h` — объявление главного окна, его слотов и отслеживаемых указателей `QPointer`.
- `mainwindow.cpp` — создание интерфейса и реализация трёх демонстрационных действий.
- `widgets.h` — классы виджетов с журналированием создания и уничтожения.
- `README.md` — отчёт по лабораторной и инструкция по запуску.
- `.gitignore` — исключения для генерируемых файлов и результатов сборки.

**Публичный Git-репозиторий:** [github.com/artem43r/ObjectTreeDemo](https://github.com/artem43r/ObjectTreeDemo).

## 6. Листинг кода

### 6.1. `ObjectTreeDemo.pro`

```pro
QT += widgets

CONFIG += c++17 console
CONFIG -= app_bundle

TARGET = ObjectTreeDemo
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    widgets.h
```

### 6.2. `widgets.h`

```cpp
#ifndef WIDGETS_H
#define WIDGETS_H

#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class LogLabel : public QLabel
{
public:
    explicit LogLabel(const QString &text, QWidget *parent = nullptr)
        : QLabel(text, parent)
    {
        qDebug() << "[CREATE] LogLabel";
    }

    ~LogLabel() override
    {
        qDebug() << "[DESTROY] LogLabel";
    }
};

class LogButton : public QPushButton
{
public:
    explicit LogButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
        qDebug() << "[CREATE] LogButton";
    }

    ~LogButton() override
    {
        qDebug() << "[DESTROY] LogButton";
    }
};

class LogLineEdit : public QLineEdit
{
public:
    explicit LogLineEdit(QWidget *parent = nullptr)
        : QLineEdit(parent)
    {
        qDebug() << "[CREATE] LogLineEdit";
    }

    ~LogLineEdit() override
    {
        qDebug() << "[DESTROY] LogLineEdit";
    }
};

class LogPanel : public QFrame
{
public:
    explicit LogPanel(const QString &title, QWidget *parent = nullptr)
        : QFrame(parent)
    {
        setObjectName(title);
        setFrameShape(QFrame::StyledPanel);
        qDebug() << "[CREATE] LogPanel";
    }

    ~LogPanel() override
    {
        qDebug() << "[DESTROY] LogPanel";
    }
};

#endif // WIDGETS_H
```

### 6.3. `mainwindow.h`

```cpp
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>

#include "widgets.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void moveMovableWidget();
    void deletePanel();
    void demoDetaching();

private:
    void setupUi();
    void setStatus(const QString &text);

    QPointer<QVBoxLayout> m_rootLayout;
    QPointer<QVBoxLayout> m_panelLayout;

    QPointer<LogLabel> m_title;
    QPointer<LogLineEdit> m_input;
    QPointer<LogLabel> m_status;

    QPointer<LogPanel> m_panel;
    QPointer<LogButton> m_movable;

    QPointer<LogButton> m_moveButton;
    QPointer<LogButton> m_deleteButton;
};

#endif // MAINWINDOW_H
```

### 6.4. `mainwindow.cpp`

```cpp
#include "mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setObjectName("MainWindow");
    setWindowTitle("Qt Object Tree Demo");
    resize(520, 640);

    m_rootLayout = new QVBoxLayout(this);

    m_title = new LogLabel(
        "Дерево объектов Qt: владелец и потомки",
        this
    );
    m_rootLayout->addWidget(m_title.data());

    m_input = new LogLineEdit(this);
    m_input->setPlaceholderText("Виджет-ребёнок главного окна");
    m_rootLayout->addWidget(m_input.data());

    m_status = new LogLabel("Статус: окно создано", this);
    m_status->setWordWrap(true);
    m_rootLayout->addWidget(m_status.data());

    m_panel = new LogPanel("Панель-владелец", this);
    m_rootLayout->addWidget(m_panel.data());

    m_panelLayout = new QVBoxLayout(m_panel.data());

    LogButton *staticButton = new LogButton(
        "Статическая кнопка внутри панели",
        m_panel.data()
    );
    m_panelLayout->addWidget(staticButton);

    m_movable = new LogButton(
        "Перемещаемый виджет (в панели)",
        m_panel.data()
    );
    m_panelLayout->addWidget(m_movable.data());

    m_moveButton = new LogButton(
        "Переместить виджет: панель <-> окно",
        this
    );
    connect(m_moveButton.data(), &QPushButton::clicked,
            this, &MainWindow::moveMovableWidget);
    m_rootLayout->addWidget(m_moveButton.data());

    m_deleteButton = new LogButton(
        "Удалить панель (deleteLater)",
        this
    );
    connect(m_deleteButton.data(), &QPushButton::clicked,
            this, &MainWindow::deletePanel);
    m_rootLayout->addWidget(m_deleteButton.data());

    LogButton *detachButton = new LogButton(
        "Демо setParent(nullptr)",
        this
    );
    connect(detachButton, &QPushButton::clicked,
            this, &MainWindow::demoDetaching);
    m_rootLayout->addWidget(detachButton);

    m_rootLayout->addStretch();

    setStatus(
        "Готово. Все виджеты созданы с родителями. "
        "Удаление родителя автоматически удалит детей."
    );
}

void MainWindow::setStatus(const QString &text)
{
    if (!m_status.isNull()) {
        m_status->setText(text);
    }
}

void MainWindow::moveMovableWidget()
{
    if (m_movable.isNull()) {
        setStatus("Перемещаемый виджет уже уничтожен.");
        return;
    }

    if (m_panel.isNull()) {
        setStatus("Панель уже удалена. Вернуть виджет в неё невозможно.");
        return;
    }

    if (m_movable->parentWidget() == m_panel.data()) {
        if (!m_panelLayout.isNull()) {
            m_panelLayout->removeWidget(m_movable.data());
        }

        m_movable->setParent(this);

        if (!m_rootLayout.isNull()) {
            // Вставляем перед кнопками управления, чтобы виджет оставался видимым.
            m_rootLayout->insertWidget(4, m_movable.data());
        }

        m_movable->setText("Перемещаемый виджет (вне панели)");
        setStatus(
            "setParent(this) сделал главным окном новым владельцем. "
            "Панель больше не удалит этот виджет автоматически."
        );
    } else {
        if (!m_rootLayout.isNull()) {
            m_rootLayout->removeWidget(m_movable.data());
        }

        m_movable->setParent(m_panel.data());

        if (!m_panelLayout.isNull()) {
            m_panelLayout->addWidget(m_movable.data());
        }

        m_movable->setText("Перемещаемый виджет (в панели)");
        setStatus(
            "Виджет снова стал ребёнком панели. "
            "Теперь он будет удалён вместе с панелью."
        );
    }

    m_movable->show();
    qDebug() << "[SET_PARENT] Movable widget parent changed";
}

void MainWindow::deletePanel()
{
    if (m_panel.isNull()) {
        setStatus("Панель уже удалена.");
        return;
    }

    if (!m_movable.isNull() &&
        m_movable->parentWidget() == m_panel.data()) {
        setStatus(
            "Панель будет удалена. Перемещаемый виджет является её ребёнком, "
            "поэтому будет уничтожен автоматически."
        );
    } else {
        setStatus(
            "Панель будет удалена. Перемещаемый виджет не является её ребёнком, "
            "поэтому останется жив."
        );
    }

    if (!m_deleteButton.isNull()) {
        m_deleteButton->setEnabled(false);
    }

    if (!m_moveButton.isNull()) {
        m_moveButton->setEnabled(false);
    }

    qDebug() << "[DELETE_LATER] Panel deletion requested";
    m_panel->deleteLater();
}

void MainWindow::demoDetaching()
{
    LogPanel *tempOwner = new LogPanel("TempOwner", this);
    tempOwner->hide();

    new LogLabel("Ребёнок TempOwner", tempOwner);

    LogLabel *detachedChild = new LogLabel(
        "Отвязанный ребёнок TempOwner",
        tempOwner
    );

    qDebug() << "[DEMO] Children before detach:"
             << tempOwner->children().count();

    detachedChild->setParent(nullptr);

    qDebug() << "[DEMO] Children after detach:"
             << tempOwner->children().count();

    setStatus(
        "Демо: одна метка осталась ребёнком TempOwner, "
        "другая отвязана через setParent(nullptr). "
        "При удалении TempOwner автоматическое удаление применится "
        "только к первой."
    );

    tempOwner->deleteLater();
    detachedChild->deleteLater();
}
```

### 6.5. `main.cpp`

```cpp
#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
```

## 7. Сборка и запуск

### Требования

- Qt 6 с модулем Qt Widgets;
- Qt Creator;
- комплект компилятора MinGW 64-bit для Windows (или другой совместимый комплект);
- система сборки qmake.

### Запуск в Qt Creator

1. Открыть `ObjectTreeDemo.pro` в Qt Creator.
2. Выбрать установленный Desktop Kit.
3. Выполнить **Сборка → Запустить qmake**.
4. Собрать проект (`Ctrl+B`).
5. Запустить проект (`Ctrl+R`).
6. Для просмотра сообщений `qDebug()` использовать вывод приложения/терминал Qt Creator.

## 8. Проверка работы и результаты экспериментов

### Эксперимент A. Автоматическое удаление дочерних виджетов

Исходная иерархия:

```text
MainWindow
└── Панель-владелец
    ├── Статическая кнопка
    └── Перемещаемый виджет
```

Не перемещая кнопку, нажать **«Удалить панель (deleteLater)»**. После обработки события панель и обе её дочерние кнопки уничтожаются. Вручную удалять дочерние кнопки не требуется.

Пример сообщений:

```text
[DELETE_LATER] Panel deletion requested
[DESTROY] LogPanel
[DESTROY] LogButton
[DESTROY] LogButton
```

### Эксперимент B. Смена родителя

Перезапустить приложение. Нажать **«Переместить виджет: панель <-> окно»**, затем нажать **«Удалить панель (deleteLater)»**. Перемещаемая кнопка становится ребёнком `MainWindow`, поэтому после удаления панели остаётся в окне. Статическая кнопка, оставшаяся внутри панели, уничтожается вместе с ней.

Пример сообщений:

```text
[SET_PARENT] Movable widget parent changed
[DELETE_LATER] Panel deletion requested
[DESTROY] LogPanel
[DESTROY] LogButton
```

### Эксперимент C. `setParent(nullptr)`

Перезапустить приложение и нажать **«Демо setParent(nullptr)»**. Создаётся временный владелец с двумя дочерними метками. После отвязки одной метки количество детей уменьшается с двух до одного. Панель и оставшаяся метка удаляются через `deleteLater()`, а отвязанная метка удаляется отдельным вызовом `deleteLater()`.

Пример сообщений:

```text
[CREATE] LogPanel
[CREATE] LogLabel
[CREATE] LogLabel
[DEMO] Children before detach: 2
[DEMO] Children after detach: 1
[DESTROY] LogPanel
[DESTROY] LogLabel
[DESTROY] LogLabel
```

Порядок сообщений об уничтожении дочерних объектов может различаться; существенный результат — дочерние объекты панели уничтожаются автоматически, а отвязанный объект удаляется отдельно.

## 9. Проблемы, возникшие при выполнении, и их решения

1. **Шаблон Qt Creator отличался от структуры в методичке.** Изначально были созданы стандартные `widget.cpp`, `widget.h` и `widget.ui`. Для реализации по методичке проект был приведён к файлам `mainwindow.cpp`, `mainwindow.h` и `widgets.h`, после чего список исходных и заголовочных файлов был обновлён в `.pro`.
2. **Сообщения `qDebug()` с русским текстом отображались некорректно.** Для диагностических строк использованы короткие сообщения на английском языке, состоящие из ASCII-символов. Русские строки пользовательского интерфейса при этом сохранены.
3. **После переноса кнопка могла исчезнуть из видимой области layout.** Причина заключалась в добавлении виджета после растягивающего элемента layout. При переносе теперь используется `insertWidget(4, m_movable.data())`, что помещает кнопку в видимую область перед кнопками управления.

## 10. Вывод

В ходе работы изучено дерево объектов Qt и принцип владения дочерними объектами через родителя. Создано приложение Qt Widgets, которое отслеживает создание и уничтожение виджетов и демонстрирует три ситуации: автоматическое удаление дочерних виджетов, перенос виджета к другому родителю и отвязку через `setParent(nullptr)`. На практике использованы `QPointer`, `deleteLater()`, `setParent()`, layout и сообщения `qDebug()`. Эксперименты показали, что смена родителя изменяет владельца виджета, а дочерний объект автоматически уничтожается вместе со своим текущим родителем.

## 11. Ответы на контрольные вопросы

### 1. Что такое дерево объектов `QObject`?

Это иерархия объектов Qt, в которой каждый объект может иметь одного родителя и произвольное количество дочерних объектов.

### 2. Кто является владельцем дочернего объекта?

Его родитель (`parent`). Родитель отвечает за время жизни дочернего объекта.

### 3. Что произойдёт при удалении родителя?

Qt автоматически уничтожит его дочерние объекты. Это избавляет от необходимости вручную удалять каждого ребёнка.

### 4. Чем `QWidget`-дерево отличается от простого `QObject`-дерева?

У `QWidget` родитель задаёт не только владение объектом, но и визуальную иерархию: дочерний виджет размещается внутри родителя, использует связанную с ним систему координат и участвует в обработке событий.

### 5. Зачем передавать `parent` в конструктор виджета?

Чтобы сразу назначить родителя. Для виджета это устанавливает владельца и визуального родителя, а также позволяет Qt автоматически удалить его вместе с родителем.

### 6. Что делает `setParent()`?

Меняет родителя объекта. Для виджета меняются владелец и визуальная иерархия. При переносе между контейнерами нужно также корректно обновить layout.

### 7. Что произойдёт при вызове `setParent(nullptr)`?

Объект отвяжется от прежнего родителя и перестанет удаляться вместе с ним. За его дальнейшее удаление приложение должно отвечать самостоятельно — например, вызвать `deleteLater()`.

### 8. Можно ли вручную удалять объект, у которого есть родитель?

Технически можно, но обычно в этом нет необходимости: родитель удалит ребёнка автоматически. Если объект удалён вручную, нельзя продолжать пользоваться указателями на него.

### 9. Почему опасно удалять объект через `delete` внутри обработчика его собственного сигнала?

Немедленное удаление может уничтожить объект, пока его методы или обработка сигнала ещё выполняются. Это способно привести к обращению к уже уничтоженному объекту. В подобных сценариях безопаснее использовать `deleteLater()`.

### 10. Чем `deleteLater()` отличается от `delete`?

`delete` уничтожает объект немедленно. `deleteLater()` откладывает удаление до того момента, когда цикл событий Qt обработает запрос на отложенное удаление.

### 11. Что такое висячий указатель?

Это указатель, который хранит адрес уже уничтоженного объекта. Обращение через него недопустимо и может привести к неопределённому поведению или аварийному завершению программы.

### 12. Как `QPointer` помогает избежать работы с удалённым объектом?

`QPointer<T>` автоматически становится пустым, когда отслеживаемый объект `QObject` уничтожается. Перед обращением к объекту можно проверить `isNull()`.

### 13. Почему стековый объект нельзя делать ребёнком кучного родителя?

При уничтожении кучного родителя Qt попытается удалить ребёнка. Если ребёнок создан на стеке, это может привести к повторному уничтожению при выходе из области видимости и падению программы.

### 14. Как правильно перемещать виджет из одного layout в другой?

Сначала удалить его из старого layout через `removeWidget()`, затем вызвать `setParent()` для нового родителя, добавить виджет в новый layout через `addWidget()` и при необходимости вызвать `show()`.

### 15. Что произойдёт, если удалить родителя, а затем обратиться к указателю на ребёнка?

Если ребёнок автоматически уничтожен вместе с родителем, обычный указатель на него станет висячим. Разыменовывать его нельзя. `QPointer` при уничтожении объекта обнулится.

### 16. Как отследить момент уничтожения объекта через деструктор?

Определить собственный производный класс и переопределить его деструктор, добавив диагностическое сообщение через `qDebug()`. В этой работе для этого используются классы `LogLabel`, `LogButton`, `LogLineEdit` и `LogPanel`.

### 17. Почему порядок вызова деструкторов родителя и детей может быть неочевидным?

Сначала выполняется тело деструктора производного класса, а затем разрушаются базовые классы. Удаление дочерних объектов `QObject` происходит на этапе деструктора базового класса `QObject`, поэтому сообщение родителя может появиться до сообщений о детях.

### 18. Что возвращает `children()` у `QObject`?

Список непосредственных дочерних объектов этого `QObject` (`QObjectList`). Он не является списком всех потомков на любой глубине.

### 19. Чем `findChildren<QWidget*>()` отличается от `children()`?

`children()` возвращает всех непосредственных детей независимо от их типа. `findChildren<QWidget*>()` ищет дочерние объекты заданного типа; по умолчанию поиск рекурсивно охватывает и более глубоких потомков.

### 20. Можно ли иметь у объекта несколько родителей?

Нет. У объекта `QObject` может быть только один родитель, но у родителя может быть произвольное количество детей.

### 21. Как связано дерево объектов и цикл событий Qt?

Дерево объектов задаёт владение и время жизни объектов. Цикл событий обрабатывает события приложения, в том числе запросы `deleteLater()`. Чтобы отложенное удаление и взаимодействие с интерфейсом выполнялись штатно, GUI-приложение запускает цикл событий через `app.exec()`.

### 22. Почему для GUI-приложения нужен `QApplication` и `app.exec()`?

`QApplication` обеспечивает инфраструктуру приложения с виджетами. Вызов `app.exec()` запускает цикл событий, необходимый для обработки пользовательского ввода, кликов, перерисовки окна и отложенного удаления объектов.

### 23. Что произойдёт, если попытаться сделать родителем объект из другого потока?

Qt не позволяет безопасно устанавливать родителя, если объекты принадлежат разным потокам; может появиться предупреждение `QObject::setParent: Cannot set parent, new parent is in a different thread`. Объекты одного дерева должны находиться в одном потоке. Для взаимодействия между потоками используют сигналы и слоты.

### 24. Как проверить отсутствие утечек памяти?

В рамках этой работы можно отслеживать создание и уничтожение объектов по сообщениям в конструкторах и деструкторах и проверять, что динамически созданные объекты имеют владельцев либо явно удаляются. Для более глубокой проверки применяют инструменты анализа памяти; конкретный инструмент методичка не предписывает.

### 25. Почему в данной работе используется `QPointer`, а не обычные указатели?

Потому что некоторые виджеты могут быть уничтожены вместе с родителем. `QPointer` автоматически обнуляется после уничтожения объекта, поэтому позволяет проверить его существование перед обращением и снижает риск использования висячего указателя.
