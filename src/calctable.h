/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         calctable.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Overloaded QTableWidget class that supports colored cells.

////////////////////////////////////////////////////////////////////////////////////////////*/
#ifndef CALCTABLE
#define CALCTABLE

#include <QTableWidget>
#include <QHeaderView>
#include <QColor>
#include <QAbstractItemView>
#include <QEvent>

#include "global.h"
#include "list.h"

class CalcTable : public QTableWidget
{
    Q_OBJECT

    int  coloredCol;
    bool editable;
    List<QColor> colorTable;

public:
    explicit CalcTable(QWidget *parent,
                       int coloredColumn = 0,
                       bool isEditable   = true);

    // Change the color of the "colored column" for a given row
    void changeColor(int row, const QColor &color);

protected:
    // Hook into edit start to emit cellEditStarted (replacement for old beginEdit)
    bool edit(const QModelIndex &index,
              EditTrigger trigger,
              QEvent *event) override;

signals:
    void cellEditStarted(int row, int col);
};

#endif // CALCTABLE
