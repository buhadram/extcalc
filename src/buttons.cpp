/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////
//
// File:         buttons.cpp
// Author:       Rainer Strobel
// Email:        rainer1223@users.sourceforge.net
// Homepage:     http://extcalc-linux.sourceforge.net
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
////////////////////////////////////////////////////////////////////////////////////////////*/

#include "buttons.h"
#include <QAbstractButton>
#include <QFont>
#include <QPushButton>
#include <QSizePolicy>


/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////StandardButtons////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

StandardButtons::StandardButtons(QWidget *parent)
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

void StandardButtons::setPref(Preferences newPref)
{
    pref = newPref;
    if (pref.alpha)
        setAlphaSet();
    else if (pref.shift)
        setShiftSet();
    else
        setStandardSet();
}

void StandardButtons::setStandardSet()
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

void StandardButtons::setAlphaSet()
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

void StandardButtons::setShiftSet()
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


void StandardButtons::buttonSlot(int num)
{
    QAbstractButton *ab = group_->button(num);
    QString text;
    if (QPushButton *pb = qobject_cast<QPushButton*>(ab))
        text = pb->text();

    switch (num)
    {
    case 0:
        if (pref.shift)
            emit emitText(getUnicode(MEGASTRING));
        else
            emit emitText(text);
        break;
    case 1:
        if (pref.shift)
            emit emitText(getUnicode(GIGASTRING));
        else
            emit emitText(text);
        break;
    case 2:
        if (pref.shift)
            emit emitText(getUnicode(TERASTRING));
        else
            emit emitText(text);
        break;
    case 3:
        emit emitText("backkey");
        break;
    case 4:
        emit emitText("clearall");
        break;
    case 17:
        emit emitText("e");
        break;
    case 18:
        emit emitText("ans");
        break;
    case 19:
        emit emitText("calculate");
        break;
    default:
        if (!text.isEmpty())
            emit emitText(text);
        break;
    }

    // Reset shift/alpha/hyp after a button press that uses them.
    if (pref.hyp || pref.shift || pref.alpha)
    {
        pref.hyp = pref.shift = pref.alpha = false;
        emit prefChange(pref);
    }
}

void StandardButtons::resizeEvent(QResizeEvent *)
{
    int width  = geometry().right() - geometry().left() - 2;
    int height = geometry().bottom() - geometry().top() - 2;

    if (width <= 0 || height <= 0)
        return;

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            int id = r * cols + c;
            if (QAbstractButton *ab = group_->button(id))
            {
                ab->setGeometry(c * (width / cols) + 2,
                                r * (height / rows) + 2,
                                width / cols,
                                height / rows);
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////ExtButtons//////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

ExtButtons::ExtButtons(QWidget *parent)
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

void ExtButtons::setPref(Preferences newPref)
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

void ExtButtons::setStandardSet()
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

void ExtButtons::setShiftSet()
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

void ExtButtons::setAlphaSet()
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

void ExtButtons::setHypSet()
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

void ExtButtons::setBaseStandardSet()
{
    QString buttonName[] = {
        "Shift","Rand","!",   "%",   "x",   "^",
        "Alpha","x\xb2",  "&&",  "|",   "~",   "xor",
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

void ExtButtons::setBaseShiftSet()
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


void ExtButtons::buttonSlot(int num)
{
    QAbstractButton *ab = group_->button(num);
    QString text;
    if (QPushButton *pb = qobject_cast<QPushButton*>(ab))
        text = pb->text();

    switch (num)
    {
    case 0:
        // Shift toggle
        pref.shift = !pref.shift;
        emit prefChange(pref);
        break;

    case 1:
        if (pref.calcType == SCIENTIFIC && !pref.shift && !pref.alpha)
            emit emitText(getUnicode(INTEGRALSTRING) + "(");
        else if ((pref.calcType == SCIENTIFIC && pref.shift && !pref.alpha) ||
                 (pref.calcType == BASE && !pref.alpha))
            emit emitText("rnd(");
        else
            emit emitText(text);
        break;

    case 2:
        if (pref.calcType == SCIENTIFIC && !pref.shift && !pref.alpha)
            emit emitText("d/dx(");
        else
            emit emitText(text);
        break;

    case 4:
        if (pref.calcType == BASE && !pref.alpha)
            emit emitText(getUnicode(ROOTSTRING));
        else
            emit emitText(text);
        break;

    case 6:
        // Alpha toggle
        pref.alpha = !pref.alpha;
        emit prefChange(pref);
        break;

    case 7:
        if (pref.shift)
            emit emitText("\xb3");
        else if (pref.alpha)
            emit emitText(text);
        else
            emit emitText("\xb2");
        break;

    case 8:
        if (pref.calcType == SCIENTIFIC)
        {
            if (pref.shift)
                emit emitText(text);
            else if (pref.alpha)
                emit emitText(text);
            else
                emit emitText("^");
        }
        else // BASE mode
        {
            if (pref.shift)
                emit emitText("&&");
            else if (pref.alpha)
                emit emitText(text);
            else
                emit emitText("&");
        }
        break;

    case 11:
        if (pref.calcType == BASE && !pref.alpha)
            emit emitText("x");
        else if (pref.calcType == SCIENTIFIC && !pref.alpha && !pref.shift)
            emit emitText(getUnicode(ROOTSTRING));
        else
            emit emitText(text);
        break;

    case 14:
        if (pref.alpha || pref.calcType == BASE)
            emit emitText(text);
        else
        {            // Hyp toggle
            pref.hyp = !pref.hyp;
            emit prefChange(pref);
        }
        break;

    default:
        if (!text.isEmpty())
            emit emitText(text);
        break;
    }

    // Auto-reset flags in same conditions as original logic
    if ((num != 14 && num != 0 && num != 6 && (pref.hyp || pref.shift)) ||
        (num != 0 && num != 6 && (pref.alpha || pref.calcType == BASE)))
    {
        pref.hyp = pref.shift = pref.alpha = false;
        emit prefChange(pref);
    }
}

void ExtButtons::resizeEvent(QResizeEvent *)
{
    int width  = geometry().right() - geometry().left() - 2;
    int height = geometry().bottom() - geometry().top() - 2;

    if (width <= 0 || height <= 0)
        return;

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            int id = r * cols + c;
            if (QAbstractButton *ab = group_->button(id))
            {
                ab->setGeometry(c * (width / cols) + 2,
                                r * (height / rows) + 2,
                                width / cols,
                                height / rows);
            }
        }
    }
}
