/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         tablepref.cpp
Author:       Rainer Strobel (Qt3 original), Qt6 port by Lutfi Shihab

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This file implements the function table preferences dialog logic.

////////////////////////////////////////////////////////////////////////////////////////////*/

#include "tablepref.h"

#include <QEvent>
#include <QDoubleValidator>

void TablePreferences::windowActivationChange(bool active)
{
    Q_UNUSED(active);
    // In Qt3 this used to be a virtual from QWidget.
    // In Qt6 it's just a plain method; you can add any
    // activation-related behavior here if you like.
}

void TablePreferences::saveSlot()
{
    // Read numeric values from the widgets
    bool ok = false;

    double xStartVal = xStart->text().toDouble(&ok);
    if (ok) pref.tableXStart = xStartVal;

    double xEndVal = xEnd->text().toDouble(&ok);
    if (ok) pref.tableXEnd = xEndVal;

    double zStartVal = zStart->text().toDouble(&ok);
    if (ok) pref.tableZStart = zStartVal;

    double zEndVal = zEnd->text().toDouble(&ok);
    if (ok) pref.tableZEnd = zEndVal;

    double aVal = aValue->text().toDouble(&ok);
    if (ok) pref.tableAValue = aVal;

    pref.tableXSteps = xStep->value();
    pref.tableZSteps = zStep->value();

    // Map combo box selection to tableType
    const QString typeText = typeBox->currentText();

    if (typeText == TABLEH_STR3)
        pref.tableType = TABLENORMAL;
    else if (typeText == TABLEH_STR4)
        pref.tableType = TABLEPOLAR;
    else if (typeText == TABLEH_STR5)
        pref.tableType = TABLEPARAMETER;
    else if (typeText == TABLEH_STR6)
        pref.tableType = TABLEINEQUALITY;
    else if (typeText == TABLEH_STR7)
        pref.tableType = TABLE3D;
    else if (typeText == TABLEH_STR9)
        pref.tableType = TABLECOMPLEX;

    // Notify listeners and close dialog
    emit prefChange(pref);
    close();
}

