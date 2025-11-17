/*
 * /////////////////////////////////////////Extcalc////////////////////////////////////////////
 * /////////////////////////////////Scientific Graphic Calculator//////////////////////////////
 * -File:         buttons.cpp
 * -Author:       Rainer Strobel
 * -Email:        rainer1223@users.sourceforge.net
 * -Homepage:     http://extcalc-linux.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of
 * the GNU General Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * ////////////////////////////////////////////////////////////////////////////////////////////
 */

#include "buttons.h"
#include <QResizeEvent>
#include <QAbstractButton>
#include <QPushButton>

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////StandardButtons////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

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
        {
            // Hyp toggle
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

#include "buttons.h"
#include <QResizeEvent>
#include <QAbstractButton>
#include <QPushButton>

/////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////StandardButtons////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

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
        {
            // Hyp toggle
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
