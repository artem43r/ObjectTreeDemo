# Отчёт к лабораторной работе № 2

## Дерево объектов Qt и автоматическое удаление виджетов

**Учебное заведение:** Северо-Кавказский федеральный университет  
**Институт:** перспективной инженерии  
**Направление:** 09.03.04 «Программная инженерия»  
**Студент:** Залин Артём Антонович  
**Группа:** ПИЖ-б-о-24-1  
**Преподаватель:** Новикова Е. Н.  
**Год:** 2026

> Перед сдачей проверьте правильность личных данных и добавьте ссылку на созданный Git-репозиторий в раздел «Репозиторий».

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

## 11. Репозиторий

**Ссылка:** `ВСТАВИТЬ_ССЫЛКУ_НА_ПУБЛИЧНЫЙ_GIT-РЕПОЗИТОРИЙ`

В репозитории должны находиться исходные файлы проекта, данный `README.md` и `.gitignore`. Папки сборки, исполняемые файлы и пользовательские настройки IDE добавлять не следует.

## 12. Контрольный список перед сдачей

- [ ] Проверить имя студента, группу и преподавателя на титульной странице.
- [ ] Добавить публичную ссылку на Git-репозиторий в раздел 11.
- [ ] Убедиться, что в репозитории присутствуют `ObjectTreeDemo.pro`, `main.cpp`, `mainwindow.h`, `mainwindow.cpp`, `widgets.h`, `README.md` и `.gitignore`.
- [ ] Сделать минимум 3–5 осмысленных коммитов.
- [ ] Проверить, что `build/`, `*.exe` и пользовательские файлы Qt Creator не попали в Git.
- [ ] При необходимости приложить в репозиторий скриншоты окна и результатов трёх экспериментов.
