/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         calcpref.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Calculator preferences dialog class

////////////////////////////////////////////////////////////////////////////////////////////*/

#ifndef CALCPREF_H
#define CALCPREF_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QEvent>

#include "global.h"

class CalcPreferences : public QWidget
{
    Q_OBJECT

    QComboBox *angleBox;
    QComboBox *outputBox;
    QComboBox *calcModeBox;
    QComboBox *baseBox;
    QSpinBox  *outputLenBox;
    QPushButton *saveButton;
    QPushButton *cancelButton;
    QCheckBox *complexBox;
    QLabel *angleLabel;
    QLabel *outputLabel;
    QLabel *outputLenLabel;
    QLabel *calcModeLabel;
    QLabel *baseLabel;
    Preferences pref;

public:
    explicit CalcPreferences(const Preferences &p, QWidget *parent = nullptr)
    : QWidget(parent, Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    angleBox(nullptr),
    outputBox(nullptr),
    calcModeBox(nullptr),
    baseBox(nullptr),
    outputLenBox(nullptr),
    saveButton(nullptr),
    cancelButton(nullptr),
    complexBox(nullptr),
    angleLabel(nullptr),
    outputLabel(nullptr),
    outputLenLabel(nullptr),
    calcModeLabel(nullptr),
    baseLabel(nullptr),
    pref(p)
    {
        setWindowTitle(QStringLiteral("Calculator Preferences"));

        saveButton   = new QPushButton(CALCPREFH_STR1, this);
        cancelButton = new QPushButton(CALCPREFH_STR2, this);

        angleBox    = new QComboBox(this);
        outputBox   = new QComboBox(this);
        calcModeBox = new QComboBox(this);
        baseBox     = new QComboBox(this);

        outputLenBox = new QSpinBox(this);
        outputLenBox->setRange(2, pref.precision);
        outputLenBox->setSingleStep(1);

        angleLabel     = new QLabel(CALCPREFH_STR3, this);
        outputLabel    = new QLabel(CALCPREFH_STR4, this);
        outputLenLabel = new QLabel(CALCPREFH_STR5, this);
        calcModeLabel  = new QLabel(CALCPREFH_STR6, this);
        baseLabel      = new QLabel(CALCPREFH_STR7, this);

        complexBox = new QCheckBox(CALCPREFH_STR13, this);

        // Fixed size, keeping original pixel-based layout
        setGeometry(0, 0, 400, 290);
        setFixedSize(400, 290);

        angleLabel->setGeometry(10, 10, 150, 20);
        angleBox->setGeometry(160, 10, 120, 30);

        outputLabel->setGeometry(10, 50, 150, 20);
        outputBox->setGeometry(160, 50, 230, 30);

        outputLenLabel->setGeometry(10, 90, 150, 20);
        outputLenBox->setGeometry(160, 90, 80, 30);

        calcModeLabel->setGeometry(10, 130, 150, 20);
        calcModeBox->setGeometry(160, 130, 160, 30);

        complexBox->setGeometry(160, 165, 300, 20);

        baseLabel->setGeometry(10, 200, 150, 20);
        baseBox->setGeometry(160, 200, 120, 30);

        saveButton->setGeometry(90, 250, 100, 30);
        cancelButton->setGeometry(210, 250, 100, 30);

        // Fill combo boxes (Qt5/Qt6: addItem)
        angleBox->addItem("DEG");
        angleBox->addItem("RAD");
        angleBox->addItem("GRA");

        outputBox->addItem(CALCPREFH_STR8);
        outputBox->addItem(CALCPREFH_STR9);
        outputBox->addItem(CALCPREFH_STR10);

        calcModeBox->addItem(CALCPREFH_STR11); // SCIENTIFIC
        calcModeBox->addItem(CALCPREFH_STR12); // BASE

        baseBox->addItem("hex");
        baseBox->addItem("dec");
        baseBox->addItem("oct");
        baseBox->addItem("bin");

        complexBox->setChecked(pref.complex);

        // Initial selections from pref
        switch (pref.angle) {
            case DEG: angleBox->setCurrentIndex(0); break;
            case RAD: angleBox->setCurrentIndex(1); break;
            case GRA: angleBox->setCurrentIndex(2); break;
            default:  angleBox->setCurrentIndex(0); break;
        }

        switch (pref.outputType) {
            case FIXEDNUM:     outputBox->setCurrentIndex(0); break;
            case VARIABLENUM:  outputBox->setCurrentIndex(1); break;
            case EXPSYM:       outputBox->setCurrentIndex(2); break;
            default:           outputBox->setCurrentIndex(0); break;
        }

        switch (pref.calcType) {
            case SCIENTIFIC: calcModeBox->setCurrentIndex(0); break;
            case BASE:       calcModeBox->setCurrentIndex(1); break;
            default:         calcModeBox->setCurrentIndex(0); break;
        }

        switch (pref.base) {
            case HEX: baseBox->setCurrentIndex(0); break;
            case DEC: baseBox->setCurrentIndex(1); break;
            case OCT: baseBox->setCurrentIndex(2); break;
            case BIN: baseBox->setCurrentIndex(3); break;
            default:  baseBox->setCurrentIndex(1); break; // default to dec
        }

        outputLenBox->setValue(pref.outputLength);

        // Connections
        QObject::connect(saveButton,   &QPushButton::clicked,
                         this,        &CalcPreferences::saveSlot);
        QObject::connect(cancelButton, &QPushButton::clicked,
                         this,        &QWidget::close);
    }

protected:
    // Replacement for old windowActivationChange(bool)
    void changeEvent(QEvent *event) override;

public slots:
    void saveSlot();

signals:
    void prefChange(Preferences);
};

#endif // CALCPREF_H
