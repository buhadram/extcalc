/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         catalog.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This is an overloaded popup menu that serves a complete function catalog.

////////////////////////////////////////////////////////////////////////////////////////////*/
#ifndef CATALOGH
#define CATALOGH

#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QDialog>

#include "global.h"

// ---------------------------------------------------------------------
// ConstantDialog (Qt6 version: uses QListWidget instead of Q3ListBox)
// ---------------------------------------------------------------------
class ConstantDialog : public QDialog
{
    Q_OBJECT

    QLabel      *constLabel;
    QLabel      *descriptionLabel;
    QLabel      *valueLabel;
    QLabel      *identifierLabel;
    QLineEdit   *descriptionLine;
    QLineEdit   *valueLine;
    QLineEdit   *identifierLine;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *addButton;
    QPushButton *removeButton;
    QListWidget *variablesList;       // was Q3ListBox
    Preferences  pref;

public:
    ConstantDialog(QWidget *parent, const QString &name, Preferences p);

public slots:
    void boxSlot();
    void applySlot();
    void removeSlot();
    void setPref(Preferences newPref);

signals:
    void prefChange(Preferences);
};

// ---------------------------------------------------------------------
// Catalog (Qt6 version: uses addMenu/addAction + QAction::data)
// ---------------------------------------------------------------------
class Catalog : public QMenu
{
    Q_OBJECT

    QMenu *mathStandard;
    QMenu *mathTrigonometric;
    QMenu *mathExtended;
    QMenu *mathLogic;
    QMenu *matrix;
    QMenu *scriptStandard;
    QMenu *scriptText;
    QMenu *scriptGraphics;
    QMenu *scriptGL;
    QMenu *scriptFile;
    QMenu *constantsMath;
    QMenu *constantsPhysics;
    QMenu *constantsConv;
    QMenu *constantsUser;

    Preferences pref;
    int mathConstLen;
    int physicsConstLen;
    int convConstLen;
    int state;
    ConstantDialog *cDialog;

public:
    explicit Catalog(int st, QWidget *parent, Preferences *p = nullptr)
    : QMenu(parent),
    mathStandard(nullptr),
    mathTrigonometric(nullptr),
    mathExtended(nullptr),
    mathLogic(nullptr),
    matrix(nullptr),
    scriptStandard(nullptr),
    scriptText(nullptr),
    scriptGraphics(nullptr),
    scriptGL(nullptr),
    scriptFile(nullptr),
    constantsMath(nullptr),
    constantsPhysics(nullptr),
    constantsConv(nullptr),
    constantsUser(nullptr),
    mathConstLen(0),
    physicsConstLen(0),
    convConstLen(0),
    state(st),
    cDialog(nullptr)
    {
        if (p == nullptr) {
            if (state & CATCONSTANTS)
                state &= ~CATCONSTANTS;
            pref.constList = nullptr;
        } else {
            pref = *p;
        }

        // --- Create submenus ---
        mathStandard      = new QMenu(this);
        mathTrigonometric = new QMenu(this);
        mathExtended      = new QMenu(this);
        mathLogic         = new QMenu(this);
        matrix            = new QMenu(this);
        scriptStandard    = new QMenu(this);
        scriptText        = new QMenu(this);
        scriptGraphics    = new QMenu(this);
        scriptGL          = new QMenu(this);
        scriptFile        = new QMenu(this);

        constantsMath     = new QMenu(this);
        constantsPhysics  = new QMenu(this);
        constantsConv     = new QMenu(this);
        constantsUser     = new QMenu(this);

        cDialog = new ConstantDialog(parent, QStringLiteral("Modify Constants"), pref);

        // Helper lambda: create an action with a numeric ID
        auto addItemWithId = [](QMenu *menu, const QString &text, int id) {
            QAction *act = menu->addAction(text);
            act->setData(id);
            return act;
        };

        // ---- Constants menus ----
        if (state & CATCONSTANTS) {
            addMenu(constantsMath)->setText(tr("Mathematics Constants"));
            addMenu(constantsPhysics)->setText(tr("Physics Constants"));
            addMenu(constantsConv)->setText(tr("Conversation Constants"));
            addMenu(constantsUser)->setText(tr("Userdefined Constants"));
        }

        // ---- Math standard / trig / extended ----
        if (state & CATMATHSTD) {
            // Math Standard
            addItemWithId(mathStandard, "+",   1);
            addItemWithId(mathStandard, "-",   2);
            addItemWithId(mathStandard, "*",   3);
            addItemWithId(mathStandard, "/",   4);
            addItemWithId(mathStandard, "^2",  5);
            addItemWithId(mathStandard, "^3",  6);
            addItemWithId(mathStandard, "^-1", 7);
            addItemWithId(mathStandard, "^",   8);
            addItemWithId(mathStandard, "sqrt",9);
            addItemWithId(mathStandard, "curt",10);
            addItemWithId(mathStandard, "root",11);
            addItemWithId(mathStandard, "log", 12);
            addItemWithId(mathStandard, "ln",  13);
            addItemWithId(mathStandard, "set ->",14);
            addItemWithId(mathStandard, "set =", 15);

            // Math Trigonometric
            addItemWithId(mathTrigonometric, "sin",  1);
            addItemWithId(mathTrigonometric, "cos",  2);
            addItemWithId(mathTrigonometric, "tan",  3);
            addItemWithId(mathTrigonometric, "asin", 4);
            addItemWithId(mathTrigonometric, "acos", 5);
            addItemWithId(mathTrigonometric, "atan", 6);
            addItemWithId(mathTrigonometric, "sinh", 7);
            addItemWithId(mathTrigonometric, "cosh", 8);
            addItemWithId(mathTrigonometric, "tanh", 9);
            addItemWithId(mathTrigonometric, "asinh",10);
            addItemWithId(mathTrigonometric, "acosh",11);
            addItemWithId(mathTrigonometric, "atanh",12);

            // Math Extended
            addItemWithId(mathExtended, "abs",        1);
            addItemWithId(mathExtended, "faculty !",  6);
            addItemWithId(mathExtended, "modulo %",   7);
            addItemWithId(mathExtended, "rnd",        8);
            addItemWithId(mathExtended, "d/dx",       9);
            addItemWithId(mathExtended, "integral",  10);

            // attach submenus to this menu
            addMenu(mathStandard)->setText(tr("Math Standard"));
            addMenu(mathTrigonometric)->setText(tr("Math Trigonometric"));
            addMenu(mathExtended)->setText(tr("Math Extended"));
        }

        // ---- Math Logic ----
        if (state & CATMATHLOGIC) {
            addItemWithId(mathLogic, "logic and &&&&", 1);
            addItemWithId(mathLogic, "logic or ||",    2);
            addItemWithId(mathLogic, "logic not !",    3);
            addItemWithId(mathLogic, "==",             4);
            addItemWithId(mathLogic, ">=",             5);
            addItemWithId(mathLogic, "<=",             6);
            addItemWithId(mathLogic, "!=",             7);
            addItemWithId(mathLogic, "<",              8);
            addItemWithId(mathLogic, ">",              9);
            addItemWithId(mathLogic, "binary and &&", 10);
            addItemWithId(mathLogic, "binary or |",  11);
            addItemWithId(mathLogic, "binary not ~", 12);
            addItemWithId(mathLogic, "left shift <<",13);
            addItemWithId(mathLogic, "right shift >>",14);

            addMenu(mathLogic)->setText(tr("Math Logic"));
        }

        // ---- Complex operations ----
        if (state & CATMATHCOMPLEX) {
            addItemWithId(mathExtended, "arg",   2);
            addItemWithId(mathExtended, "conj",  3);
            addItemWithId(mathExtended, "real",  4);
            addItemWithId(mathExtended, "imag",  5);
        }

        // ---- Matrix ----
        if (state & CATMATRIX) {
            addItemWithId(matrix, "[]",   1);
            addItemWithId(matrix, "sprod",2);
            addItemWithId(matrix, "^-1",  3);

            addMenu(matrix)->setText(tr("Matrix/Vector"));
        }

        // ---- Script menus ----
        if (state & CATSCRIPT) {
            // Script Standard
            addItemWithId(scriptStandard, "if",        1);
            addItemWithId(scriptStandard, "for",       2);
            addItemWithId(scriptStandard, "while",     3);
            addItemWithId(scriptStandard, "break",     4);
            addItemWithId(scriptStandard, "continue",  5);
            addItemWithId(scriptStandard, "stop",      6);
            addItemWithId(scriptStandard, "sleep",     7);
            addItemWithId(scriptStandard, "run",       8);
            addItemWithId(scriptStandard, "(bool)",    9);
            addItemWithId(scriptStandard, "(int)",    10);
            addItemWithId(scriptStandard, "(float)",  11);
            addItemWithId(scriptStandard, "(string)", 12);
            addItemWithId(scriptStandard, ";",        13);
            addItemWithId(scriptStandard, "{",        14);
            addItemWithId(scriptStandard, "}",        15);

            // Script Text
            addItemWithId(scriptText, "print",     1);
            addItemWithId(scriptText, "clear",     2);
            addItemWithId(scriptText, "setcursor", 3);
            addItemWithId(scriptText, "getline",   4);
            addItemWithId(scriptText, "getkey",    5);
            addItemWithId(scriptText, "keystate",  6);

            // Script Graphics
            addItemWithId(scriptGraphics, "drawpoint",  1);
            addItemWithId(scriptGraphics, "drawline",   2);
            addItemWithId(scriptGraphics, "drawrect",   3);
            addItemWithId(scriptGraphics, "drawcircle", 4);
            addItemWithId(scriptGraphics, "drawstring", 5);
            addItemWithId(scriptGraphics, "drawcolor",  6);
            addItemWithId(scriptGraphics, "drawclear",  7);

            // Script GL
            addItemWithId(scriptGL, "glbegin",        1);
            addItemWithId(scriptGL, "glendlist",      2);
            addItemWithId(scriptGL, "glend",          3);
            addItemWithId(scriptGL, "glshow",         4);
            addItemWithId(scriptGL, "glclear",        5);
            addItemWithId(scriptGL, "glloadidentity", 6);
            addItemWithId(scriptGL, "glstartlist",    7);
            addItemWithId(scriptGL, "glcalllist",     8);
            addItemWithId(scriptGL, "glpoint",        9);
            addItemWithId(scriptGL, "glscale",       10);
            addItemWithId(scriptGL, "glmove",        11);
            addItemWithId(scriptGL, "glrotate",      12);
            addItemWithId(scriptGL, "glcolor",       13);
            addItemWithId(scriptGL, "glstring",      14);

            // Script File
            addItemWithId(scriptFile, "readfile",   1);
            addItemWithId(scriptFile, "writefile",  2);
            addItemWithId(scriptFile, "removefile", 3);
            addItemWithId(scriptFile, "appendfile", 4);

            addMenu(scriptStandard)->setText(tr("Script Standard"));
            addMenu(scriptText)->setText(tr("Script Text"));
            addMenu(scriptGraphics)->setText(tr("Script Graphics"));
            addMenu(scriptGL)->setText(tr("Script GL"));
            addMenu(scriptFile)->setText(tr("Script File"));
        }

        // ---- Connections (Qt5/6: triggered(QAction*)) ----
        auto connectMenu = [this](QMenu *menu, auto slotFunc) {
            QObject::connect(menu, &QMenu::triggered,
                             this, [this, slotFunc](QAction *act) {
                                 bool ok = false;
                                 int id = act->data().toInt(&ok);
                                 if (ok)
                                     (this->*slotFunc)(id);
                             });
        };

        connectMenu(mathStandard,       &Catalog::mathStandardSlot);
        connectMenu(mathTrigonometric,  &Catalog::mathTrigonometricSlot);
        connectMenu(mathExtended,       &Catalog::mathExtendedSlot);
        connectMenu(mathLogic,          &Catalog::mathLogicSlot);
        connectMenu(matrix,             &Catalog::matrixSlot);
        connectMenu(scriptStandard,     &Catalog::scriptStandardSlot);
        connectMenu(scriptText,         &Catalog::scriptTextSlot);
        connectMenu(scriptGraphics,     &Catalog::scriptGraphicsSlot);
        connectMenu(scriptGL,           &Catalog::scriptGLSlot);
        connectMenu(scriptFile,         &Catalog::scriptFileSlot);
        connectMenu(constantsPhysics,   &Catalog::constantsSlot);
        connectMenu(constantsMath,      &Catalog::constantsSlot);
        connectMenu(constantsConv,      &Catalog::constantsSlot);
        connectMenu(constantsUser,      &Catalog::constantsSlot);

        QObject::connect(cDialog, &ConstantDialog::prefChange,
                         this,    &Catalog::getPref);
    }

    void setPref(Preferences newPref);

public slots:
    void mathStandardSlot(int);
    void mathTrigonometricSlot(int);
    void mathExtendedSlot(int);
    void mathLogicSlot(int);

    void matrixSlot(int);

    void scriptStandardSlot(int);
    void scriptTextSlot(int);
    void scriptGraphicsSlot(int);
    void scriptGLSlot(int);
    void scriptFileSlot(int);

    void constantsSlot(int);

    void getPref(Preferences pref) { emit prefChange(pref); }

signals:
    void menuSignal(QString);
    void prefChange(Preferences);
};

#endif // CATALOGH
