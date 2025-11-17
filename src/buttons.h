#ifndef BUTTONSH
#define BUTTONSH

#include "global.h"

#include <QPushButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QString>
#include <QFont>
#include <QResizeEvent>
#include <stdio.h>

class StandardButtons : public QGroupBox
{
    Q_OBJECT

    int cols, rows;
    Preferences pref;
    QFont stdFont;
    QButtonGroup* group_;

public:
    explicit StandardButtons(QWidget *parent = nullptr)
    : QGroupBox(parent),
    cols(5),
    rows(4),
    group_(new QButtonGroup(this))
    {
        QString buttonName[] = {
            "7", "8", "9", "DEL", "AC",
            "4", "5", "6", "*",  "/",
            "1", "2", "3", "+",  "-",
            "0", ".", "EXP", "ANS", "="
        };

        group_->setExclusive(true);

        for (int c = 0; c < 20; ++c) {
            QPushButton *btn = new QPushButton(buttonName[c], this);
            group_->addButton(btn, c);          // use id = index
        }

        setMinimumWidth(280);
        setMaximumWidth(280);
        setMinimumHeight(200);
        setMaximumHeight(200);

        // QButtonGroup emits buttonClicked(int id) in Qt5/6
        QObject::connect(group_, SIGNAL(buttonClicked(int)),
                         this,   SLOT(buttonSlot(int)));
    }

    void setPref(Preferences newPref)
    {
        pref = newPref;
        if (pref.alpha)
            setAlphaSet();
        else if (pref.shift)
            setShiftSet();
        else
            setStandardSet();
    }

    void setStandardSet()
    {
        QString buttonName[] = {
            "7", "8", "9", "DEL", "AC",
            "4", "5", "6", "*",  "/",
            "1", "2", "3", "+",  "-",
            "0", ".", "EXP", "ANS", "="
        };

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

    void setAlphaSet()
    {
        QString buttonName[] = {
            "7", "8", "9", "DEL", "AC",
            "4", "5", "6", "W",   "X",
            "1", "2", "3", "Y",   "Z",
            "0", ".", "EXP", "ANS", "="
        };

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

    void setShiftSet()
    {
        QString buttonName[] = {
            "M", "G", "T", "DEL", "AC",
            "\xb5", "m", "k", "*", "/",
            "f", "p", "n", "+", "-",
            "0", ".", "EXP", "ANS", "="
        };

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void buttonSlot(int);

signals:
    void emitText(QString);
    void prefChange(Preferences);
};


class ExtButtons : public QGroupBox
{
    Q_OBJECT

    int cols, rows;
    Preferences pref;
    QButtonGroup *group_;

public:
    explicit ExtButtons(QWidget *parent = nullptr)
    : QGroupBox(parent),
    cols(6),
    rows(4),
    group_(new QButtonGroup(this))
    {
        // create 24 buttons, set ids 0..23
        for (int c = 0; c < 24; ++c) {
            QPushButton *btn = new QPushButton(this);
            group_->addButton(btn, c);
        }

        setStandardSet();

        // toggle buttons: 0, 6, 14
        if (QAbstractButton *b = group_->button(0))
            b->setCheckable(true);
        if (QAbstractButton *b = group_->button(6))
            b->setCheckable(true);
        if (QAbstractButton *b = group_->button(14))
            b->setCheckable(true);

        setMinimumWidth(300);
        setMaximumWidth(300);
        setMinimumHeight(160);
        setMaximumHeight(160);

        QObject::connect(group_, SIGNAL(buttonClicked(int)),
                         this,   SLOT(buttonSlot(int)));
    }

    void setPref(Preferences newPref)
    {
        pref = newPref;

        // buttons[0].setOn(pref.shift) etc. → setChecked
        if (QAbstractButton *b = group_->button(0))
            b->setChecked(pref.shift);
        if (QAbstractButton *b = group_->button(6))
            b->setChecked(pref.alpha);
        if (QAbstractButton *b = group_->button(14))
            b->setChecked(pref.hyp);

        if (pref.calcType == BASE) {
            if (pref.shift)
                setBaseShiftSet();
            else if (pref.alpha)
                setAlphaSet();
            else
                setBaseStandardSet();
        } else {
            if (pref.shift)
                setShiftSet();
            else if (pref.alpha)
                setAlphaSet();
            else if (pref.hyp)
                setHypSet();
            else
                setStandardSet();
        }
    }

    void setStandardSet()
    {
        QString buttonName[] = {
            "Shift", "",       "d/dx", "i",   "",        "",
            "Alpha", "x\xb2",  "^x",   "^-1", "\xb2",    "x",
            "log",   "ln",     "hyp",  "sin", "cos",     "tan",
            ";",     "X",      "(",    ")",   ",",       "->"
        };

        buttonName[1].insert(0,  getUnicode(INTEGRALSTRING));
        buttonName[10].insert(1, getUnicode(ROOTSTRING));
        buttonName[11].insert(1, getUnicode(ROOTSTRING));
        buttonName[4].insert(0,  getUnicode(PISTRING));
        buttonName[5].insert(0,  getUnicode(EULERSTRING));

        // button 14 is toggle
        if (QAbstractButton *b = group_->button(14))
            b->setCheckable(true);

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

    void setShiftSet()
    {
        QString buttonName[] = {
            "Shift","Rand","!",    "i",   "",        "",
            "Alpha","x\xb3","\xb3","abs","arg",     "conj",
            "10^", "^",    "hyp", "asin","acos",    "atan",
            "==",  "[",    "{",   "}",   "]",       ""
        };

        buttonName[8].insert(1,  getUnicode(ROOTSTRING));
        buttonName[13].insert(0, getUnicode(EULERSTRING));
        buttonName[4].insert(0,  getUnicode(PISTRING));
        buttonName[5].insert(0,  getUnicode(EULERSTRING));
        buttonName[23].insert(0, getUnicode(DEGREESTRING));

        if (QAbstractButton *b = group_->button(14))
            b->setCheckable(true);

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }

        if (pref.hyp)
            setHypSet();
    }

    void setAlphaSet()
    {
        QString buttonName[] = {
            "Shift","A","B","C","D","E",
            "Alpha","F","G","H","I","J",
            "K","L","M","N","O","P",
            "Q","R","S","T","U","V"
        };

        if (QAbstractButton *b = group_->button(14))
            b->setCheckable(false);

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

    void setHypSet()
    {
        if (pref.shift) {
            if (QAbstractButton *b = group_->button(15))
                b->setText("asinh");
            if (QAbstractButton *b = group_->button(16))
                b->setText("acosh");
            if (QAbstractButton *b = group_->button(17))
                b->setText("atanh");
        } else {
            if (QAbstractButton *b = group_->button(15))
                b->setText("sinh");
            if (QAbstractButton *b = group_->button(16))
                b->setText("cosh");
            if (QAbstractButton *b = group_->button(17))
                b->setText("tanh");
        }
    }

    void setBaseStandardSet()
    {
        QString buttonName[] = {
            "Shift","Rand","!",   "%",   "x",   "^",
            "Alpha","x�",  "&&",  "|",   "~",   "xor",
            "A",    "B",   "C",   "D",   "E",   "F",
            "==",   "X",   "(",   ")",   ",",   "->"
        };

        buttonName[4].insert(1, getUnicode(ROOTSTRING));

        if (QAbstractButton *b = group_->button(14))
            b->setCheckable(false);

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

    void setBaseShiftSet()
    {
        QString buttonName[] = {
            "Shift","Rand",">",   "%",   "x",    "^",
            "Alpha","x\xb3","&&&&","||", "<",    "xor",
            "dec",  "bin", "hex", "oct", "<<",   ">>",
            "!=",   "X",   "(",   ")",   ",",    "->"
        };

        buttonName[4].insert(1, getUnicode(ROOTSTRING));

        if (QAbstractButton *b = group_->button(14))
            b->setCheckable(false);

        for (int c = 0; c < cols*rows; ++c) {
            if (QAbstractButton *ab = group_->button(c)) {
                if (QPushButton *currentButton = qobject_cast<QPushButton*>(ab))
                    currentButton->setText(buttonName[c]);
            }
        }
    }

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void buttonSlot(int);

signals:
    void emitText(QString);
    void prefChange(Preferences);
};

#endif // BUTTONSH
