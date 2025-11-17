/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         calctable.cpp
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

////////////////////////////////////////////////////////////////////////////////////////////*/

#include "calctable.h"

#include <QTableWidgetItem>
#include <QEvent>

void CalcTable::changeColor(int row, const QColor &color)
{
    int rowNum = rowCount();

    // Keep behavior similar to original colorTable logic
    if (rowNum != colorTable.GetLen()) {
        // Original code only ever appended; we mimic that pattern
        for (int c = 0; c < rowNum; ++c)
            colorTable.NewItem(QColor(0, 0, 0));
    }

    if (row < colorTable.GetLen())
        colorTable[row] = color;

    // Apply the color to the actual cell in the colored column
    if (row < 0 || row >= rowCount())
        return;

    if (coloredCol < 0 || coloredCol >= columnCount())
        return;

    QTableWidgetItem *item = this->item(row, coloredCol);
    if (!item) {
        item = new QTableWidgetItem;
        setItem(row, coloredCol, item);
    }

    // In the original code, the QColorGroup::Text was modified → text color
    item->setForeground(color);
}

bool CalcTable::edit(const QModelIndex &index,
                     EditTrigger trigger,
                     QEvent *event)
{
    if (!index.isValid())
        return false;

    // Replaces the old beginEdit() logic:
    // if (!editable && col == coloredCol) -> don't allow editing that column
    if (!editable && index.column() == coloredCol)
        return false;

    // Notify like the old beginEdit() did with cellEditStarted(row, col)
    emit cellEditStarted(index.row(), index.column());

    return QTableWidget::edit(index, trigger, event);
}
