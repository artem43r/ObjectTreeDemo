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