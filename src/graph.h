/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         graph.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Class of the graphics tab window

////////////////////////////////////////////////////////////////////////////////////////////*/

#ifndef GRAPHH
#define GRAPHH

#include <stdio.h>
#include <stdlib.h>
#include <qapplication.h>
#include <QWidget>
#include <qpainter.h>
#include <qmessagebox.h>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtOpenGL>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qclipboard.h>
#include <qapplication.h>
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
#include <math.h>
#include "list.h"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "functiontable.h"
#include "graphout.h"
#include "buttons.h"
#include "catalog.h"
#include "tabwidget.h"
#include "screenshotdialog.h"


class GraphArea :public QWidget
{
    Q_OBJECT
            
    public:
    GraphArea(QWidget*parent) :QWidget(parent)
    {
    ;
    }
    
    protected:
    virtual void resizeEvent(QResizeEvent*)
    {
        emit sizeChanged();
    }
    
    signals:
    void sizeChanged();    
};





class GraphWidget :public TabWidget
{
    GraphOutput*graph;
    FunctionTable* functionTable;
    GraphArea*graphArea;

    QLineEdit *inputLine;

//    GraphSolveWidget *solveWidget;
  ScreenshotDialog *screenshotDialog;

    QToolBar*toolBar;
    QComboBox *solveType,*functionType,*modeBox;
    QPixmap *minimizeIcon,*maximizeIcon,*printIcon,*catalogIcon;
    Catalog *catalog;
    QPushButton *drawButton,*maximizeButton,*catalogButton;
    QSplitter *horzSplit,*vertSplit;

    bool solveMode;
    int changedRow;
    bool functionChanged;
    bool dynamicStart;
    int menuBottom;
    bool processStarted;
    
Q_OBJECT
    public:
  GraphWidget(QWidget*parent,Preferences pr,Variable*va,ThreadSync*td,ScreenshotDialog *sd) :TabWidget(parent,pr,va,td,false)
    {
        solveMode=false;
        functionChanged=false;
        changedRow=-1;
        dynamicStart=false;
        processStarted=false;


        horzSplit=new QSplitter(Qt::Horizontal,this);
        vertSplit=new QSplitter(Qt::Vertical,horzSplit);
        functionTable=new FunctionTable((QWidget*)vertSplit,pref_);
        graphArea=new GraphArea(horzSplit);
        graph=new GraphOutput(graphArea,vars_,threadData_);
        catalog=new Catalog(CATMATHSTD | CATMATHCOMPLEX,this);
		screenshotDialog=sd;
        
        setMainWidget(horzSplit);
        addSubWidget(calcButtons_);
        addSubWidget(extButtons);
        setDockArea(1);
        
        minimizeIcon=new QPixmap(INSTALLDIR+QString("/data/view_top_bottom.png"));
        maximizeIcon=new QPixmap(INSTALLDIR+QString("/data/view_remove.png"));
        printIcon=new QPixmap(INSTALLDIR+QString("/data/print.png"));
        catalogIcon=new QPixmap(INSTALLDIR+QString("/data/catalog.png"));

        toolBar=new QToolBar();

        drawButton=new QPushButton(*printIcon,GRAPHH_STR1,toolBar);
        maximizeButton=new QPushButton(*maximizeIcon,"",toolBar);
        modeBox=new QComboBox(toolBar);
//    modeBox->hide();
        catalogButton=new QPushButton(*catalogIcon,"",toolBar);
        catalogButton->setFixedWidth(30);
        maximizeButton->setFixedWidth(30);


        drawButton->setFixedHeight(25);

        solveType=new QComboBox(toolBar);
        functionType=new QComboBox(this);
//        solveWidget=new GraphSolveWidget(this,pref_,vars,threadData);
//    screenshotDialog=new ScreenshotDialog(parent);
//    screenshotDialog->hide();

        solveType->hide();
        functionType->hide();
//        solveWidget->hide();
        inputLine=new QLineEdit(vertSplit);
        inputLine->setFixedHeight(25);
        QList<int> s = horzSplit->sizes();
        s.replace(1,300); horzSplit->setSizes(s);
        s[0]=300;
        horzSplit->setSizes(s);
        
        
        

        solveType->insertItem(0, GRAPHH_STR4, QVariant(-2));
        solveType->insertItem(1, GRAPHH_STR5, QVariant(-3));
        solveType->insertItem(2, GRAPHH_STR6, QVariant(-3));
        solveType->insertItem(3, GRAPHH_STR7, QVariant(-4));
        solveType->insertItem(4, GRAPHH_STR8, QVariant(-5));
        solveType->insertItem(5, GRAPHH_STR9, QVariant(-6));
        solveType->insertItem(6, GRAPHH_STR10, QVariant(-7));
        solveType->insertItem(7, GRAPHH_STR11, QVariant(-8));
        solveType->insertItem(8, GRAPHH_STR26, QVariant(-9));
        
        functionType->insertItem(0, TABLEH_STR3);
        functionType->insertItem(1, TABLEH_STR4);
        functionType->insertItem(0, TABLEH_STR5);
        functionType->insertItem(1, TABLEH_STR6);
        functionType->insertItem(2, TABLEH_STR7);
        functionType->insertItem(3, GRAPHH_STR30);
		modeBox->insertItem(0, tr("Edit"));
		modeBox->insertItem(1, tr("Screenshot"));
        modeBox->setCurrentIndex(0);


        QObject::connect(functionTable, &FunctionTable::currentChanged, this, &GraphWidget::selectionChangedSlot);
        QObject::connect(functionTable, &FunctionTable::textEditStarted, this, &GraphWidget::tableEditSlot);
        QObject::connect(drawButton, &QPushButton::released, graph, &GraphOutput::resetRotation);
        QObject::connect(drawButton, &QPushButton::released, graph, &GraphOutput::removeLines);
        QObject::connect(drawButton, &QPushButton::released, this, &GraphWidget::drawSlot);
        QObject::connect(modeBox, &QComboBox::activated, this, &GraphWidget::modeSlot);
        QObject::connect(calcButtons_, &StandardButtons::emitText, this, &GraphWidget::buttonInputSlot);
        QObject::connect(extButtons, &ExtButtons::emitText, this, &GraphWidget::buttonInputSlot);
        QObject::connect(calcButtons_, &StandardButtons::prefChange, this, &GraphWidget::getPref);
        QObject::connect(extButtons, &ExtButtons::prefChange, this, &GraphWidget::getPref);
        QObject::connect(graph, &GraphOutput::prefChange, this, &GraphWidget::getPref);
        QObject::connect(functionTable, &FunctionTable::prefChange, this, &GraphWidget::getPref);
//        QObject::connect(solveWidget,SIGNAL(prefChange(Preferences)),this,SLOT(getPref(Preferences)));
        QObject::connect(inputLine, &QLineEdit::returnPressed, this, &GraphWidget::inputTextFinished);
        QObject::connect(inputLine, &QLineEdit::textChanged, this, &GraphWidget::inputTextChanged);
//        QObject::connect(graph,SIGNAL(leftMButtonPressed(double,double)),solveWidget,SLOT(graphLMButtonPressed(double,double)));
//        QObject::connect(solveWidget,SIGNAL(addHorizontalLine(double)),graph,SLOT(drawHorizontalLine(double)));
//        QObject::connect(solveWidget,SIGNAL(addVerticalLine(double)),graph,SLOT(drawVerticalLine(double)));
//        QObject::connect(solveWidget,SIGNAL(addPolarLine(double)),graph,SLOT(drawPolarLine(double)));
//        QObject::connect(solveWidget,SIGNAL(addCircle(double)),graph,SLOT(drawCircle(double)));
//        QObject::connect(solveWidget,SIGNAL(add3dXLine(double,double)),graph,SLOT(draw3dXLine(double,double)));
//        QObject::connect(solveWidget,SIGNAL(add3dYLine(double,double)),graph,SLOT(draw3dYLine(double,double)));
//        QObject::connect(solveWidget,SIGNAL(add3dZLine(double,double)),graph,SLOT(draw3dZLine(double,double)));
//        QObject::connect(solveWidget,SIGNAL(removeLines()),graph,SLOT(removeLines()));
        QObject::connect(solveType, &QComboBox::activated, this, &GraphWidget::solveTypeSlot);
//        QObject::connect(this,SIGNAL(solveTypeSignal(int)),solveWidget,SLOT(setState(int)));
//        QObject::connect(functionType,SIGNAL(activated(int)),solveWidget,SLOT(setFunctionType(int)));
        QObject::connect(functionType, &QComboBox::activated, this, &GraphWidget::functionTypeSlot);
//        QObject::connect(solveWidget,SIGNAL(drawInequalityIntersection(int, int)),this,SLOT(inequalitySlot(int,int)));
		QObject::connect(screenshotDialog, &ScreenshotDialog::redrawGraphs, this, &GraphWidget::drawSlot);
        QObject::connect(graph, &GraphOutput::redrawSignal, this, &GraphWidget::drawSlot);
		QObject::connect(graph, &GraphOutput::screenshotSignal, screenshotDialog, &ScreenshotDialog::screenshotSlot);
		QObject::connect(screenshotDialog, &ScreenshotDialog::getScreenshotSignal, graph, &GraphOutput::screenshotSlot);
		QObject::connect(screenshotDialog, &ScreenshotDialog::drawSignal, graph, &GraphOutput::drawSlot);
        QObject::connect(this, &GraphWidget::drawPointsSignal, graph, &GraphOutput::drawPoints);
        QObject::connect(this, &GraphWidget::removeLinesSignal, graph, &GraphOutput::removeLines);
        QObject::connect(graph, &GraphOutput::statisticsRedrawSignal, this, &GraphWidget::statisticsRedrawSignal);
        QObject::connect(catalog, &Catalog::menuSignal, this, &GraphWidget::buttonInputSlot);
        QObject::connect(catalogButton, &QPushButton::clicked, this, &GraphWidget::catalogSlot);
        QObject::connect(graphArea, &GraphArea::sizeChanged, this, &GraphWidget::graphSizeSlot);
        QObject::connect(graph, &GraphOutput::processingFinished, this, &GraphWidget::graphProcessingFinishedSlot);

    }
    
void setPref(Preferences newPref)
{
    pref_=newPref;
    if(modeBox->currentIndex()!=2)
    {
        if(!(functionType->currentIndex() == 0 ||
                functionType->currentIndex() == 2 ||
                functionType->currentIndex() == 3) && pref_.graphType==GRAPHSTD)
        {
//            solveWidget->setFunctionType(0);
            functionType->setCurrentIndex(0);
            functionTypeSlot(0);
        }
        else if(functionType->currentIndex() != 1 && pref_.graphType==GRAPHPOLAR)
        {
//            solveWidget->setFunctionType(1);
            functionType->setCurrentIndex(1);
            functionTypeSlot(1);
        }
        else if(functionType->currentIndex() != 4 && pref_.graphType==GRAPH3D)
        {
//            solveWidget->setFunctionType(4);
            functionType->setCurrentIndex(4);
            functionTypeSlot(4);
        }
    }
    
    

    graph->setPref(pref_);
    calcButtons_->setPref(pref_);
    extButtons->setPref(pref_);
//    solveWidget->setPref(pref_);
    functionTable->setPref(pref_);
    

    

    graphSizeSlot();

}
void getUIState(int*sizes)
{
    for(int c=0; c<6; c++)
        sizes[c]=functionTable->columnWidth(c);
}
void setUIState(int*sizes)
{
    for(int c=0; c<6; c++)
        functionTable->setColumnWidth(c,sizes[c]);
}


public slots:
    
    void selectionChangedSlot(int row,int col);
    void tableEditSlot(QString);
    void drawSlot();
    void modeSlot(int);
    void inputTextChanged(const QString&);
    void inputTextFinished();
    void buttonInputSlot(QString);
    void solveTypeSlot(int);
    void editSlot(int);
    void catalogSlot();
    void graphSizeSlot();
    void dockWindowSlot();
    void inequalitySlot(int,int);
    void graphProcessingFinishedSlot();
    void getPref(Preferences newPref)
    {
        emit prefChange(newPref);
    }
    void functionTypeSlot(int fType);


protected:
//    virtual void resizeEvent(QResizeEvent*);

signals:
    void prefChange(Preferences);
    void solveTypeSignal(int);
    void drawPointsSignal(long double*,int,bool);
    void removeLinesSignal();
    void statisticsRedrawSignal();
    
};


#endif


