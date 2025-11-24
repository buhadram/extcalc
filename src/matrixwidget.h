/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         matrixwidget.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

The class of the matrix/vector tab window.
Includes the matrix/vector window GUI and all needed math. functions.

////////////////////////////////////////////////////////////////////////////////////////////*/
#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H


#include <QWidget>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDockWidget>
#include <QMainWindow>
#include <qicon.h>
#include <QMenu>
#include <QAction>
#include <qtooltip.h>
#include <qsplitter.h>
//Added by qt3to4:
#include <QResizeEvent>
//#include <QList>
#include <QVector>
#include <QPixmap>
#include "buttons.h"
#include "calcinput.h"
#include "calctable.h"
#include "catalog.h"
#include "tabwidget.h"
#include <QFrame>





class MatrixWidget :public TabWidget
{

    CalcTable*outputTable;
    CalcTable*varTable;
    CalcTable*resultTable;
    QPushButton*sprodButton,*invertButton,*detButton;
    QComboBox*operationBox;
    QSpinBox *size1Box,*size2Box;
    QComboBox *matrixBox,*vectorBox,*typeBox;
    QLabel *matrixLabel,*vectorLabel,*size1Label,*size2Label,*label1,*label2,*label3;
    QLineEdit *input1,*input2,*input3;
    QPushButton *calcButton,*sizeButton,*catalogButton;

    QToolBar*toolBar;
//    Q3DockArea*dockArea;
    QPixmap *catalogIcon;
    Catalog *catalog;
    QSplitter *split;
    QFrame*leftFrame,*rightFrame;

    CalcInput*calcWidget;
    int currentVar;
    int state;
//    bool fullscreen;

    Q_OBJECT
    public:
    MatrixWidget(QWidget*parent,Preferences p,Variable*va,ThreadSync*td) :TabWidget(parent,p,va,td,false)
        {
//            pref=p;
//            threadData=td;
//            vars=va;
            currentVar=0;
            state=MATCALC;

            split=new QSplitter(Qt::Horizontal,this);

            varTable=new CalcTable(split,0,true);
            outputTable=new CalcTable(split,0,true);
            outputTable->setRowCount(10);
            outputTable->setColumnCount(4);


            catalog=new Catalog(CATMATHSTD | CATMATHCOMPLEX | CATMATRIX,this);
    
            catalogIcon=new QPixmap(INSTALLDIR+QString("/data/catalog.png"));
            

            toolBar=new QToolBar();
            
            setMainWidget(split);

            
            
            varTable->setRowCount(27);
            varTable->setColumnCount(3);
            QString headLine("A");
            for(int c=0; c<26; c++)
            {
                headLine[0]=(char)(c+65);
                varTable->setVerticalHeaderItem(c, new QTableWidgetItem(headLine));
            }
            varTable->setVerticalHeaderItem(26, new QTableWidgetItem("ans"));
            varTable->setHorizontalHeaderItem(0, new QTableWidgetItem(MATRIXWIDGETH_STR1));
            varTable->setHorizontalHeaderItem(1, new QTableWidgetItem(MATRIXWIDGETH_STR2));
            varTable->setHorizontalHeaderItem(2, new QTableWidgetItem(MATRIXWIDGETH_STR3));
            varTable->setSelectionMode(QAbstractItemView::SingleSelection);
            varTable->selectRow(currentVar);
            for(int row = 0; row < varTable->rowCount(); ++row) { QTableWidgetItem* item = varTable->item(row, 0); if (item) item->setFlags(item->flags() & ~Qt::ItemIsEditable); }
            setVarTable();

            operationBox=new QComboBox(toolBar);
            sprodButton=new QPushButton(getUnicode(DEGREESTRING),toolBar);
            invertButton=new QPushButton(getUnicode(8315)+getUnicode(185),toolBar);
            detButton=new QPushButton("det",toolBar);
            catalogButton=new QPushButton(*catalogIcon,"",toolBar);
      operationBox->hide();

            operationBox->insertItem(0, MATRIXWIDGETH_STR4);
            operationBox->insertItem(1, MATRIXWIDGETH_STR5);
            operationBox->insertItem(2, MATRIXWIDGETH_STR6);
            operationBox->insertItem(3, MATRIXWIDGETH_STR7);
            operationBox->insertItem(4, MATRIXWIDGETH_STR8);

            calcWidget=new CalcInput(this,m_vars,m_threadData,true);
      addSubWidget(calcWidget);
      calcWidget->setFixedHeight(160);



            size1Box=new QSpinBox(this);
            size1Box->setMinimum(1);
            size1Box->setMaximum(20);
            size1Box->setSingleStep(1);
            size2Box=new QSpinBox(this);
            size2Box->setMinimum(1);
            size2Box->setMaximum(20);
            size2Box->setSingleStep(1);
            size1Label=new QLabel(" ",this);
            size2Label=new QLabel(" ",this);
            matrixLabel=new QLabel(" ",this);
            vectorLabel=new QLabel(" ",this);
            label1=new QLabel(" ",this);
            label2=new QLabel(" ",this);
            label3=new QLabel(" ",this);
            input1=new QLineEdit(this);
            input2=new QLineEdit(this);
            input3=new QLineEdit(this);
            calcButton=new QPushButton(MATRIXWIDGETH_STR9,this);
            sizeButton=new QPushButton(MATRIXWIDGETH_STR10,this);
            resultTable=new CalcTable(this,0,true);
            matrixBox=new QComboBox(this);
            vectorBox=new QComboBox(this);
            
            QList<int> s = split->sizes();
            s.replace(0,150); split->setSizes(s);
            s[1]=450;
            split->setSizes(s);
            invertButton->setFixedWidth(30);
            detButton->setFixedWidth(30);
            sprodButton->setFixedWidth(30);
            catalogButton->setFixedWidth(30);
            
            vectorBox->insertItem(0, MATRIXWIDGETH_STR11);
            for(int c=0; c<26; c++)
            {
                headLine[0]=(char)(c+65);
                matrixBox->insertItem(c+1, headLine);
                vectorBox->insertItem(c+1, headLine);
            }
            typeBox=new QComboBox(this);
            typeBox->insertItem(0, MATRIXWIDGETH_STR12);
            typeBox->insertItem(1, MATRIXWIDGETH_STR13);
            typeBox->insertItem(2, MATRIXWIDGETH_STR14);
            typeBox->insertItem(3, MATRIXWIDGETH_STR15);
            typeBox->insertItem(4, MATRIXWIDGETH_STR16);
            typeBox->insertItem(5, MATRIXWIDGETH_STR17);
            typeBox->insertItem(6, MATRIXWIDGETH_STR18);

      setDockArea(1);

            resetInterface();



            QObject::connect(m_calcButtons, &StandardButtons::prefChange, this, &MatrixWidget::getPref);
            QObject::connect(sprodButton, &QPushButton::clicked, this, &MatrixWidget::sprodButtonSlot);
            QObject::connect(invertButton, &QPushButton::clicked, this, &MatrixWidget::invertButtonSlot);
            QObject::connect(detButton, &QPushButton::clicked, this, &MatrixWidget::detButtonSlot);
            QObject::connect(operationBox, QOverload<int>::of(&QComboBox::activated), this, &MatrixWidget::operationBoxSlot);
            QObject::connect(calcWidget, &CalcInput::prefChange, this, &MatrixWidget::getPref);
            QObject::connect(m_calcButtons, &StandardButtons::emitText, this, &MatrixWidget::buttonInputSlot);
            QObject::connect(varTable->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current){ varSelectionSlot(current.row(), current.column()); });
            QObject::connect(varTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item){ if(item) varChangedSlot(item->row(), item->column()); });
            QObject::connect(outputTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item){ if(item) outputChangedSlot(item->row(), item->column()); });
            QObject::connect(calcWidget, &CalcInput::calcSignal, this, &MatrixWidget::enterSlot);
            QObject::connect(calcButton, &QPushButton::pressed, this, &MatrixWidget::calcButtonSlot);
            QObject::connect(matrixBox, QOverload<int>::of(&QComboBox::activated), this, &MatrixWidget::matrixBoxSlot);
            QObject::connect(vectorBox, QOverload<int>::of(&QComboBox::activated), this, &MatrixWidget::vectorBoxSlot);
            QObject::connect(typeBox, QOverload<int>::of(&QComboBox::activated), this, &MatrixWidget::typeBoxSlot);
            QObject::connect(size1Box, QOverload<int>::of(&QSpinBox::valueChanged), this, &MatrixWidget::size1BoxSlot);
            QObject::connect(sizeButton, &QPushButton::pressed, this, &MatrixWidget::sizeButtonSlot);
            QObject::connect(catalog, &Catalog::menuSignal, this, &MatrixWidget::buttonInputSlot);
            QObject::connect(catalogButton, &QPushButton::clicked, this, &MatrixWidget::catalogSlot);
            
        }
    
        void setPref(Preferences);
    
        void setVarTable();
        void setOutputTable(int num);
        void resizeVar(int var,int rows,int cols);
        void resetInterface();
        void setHeader(CalcTable*);
    
    public slots:
        void getPref(Preferences);
        void sprodButtonSlot();
        void invertButtonSlot();
        void detButtonSlot();
        void operationBoxSlot(int);
        void enterSlot();
        void buttonInputSlot(QString);
        void varSelectionSlot(int,int);
        void varChangedSlot(int,int);
        void outputChangedSlot(int,int);
        void calcButtonSlot();
        void matrixBoxSlot(int);
        void vectorBoxSlot(int);
        void size1BoxSlot(int);
        void sizeButtonSlot();
        void typeBoxSlot(int);
        void dockWindowSlot();
        void catalogSlot();
    
//    protected:
//        virtual void resizeEvent(QResizeEvent*);
    
    
    signals:
        void prefChange(Preferences);
    
};

#endif
