/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         calcpref.cpp
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.


////////////////////////////////////////////////////////////////////////////////////////////*/
#include "calcpref.h"
#include <QEvent>

void CalcPreferences::saveSlot()
{
    // angle
    switch (angleBox->currentIndex()) {
        case 0:  pref.angle = DEG; break;
        case 1:  pref.angle = RAD; break;
        case 2:  pref.angle = GRA; break;
        default: pref.angle = DEG; break;
    }

    // output type
    switch (outputBox->currentIndex()) {
        case 0:  pref.outputType = FIXEDNUM;    break;
        case 1:  pref.outputType = VARIABLENUM; break;
        case 2:  pref.outputType = EXPSYM;      break;
        default: pref.outputType = FIXEDNUM;    break;
    }

    // calculator mode
    switch (calcModeBox->currentIndex()) {
        case 0:  pref.calcType = SCIENTIFIC; break;
        case 1:  pref.calcType = BASE;       break;
        default: pref.calcType = SCIENTIFIC; break;
    }

    // base
    switch (baseBox->currentIndex()) {
        case 0:  pref.base = HEX; break;
        case 1:  pref.base = DEC; break;
        case 2:  pref.base = OCT; break;
        case 3:  pref.base = BIN; break;
        default: pref.base = DEC; break;
    }

    pref.complex      = complexBox->isChecked();
    pref.outputLength = outputLenBox->value();

    emit prefChange(pref);
    close();
}

void CalcPreferences::changeEvent(QEvent *event)
{
    // Replacement for old windowActivationChange(bool)
    if (event->type() == QEvent::WindowDeactivate) {
        if (!isActiveWindow()) {
            // Qt3: setActiveWindow();
            // Qt5/6:
            activateWindow();
        }
    }

    QWidget::changeEvent(event);
}





