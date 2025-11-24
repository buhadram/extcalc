/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         tablepref.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Function table preferences dialog

////////////////////////////////////////////////////////////////////////////////////////////*/
#ifndef TABLEPREF_H
#define TABLEPREF_H

#include "global.h"

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>

class TablePreferences : public QWidget
{
    Q_OBJECT

    QComboBox *typeBox;
    QPushButton *saveButton, *cancelButton;
    QLabel *topLabel;
    QLabel *typeLabel, *xStepLabel, *zStepLabel,
           *xStartLabel, *xEndLabel,
           *zStartLabel, *zEndLabel,
           *xLabel, *zLabel, *aLabel;
    QSpinBox *xStep, *zStep;
    QLineEdit *xStart, *xEnd, *zStart, *zEnd, *aValue;
    Preferences pref;

public:
    TablePreferences(Preferences p, QWidget *parent = nullptr)
        : QWidget(parent)
    {
        pref = p;

        setWindowTitle(TABLEPREFH_STR11);
        setWindowFlags(Qt::Window);  // replaces old QWidget ctor flags

        topLabel   = new QLabel(TABLEPREFH_STR1,  this);
        typeLabel  = new QLabel(TABLEPREFH_STR2,  this);
        xLabel     = new QLabel(TABLEPREFH_STR12, this);
        zLabel     = new QLabel(TABLEPREFH_STR13, this);
        aLabel     = new QLabel(TABLEPREFH_STR14, this);
        xStartLabel= new QLabel(TABLEPREFH_STR15, this);
        xStepLabel = new QLabel(TABLEPREFH_STR16, this);
        xEndLabel  = new QLabel(TABLEPREFH_STR17, this);
        zStartLabel= new QLabel(TABLEPREFH_STR15, this);
        zEndLabel  = new QLabel(TABLEPREFH_STR17, this);
        zStepLabel = new QLabel(TABLEPREFH_STR16, this);

        // Qt6 ctor: no "editable" bool arg
        typeBox    = new QComboBox(this);

        saveButton   = new QPushButton(TABLEPREFH_STR9,  this);
        cancelButton = new QPushButton(TABLEPREFH_STR10, this);

        xStart  = new QLineEdit(this);
        xEnd    = new QLineEdit(this);
        zStart  = new QLineEdit(this);
        zEnd    = new QLineEdit(this);
        aValue  = new QLineEdit(this);

        xStep = new QSpinBox(this);
        xStep->setMinimum(1);
        xStep->setMaximum(1000);
        xStep->setSingleStep(1);

        zStep = new QSpinBox(this);
        zStep->setMinimum(1);
        zStep->setMaximum(1000);
        zStep->setSingleStep(1);

        setGeometry(0, 0, 460, 350);
        setFixedWidth(460);
        setFixedHeight(350);

        topLabel->setGeometry(10, 10, 380, 20);
        xLabel->setGeometry(10, 50, 380, 20);

        xStartLabel->setGeometry(10, 80, 75, 20);
        xStart->setGeometry(90, 80, 65, 20);
        xEndLabel->setGeometry(160, 80, 75, 20);
        xEnd->setGeometry(240, 80, 65, 20);
        xStepLabel->setGeometry(310, 80, 75, 20);
        xStep->setGeometry(390, 80, 65, 20);

        zLabel->setGeometry(10, 120, 380, 20);
        zStartLabel->setGeometry(10, 150, 75, 20);
        zStart->setGeometry(90, 150, 65, 20);
        zEndLabel->setGeometry(160, 150, 75, 20);
        zEnd->setGeometry(240, 150, 65, 20);
        zStepLabel->setGeometry(310, 150, 75, 20);
        zStep->setGeometry(390, 150, 65, 20);

        aLabel->setGeometry(10, 190, 225, 20);
        aValue->setGeometry(240, 190, 65, 20);

        typeLabel->setGeometry(10, 230, 150, 30);
        typeBox->setGeometry(160, 230, 145, 30);
        saveButton->setGeometry(70, 300, 110, 30);
        cancelButton->setGeometry(280, 300, 110, 30);

        // ⬇⬇⬇ THIS IS THE IMPORTANT FIX ⬇⬇⬇
        // Old: typeBox->insertItem(TABLEH_STR3);
        // New: use addItem(const QString&)
        typeBox->addItem(TABLEH_STR3);
        typeBox->addItem(TABLEH_STR4);
        typeBox->addItem(TABLEH_STR5);
        typeBox->addItem(TABLEH_STR6);
        typeBox->addItem(TABLEH_STR7);
        typeBox->addItem(TABLEH_STR9);

        xStart->setText(QString::number(pref.tableXStart));
        xEnd->setText(QString::number(pref.tableXEnd));
        zStart->setText(QString::number(pref.tableZStart));
        zEnd->setText(QString::number(pref.tableZEnd));
        aValue->setText(QString::number(pref.tableAValue));
        xStep->setValue(pref.tableXSteps);
        zStep->setValue(pref.tableZSteps);

        if (pref.tableType == TABLENORMAL)
            typeBox->setCurrentText(TABLEH_STR3);
        else if (pref.tableType == TABLEPOLAR)
            typeBox->setCurrentText(TABLEH_STR4);
        else if (pref.tableType == TABLEPARAMETER)
            typeBox->setCurrentText(TABLEH_STR5);
        else if (pref.tableType == TABLEINEQUALITY)
            typeBox->setCurrentText(TABLEH_STR6);
        else if (pref.tableType == TABLE3D)
            typeBox->setCurrentText(TABLEH_STR7);
        else if (pref.tableType == TABLECOMPLEX)
            typeBox->setCurrentText(TABLEH_STR9);

        // old QObject::connect(...) still works, but you can modernize later
        QObject::connect(saveButton,   SIGNAL(clicked()),
                         this,        SLOT(saveSlot()));
        QObject::connect(cancelButton, SIGNAL(clicked()),
                         this,        SLOT(close()));
    }

protected:
    virtual void windowActivationChange(bool);

public slots:
    void saveSlot();

signals:
    void prefChange(Preferences);
};

#endif // TABLEPREF_H

