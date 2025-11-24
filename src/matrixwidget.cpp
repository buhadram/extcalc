/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         matrixwidget.cpp
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.


////////////////////////////////////////////////////////////////////////////////////////////*/
#include "matrixwidget.h"
//Added by qt3to4:
#include <QResizeEvent>
#include <QTableWidgetItem>
#include <QDebug>
#include <QByteArray>

/*
void MatrixWidget::resizeEvent(QResizeEvent*)
{
    int width=geometry().right() - geometry().left();
    int height=geometry().bottom() - geometry().top();

    
//    varTable->setGeometry(20,50,width/4-30,height-290);
//    outputTable->setGeometry(width/4+10,50,3*width/4-30,height-290);
    split->setGeometry(20,50,width-40,height-290);

//    operationBox->setGeometry(320,height-220,145,35);
//    sprodButton->setGeometry(490,height-220,35,35);
//    invertButton->setGeometry(535,height-220,35,35);
//    detButton->setGeometry(580,height-220,35,35);
    dockArea->setGeometry(320,height-220,width-340,35);

    switch(state)
    {
        case MATCALC:
            standardButtons->setGeometry(20,height-220,280,200);
            calcWidget->setGeometry(320,height-180,width-340,160);
            break;
        case MATLSE:
            matrixLabel->setGeometry(20,height-220,140,20);
            vectorLabel->setGeometry(160,height-220,140,20);
            matrixBox->setGeometry(20,height-190,120,25);
            vectorBox->setGeometry(160,height-190,120,25);
            size1Label->setGeometry(20,height-155,100,20);
            size1Box->setGeometry(140,height-155,85,20);
            sizeButton->setGeometry(230,height-155,50,20);
            size2Label->setGeometry(20,height-135,280,80);
            calcButton->setGeometry(180,height-45,100,25);
            resultTable->setGeometry(320,height-180,width-340,160);
            break;
        case MATGENERATE:
            calcWidget->setGeometry(320,height-180,width-340,160);
            vectorLabel->setGeometry(20,height-220,140,20);
            matrixLabel->setGeometry(160,height-220,140,20);
            typeBox->setGeometry(20,height-190,120,25);
            matrixBox->setGeometry(160,height-190,120,25);
            size1Label->setGeometry(20,height-155,100,20);
            size1Box->setGeometry(140,height-155,85,20);
            sizeButton->setGeometry(230,height-155,50,20);
            label1->setGeometry(20,height-130,140,20);
            input1->setGeometry(160,height-130,120,20);
            label2->setGeometry(20,height-105,140,20);
            input2->setGeometry(160,height-105,120,20);
            label3->setGeometry(20,height-80,140,20);
            input3->setGeometry(160,height-80,120,20);
            calcButton->setGeometry(180,height-45,100,25);
            break;
        case MATANALYSE:
            matrixLabel->setGeometry(20,height-220,120,25);
            matrixBox->setGeometry(160,height-220,120,25);
            size2Label->setGeometry(20,height-190,240,25);
            label1->setGeometry(20,height-160,140,20);
            input1->setGeometry(160,height-160,120,20);
            label2->setGeometry(20,height-135,140,20);
            input2->setGeometry(160,height-135,120,20);
            label3->setGeometry(20,height-110,140,20);
            input3->setGeometry(160,height-110,120,20);
            vectorLabel->setGeometry(320,height-180,width-340,20);
            resultTable->setGeometry(320,height-155,width-340,135);
            calcButton->setGeometry(180,height-45,100,25);
            break;
        case MATINV:
            matrixLabel->setGeometry(20,height-220,120,25);
            matrixBox->setGeometry(140,height-220,140,25);
            size1Label->setGeometry(20,height-180,100,20);
            size1Box->setGeometry(140,height-180,85,20);
            sizeButton->setGeometry(230,height-180,50,20);
            size2Label->setGeometry(20,height-150,280,80);
            calcButton->setGeometry(180,height-45,100,25);
            resultTable->setGeometry(320,height-180,width-340,160);
            break;
    }
}
*/

void MatrixWidget::setVarTable()
{
    for(int c=0; c<27; c++)
    {
        if(m_threadData->dimension[c][0]==1 && m_threadData->dimension[c][1]==1)
            varTable->setText(c,0,"s");
        else if(m_threadData->dimension[c][1]==1)
            varTable->setText(c,0,"v");
        else varTable->setText(c,0,"m");
        
        varTable->setText(c,1,QString::number(m_threadData->dimension[c][0]));
        varTable->setText(c,2,QString::number(m_threadData->dimension[c][1]));
    }
    varTable->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
    varTable->setHorizontalHeaderItem(1, new QTableWidgetItem(""));
    varTable->setHorizontalHeaderItem(2, new QTableWidgetItem(""));
    varTable->adjustColumn(0);
    varTable->adjustColumn(1);
    varTable->adjustColumn(2);
    varTable->setHorizontalHeaderItem(0, new QTableWidgetItem(MATRIXWIDGETH_STR1));
    varTable->setHorizontalHeaderItem(1, new QTableWidgetItem(MATRIXWIDGETH_STR2));
    varTable->setHorizontalHeaderItem(2, new QTableWidgetItem(MATRIXWIDGETH_STR3));
    
    setOutputTable(currentVar);
    varTable->clearSelection();
    varTable->selectRow(currentVar);
}


void MatrixWidget::setOutputTable(int num)
{
    outputTable->setRowCount(m_threadData->dimension[num][0]);
    outputTable->setColumnCount(m_threadData->dimension[num][1]);
    setHeader(outputTable);
    int effIndex;
    for(int c1=0; c1<m_threadData->dimension[num][0]; c1++)
    {
        for(int c2=0; c2<m_threadData->dimension[num][1]; c2++)
        {
            effIndex=c2*m_threadData->dimension[num][0]+c1;
            if(effIndex<m_threadData->numlen[num])
                outputTable->setText(c1,c2,formatOutput(m_threadData->vars[num][effIndex],&m_pref));
            else outputTable->setText(c1,c2,"invalid");
            if(c1==m_threadData->dimension[num][0]-1)
                outputTable->adjustColumn(c2);
        }
    }
    
}

void MatrixWidget::resizeVar(int var,int rows,int cols)
{
    int newlen=rows*cols;
    int oldDimension1=m_threadData->dimension[var][0],oldDimension2=m_threadData->dimension[var][1];

    if(oldDimension1==rows)
    {
        m_threadData->vars[var]=(Number*)realloc((void*)m_threadData->vars[var],sizeof(Number)*newlen);
        m_threadData->numlen[var]=newlen;
        for(int c=m_threadData->numlen[var]; c<newlen; c++)
        {
            m_threadData->vars[var][c].type=NNONE;
            m_threadData->vars[var][c].cval=nullptr;
        }
    }

    m_threadData->dimension[var][0]=rows;
    m_threadData->dimension[var][1]=cols;

    int oldEffIndex,newEffIndex;

    if(m_threadData->dimension[var][0]>oldDimension1)
    {
        
        m_threadData->vars[var]=(Number*)realloc((void*)m_threadData->vars[var],sizeof(Number)*newlen);
        for(int c=m_threadData->numlen[var]; c<newlen; c++)
        {
            m_threadData->vars[var][c].type=NNONE;
            m_threadData->vars[var][c].cval=nullptr;
        }
        m_threadData->numlen[var]=newlen;
        for(int c=oldDimension2-1; c>=1; c--)
        {
            for(int c1=oldDimension1-1; c1>=0; c1--)
            {
                            
                oldEffIndex=c1+c*oldDimension1;
                newEffIndex=c1+c*m_threadData->dimension[var][0];
                if(oldEffIndex<m_threadData->numlen[var] &&newEffIndex<m_threadData->numlen[var])
                    memcpy(&m_threadData->vars[var][newEffIndex],&m_threadData->vars[var][oldEffIndex],sizeof(Number));
            }
        }
    }
    else if(m_threadData->dimension[var][0]<oldDimension1)
    {
        for(int c=1; c<oldDimension2; c++)
        {
            for(int c1=0; c1<oldDimension1; c1++)
            {
                            
                oldEffIndex=c1+c*oldDimension1;
                newEffIndex=c1+c*m_threadData->dimension[var][0];
                if(oldEffIndex<m_threadData->numlen[var] && newEffIndex<m_threadData->numlen[var])
                    memcpy(&m_threadData->vars[var][newEffIndex],&m_threadData->vars[var][oldEffIndex],sizeof(Number));
            }
        }
        m_threadData->vars[var]=(Number*)realloc((void*)m_threadData->vars[var],sizeof(Number)*newlen);
        m_threadData->numlen[var]=newlen;
    }
}

void MatrixWidget::resetInterface()
{
    size1Box->hide();
    size2Box->hide();
    matrixBox->hide();
    vectorBox->hide();
    matrixLabel->hide();
    vectorLabel->hide();
    size1Label->hide();
    size2Label->hide();
    calcButton->hide();
    calcWidget->hide();
//    standardButtons->hide();
    resultTable->hide();
    sizeButton->hide();
    label1->hide();
    label2->hide();
    label3->hide();
    input1->hide();
    input2->hide();
    input3->hide();
    typeBox->hide();
    calcButton->setEnabled(true);
    
    switch(state)
    {
        case MATCALC:
            calcWidget->show();
            m_calcButtons->show();
            break;
        case MATLSE:
            size1Box->setMinimum(1);
            size1Box->setMaximum(20);
            vectorLabel->setText(MATRIXWIDGETC_STR1);
            size1Box->show();
            matrixBox->show();
            vectorBox->show();
            matrixLabel->show();
            vectorLabel->show();
            size1Label->show();
            size2Label->show();
            calcButton->setText(MATRIXWIDGETH_STR9);
            calcButton->show();
            resultTable->show();
            resultTable->setRowCount(1);
            resultTable->setColumnCount(1);
            setHeader(resultTable);
            matrixBox->setCurrentIndex(currentVar);
            matrixBoxSlot(currentVar);
            sizeButton->show();
            size1Label->setText(MATRIXWIDGETC_STR2);
            matrixLabel->setText(MATRIXWIDGETC_STR3);
            vectorLabel->setText(MATRIXWIDGETC_STR1);
            size2Label->setText(MATRIXWIDGETC_STR4);
            break;
        case MATGENERATE:
            calcWidget->show();
            vectorLabel->setText(MATRIXWIDGETH_STR1);
            vectorLabel->show();
            typeBox->show();
            matrixLabel->setText(MATRIXWIDGETC_STR5);
            matrixLabel->show();
            matrixBox->show();
            size1Box->show();
            size1Label->show();
            sizeButton->show();
            calcButton->setText(MATRIXWIDGETC_STR6);
            calcButton->show();
            input1->setReadOnly(false);
            input2->setReadOnly(false);
            input2->setReadOnly(false);
            break;
        case MATANALYSE:
            resultTable->show();
            resultTable->setRowCount(1);
            resultTable->setColumnCount(1);
            setHeader(resultTable);
            matrixLabel->setText(MATRIXWIDGETC_STR5);
            matrixLabel->show();
            matrixBox->show();
            matrixBox->setCurrentIndex(currentVar);
            label1->show();
            label2->show();
            label3->show();
            input1->show();
            input2->show();
            input3->show();
            input1->setReadOnly(true);
            input2->setReadOnly(true);
            input3->setReadOnly(true);
            vectorLabel->setText(MATRIXWIDGETC_STR7);
            vectorLabel->show();
            size2Label->setText("");
            size2Label->show();
            calcButton->setText(MATRIXWIDGETH_STR9);
            calcButton->show();
            matrixBoxSlot(currentVar);
            break;
        case MATINV:
            resultTable->show();
            resultTable->setRowCount(1);
            resultTable->setColumnCount(1);
            setHeader(resultTable);
            matrixLabel->setText(MATRIXWIDGETC_STR3);
            matrixLabel->show();
            matrixBox->show();
            matrixBox->setCurrentIndex(currentVar);
            size1Box->setMinimum(1);
            size1Box->setMaximum(20);
            size1Box->show();
            size1Label->show();
            size2Label->setText("");
            size2Label->show();
            sizeButton->show();
            calcButton->setText(MATRIXWIDGETH_STR9);
            calcButton->show();
            matrixBoxSlot(currentVar);
            break;
    }
    resizeEvent(nullptr);
}

void MatrixWidget::setHeader(CalcTable*table)
{
    for(int c=0; c<table->columnCount(); c++)
        table->setHorizontalHeaderItem(c, new QTableWidgetItem(QString::number(c)));
    for(int c=0; c<table->rowCount(); c++)
        table->setVerticalHeaderItem(c, new QTableWidgetItem(QString::number(c)));
    table->resizeColumnsToContents();
    table->resizeRowsToContents();
        
}



void MatrixWidget::setPref(Preferences p)
{
    m_pref=p;
    m_calcButtons->setPref(m_pref);
    calcWidget->setPref(m_pref);
}

void MatrixWidget::getPref(Preferences p)
{
    m_pref=p;
    emit prefChange(m_pref);
}


void MatrixWidget::sprodButtonSlot()
{
    calcWidget->setBold(false);
    calcWidget->textInput(getUnicode(DEGREESTRING));
}

void MatrixWidget::invertButtonSlot()
{
    calcWidget->setBold(false);
    calcWidget->textInput("^-1");
}

void MatrixWidget::detButtonSlot()
{
    calcWidget->setBold(false);
    calcWidget->textInput("det");
}


void MatrixWidget::operationBoxSlot(int index)
{
    state=index;
    resetInterface();
}

void MatrixWidget::buttonInputSlot(QString text)
{
    if(text == "calculate")
    {
        calcWidget->calculateKey();
        setOutputTable(currentVar);
        setVarTable();
    }
    else if(text == "backkey")
        calcWidget->backKey();
    else if(text == "clearall")
        calcWidget->clearAll();
    else {
        calcWidget->setBold(false);
        calcWidget->textInput(text);
    }
}


void MatrixWidget::enterSlot()
{
    setVarTable();
    setOutputTable(currentVar);
}

void MatrixWidget::varSelectionSlot(int row ,int)
{
    if(row!=currentVar)
    {
        currentVar=row;
        setOutputTable(currentVar);
        varTable->clearSelection();
        varTable->selectRow(currentVar);
    }
}

void MatrixWidget::varChangedSlot(int row,int col)
{
    int newsize=0;
    if(col==1)
    {
        newsize=varTable->text(row,col).toInt();
        if(newsize>0)
            resizeVar(row,newsize,m_threadData->dimension[row][1]);
    }
    else
    {
        newsize=varTable->text(row,col).toInt();
        if(newsize>0)
            resizeVar(row,m_threadData->dimension[row][0],newsize);
    }

    setVarTable();
    setOutputTable(currentVar);
}

void MatrixWidget::outputChangedSlot(int row,int col)
{
    int effIndex=m_threadData->dimension[currentVar][0]*col+row;
    if(m_threadData->numlen[currentVar]<effIndex)
        resizeVar(currentVar,m_threadData->dimension[currentVar][0],m_threadData->dimension[currentVar][1]);
    
    m_threadData->vars[currentVar][effIndex].type=NFLOAT;
    long double num;
    char*end;
    QByteArray cellText = outputTable->text(row,col).toLocal8Bit();
    num=strtold(cellText.constData(),&end);
    if(end && end[0]!=(char)0)
        num=runCalc(outputTable->text(row,col),&m_pref,m_vars);

    m_threadData->vars[currentVar][effIndex].fval=Complex(num);
    
    setOutputTable(currentVar);
}

void MatrixWidget::matrixBoxSlot(int)
{
    if(state==MATLSE || state==MATGENERATE || state==MATINV)
    {
        int size=m_threadData->dimension[matrixBox->currentIndex()][0];
        if(m_threadData->dimension[matrixBox->currentIndex()][1]>size)
        {
            if(vectorBox->currentIndex()==0)
                size=m_threadData->dimension[matrixBox->currentIndex()][1]-1;
            else size=m_threadData->dimension[matrixBox->currentIndex()][1];
        }    
        size1Box->setValue(size);
        size1BoxSlot(size);

        if(currentVar!=matrixBox->currentIndex())
        {
            currentVar=matrixBox->currentIndex();
            setVarTable();
            setOutputTable(currentVar);
        }
        if(state==MATINV)
            size2Label->setText("");

    }
    else if(state==MATANALYSE)
    {
        int var=matrixBox->currentIndex();
        long double det,num;
        int rank=0;
        if(m_threadData->dimension[var][1]!=1)
        {
            size2Label->setText(MATRIXWIDGETC_STR8);
            int effIndex;
            long double*matrix=(long double*)malloc(sizeof(long double)*m_threadData->dimension[var][0]*m_threadData->dimension[var][1]);
            for(int c1=0; c1<m_threadData->dimension[var][1]; c1++)
            {
                for(int c2=0; c2<m_threadData->dimension[var][0]; c2++)
                {
                    effIndex=c2+c1*m_threadData->dimension[var][0];
                    if(effIndex<m_threadData->numlen[var])
                        matrix[effIndex]=m_threadData->vars[var][effIndex].fval.real();
                    else matrix[effIndex]=NAN;
                }
            }
            det=gauss(m_threadData->dimension[var][1],m_threadData->dimension[var][0],matrix);
            rank=m_threadData->dimension[var][0];
            for(int c=m_threadData->dimension[var][0]-1; c>=0; c--)
            {
                qWarning().noquote() << formatOutput(matrix[c+m_threadData->dimension[var][0]*(m_threadData->dimension[var][1]-1)],&m_pref);
                if(matrix[c+m_threadData->dimension[var][0]*(m_threadData->dimension[var][1]-1)]==0.0)
                    rank=c;
                else break;
            }
            label1->setText(MATRIXWIDGETC_STR9);
            input1->setText(QString::number(m_threadData->dimension[var][0])+"x"+QString::number(m_threadData->dimension[var][1]));
            label2->setText(MATRIXWIDGETC_STR10);
            if(m_threadData->dimension[var][0]==m_threadData->dimension[var][0])
                input2->setText(formatOutput(det,&m_pref));
            else input2->setText(MATRIXWIDGETC_STR11);
            label3->setText(MATRIXWIDGETC_STR12);
            input3->setText(QString::number(rank));
            resultTable->setRowCount(m_threadData->dimension[var][0]);
            resultTable->setColumnCount(m_threadData->dimension[var][1]);
            setHeader(resultTable);
            for(int c1=0; c1<m_threadData->dimension[var][1]; c1++)
            {
                for(int c2=0; c2<m_threadData->dimension[var][0]; c2++)
                {
                    effIndex=c2+c1*m_threadData->dimension[var][0];
                    resultTable->setText(c2,c1,formatOutput(matrix[effIndex],&m_pref));
                }
            }
            free(matrix);
        }
        else if(m_threadData->dimension[var][0]!=1)
        {
            size2Label->setText(MATRIXWIDGETC_STR13);
            label1->setText(MATRIXWIDGETC_STR14);
            input1->setText(QString::number(m_threadData->dimension[var][0]));
            label2->setText(MATRIXWIDGETC_STR15);
            num=0.0;
            for(int c=0; c<m_threadData->dimension[var][0] && c<m_threadData->numlen[var]; c++)
                num+=m_threadData->vars[var][c].fval.real()*m_threadData->vars[var][c].fval.real();
            input2->setText(formatOutput(sqrtl(num),&m_pref));
            label3->setText("");
            input3->setText("");
            resultTable->setRowCount(0);
            resultTable->setColumnCount(0);
        }
        else {
            size2Label->setText(MATRIXWIDGETC_STR16);
            label1->setText(MATRIXWIDGETC_STR15);
            input1->setText(formatOutput(fabsl(m_threadData->vars[var][0].fval.real()),&m_pref));
            label2->setText("");
            input2->setText("");
            label3->setText("");
            input3->setText("");
            resultTable->setRowCount(0);
            resultTable->setColumnCount(0);
        }
        currentVar=matrixBox->currentIndex();
        setVarTable();
        setOutputTable(currentVar);
    }
    
}

void MatrixWidget::vectorBoxSlot(int)
{
    if(state==MATLSE && vectorBox->currentIndex()!=0)
    {
        if(vectorBox->currentIndex()-1==matrixBox->currentIndex())
        {
            vectorBox->setCurrentIndex(0);
            WarningBox(MATRIXWIDGETC_STR17);
        }

        if(vectorBox->currentIndex()!=0)
        {
            currentVar=vectorBox->currentIndex()-1;
            setVarTable();
            setOutputTable(currentVar);
            size1BoxSlot(m_threadData->dimension[currentVar][0]);
        }
    }
}

void MatrixWidget::size1BoxSlot(int newsize)
{
    if(state==MATLSE)
    {
        if(vectorBox->currentIndex()==0)
        {
            if(m_threadData->dimension[matrixBox->currentIndex()][0]==newsize &&m_threadData->dimension[matrixBox->currentIndex()][1]==newsize+1)
            {
                calcButton->setEnabled(true);
                size2Label->setText( MATRIXWIDGETC_STR18);
            }
            else {
                calcButton->setEnabled(false);
                size2Label->setText(MATRIXWIDGETC_STR19);
            }
        }
        else
        {
            if(m_threadData->dimension[matrixBox->currentIndex()][0]==newsize &&
                m_threadData->dimension[matrixBox->currentIndex()][1]==newsize &&
                m_threadData->dimension[vectorBox->currentIndex()-1][0]==newsize &&
                m_threadData->dimension[vectorBox->currentIndex()-1][1]==1)
            {
                calcButton->setEnabled(true);
                size2Label->setText(MATRIXWIDGETC_STR20);
            }
            else {
                calcButton->setEnabled(false);
                size2Label->setText(MATRIXWIDGETC_STR21);
            }
        }
    }
}

void MatrixWidget::sizeButtonSlot()
{
    if(state==MATLSE)
    {
        int newsize=size1Box->value();
        if(vectorBox->currentIndex()==0)
        {
            resizeVar(matrixBox->currentIndex(),newsize,newsize+1);
            if(currentVar!=matrixBox->currentIndex())
                currentVar=matrixBox->currentIndex();
        }
        else
        {
            resizeVar(matrixBox->currentIndex(),newsize,newsize);
            resizeVar(vectorBox->currentIndex()-1,newsize,1);
        }
        setVarTable();
        setOutputTable(currentVar);
        calcButton->setEnabled(true);
    }
    else if(state==MATGENERATE || state==MATINV)
    {
        int newsize=size1Box->value();
        resizeVar(matrixBox->currentIndex(),newsize,newsize);
        setVarTable();
        setOutputTable(currentVar);
    }
}


void MatrixWidget::typeBoxSlot(int index)
{
    if(state==MATGENERATE)
    {
        switch(index)
        {
            case 3:                //rotate x
                size1Box->setMinimum(2);
                size1Box->setMaximum(3);
                label1->setText(MATRIXWIDGETC_STR22);
                label1->show();
                label2->hide();
                label3->hide();
                input1->show();
                input2->hide();
                input3->hide();
                break;
            case 4:                //rotate y
                size1Box->setMinimum(3);
                size1Box->setMaximum(3);
                label1->setText(MATRIXWIDGETC_STR23);
                label1->show();
                label2->hide();
                label3->hide();
                input1->show();
                input2->hide();
                input3->hide();
                break;
            case 5:                //rotate z
                size1Box->setMinimum(3);
                size1Box->setMaximum(3);
                label1->setText(MATRIXWIDGETC_STR24);
                label1->show();
                label2->hide();
                label3->hide();
                input1->show();
                input2->hide();
                input3->hide();
                break;
            case 6:                //scale
                size1Box->setMinimum(2);
                size1Box->setMaximum(3);
                label1->setText(MATRIXWIDGETC_STR25);
                label2->setText(MATRIXWIDGETC_STR26);
                label3->setText(MATRIXWIDGETC_STR27);
                label1->show();
                label2->show();
                label3->show();
                input1->show();
                input2->show();
                input3->show();
                break;    
            default:
                size1Box->setMinimum(1);
                size1Box->setMaximum(20);
                label1->hide();
                label2->hide();
                label3->hide();
                input1->hide();
                input2->hide();
                input3->hide();
        }
    }
}

void MatrixWidget::dockWindowSlot()
{
}

void MatrixWidget::catalogSlot()
{
    catalog->exec(toolBar->mapToGlobal(QPoint(catalogButton->x(),catalogButton->y()+catalogButton->height())));
}



void MatrixWidget::calcButtonSlot()
{
    switch(state)
    {
        case MATLSE:
        {
            int size=size1Box->value(),effSrcIndex,effDestIndex;
            int matVar=matrixBox->currentIndex(),vecVar=vectorBox->currentIndex()-1;
            long double *matrix=(long double*)malloc(sizeof(long double)*size*(size+1));
            for(int c1=0; c1<size; c1++)
            {
                for(int c2=0; c2<size; c2++)
                {
                    effSrcIndex=c2+c1*m_threadData->dimension[matVar][0];
                    effDestIndex=c2+c1*size;
                    if(effSrcIndex<m_threadData->numlen[matVar])
                        matrix[effDestIndex]=m_threadData->vars[matVar][effSrcIndex].fval.real();
                    else matrix[effDestIndex]=NAN;
                }
                if(vecVar==-1)
                {
                    if(c1+size*size<m_threadData->numlen[matVar])
                        matrix[c1+size*size]=m_threadData->vars[matVar][c1+size*size].fval.real();
                    else matrix[c1+size*size]=NAN;
                }
                else {
                    if(c1<m_threadData->numlen[vecVar])
                        matrix[c1+size*size]=m_threadData->vars[vecVar][c1].fval.real();
                    else matrix[c1+size*size]=NAN;
                }
            }
            gauss(size+1,size,matrix);
            resultTable->setRowCount(size);
            resultTable->setColumnCount(1);
            setHeader(resultTable);

            if(matrix[(size-1)*size+size-1]==0.0)
            {
                resultTable->setColumnCount(size+1);
                for(int c2=0; c2<size; c2++)
                {
                    for(int c3=0; c3<size+1; c3++)
                        resultTable->setText(c2,c3,formatOutput(matrix[c3*size+c2],&m_pref));
                }
                if(matrix[size*size+size-1]==0.0)
                    size2Label->setText(MATRIXWIDGETC_STR28);
                else size2Label->setText(MATRIXWIDGETC_STR29);
            }
            else {
                long double *results=(long double*)malloc(sizeof(long double)*size);
                for(int c=size-1; c>=0; c--)
                {
                    results[c]=matrix[size*size+c];
                    for(int c1=c; c1<size; c1++)
                    {
                        if(c1!=c)
                            results[c]-=results[c1]*matrix[c+c1*size];
                    }
                    results[c]/=matrix[c+c*size];
                    resultTable->setText(c,0,formatOutput(results[c],&m_pref));
                }
                free(results);
                size2Label->setText(MATRIXWIDGETC_STR30);
            }
            
            for(int c=0; c<resultTable->columnCount(); c++)
                resultTable->adjustColumn(c);

            
            
            free(matrix);
            break;
        }
        case MATGENERATE:
        {
            int var=matrixBox->currentIndex();
            int size=size1Box->value();
            long double num;
            switch(typeBox->currentIndex())
            {
                case 0:                            //identity matrix
                    resizeVar(var,size,size);
                    for(int c1=0; c1<size; c1++)
                        for(int c2=0; c2<size; c2++)
                    {
                        m_threadData->vars[var][size*c1+c2].type=NFLOAT;
                        if(c1==c2)
                            m_threadData->vars[var][size*c1+c2].fval=Complex(1.0,0.0);
                        else m_threadData->vars[var][size*c1+c2].fval=Complex(0.0,0.0);
                    }
                    break;
                case 1:                            //zero matrix
                    resizeVar(var,size,size);
                    for(int c1=0; c1<size; c1++)
                        for(int c2=0; c2<size; c2++)
                    {
                        m_threadData->vars[var][size*c1+c2].type=NFLOAT;
                        m_threadData->vars[var][size*c1+c2].fval=Complex(0.0,0.0);
                    }
                    break;
                case 2:                        //zero vector
                    resizeVar(var,size,1);
                    for(int c=0; c<size; c++)
                    {
                        m_threadData->vars[var][c].type=NFLOAT;
                        m_threadData->vars[var][c].fval=Complex(0.0,0.0);
                    }    
                    break;
                case 3:                    //x-rotate mtrix
                    resizeVar(var,size,size);
                    
                    num=runCalc(input1->text(),&m_pref,m_vars);
                    if(m_pref.angle==DEG)
                        num*=PI/180.0;
                    else if(m_pref.angle==GRA)
                        num*=PI/200.0;
                    for(int c=0; c<m_threadData->numlen[var]; c++)
                        m_threadData->vars[var][c].type=NFLOAT;
                    if(size==2)
                    {
                        m_threadData->vars[var][0].fval=Complex(cosl(num));
                        m_threadData->vars[var][1].fval=Complex(sinl(num));
                        m_threadData->vars[var][2].fval=Complex(-1.0*sinl(num));
                        m_threadData->vars[var][3].fval=Complex(cosl(num));
                    }
                    else 
                    {
                        m_threadData->vars[var][0].fval=Complex(1.0);
                        m_threadData->vars[var][1].fval=Complex(0.0);
                        m_threadData->vars[var][2].fval=Complex(0.0);
                        m_threadData->vars[var][3].fval=Complex(0.0);
                        m_threadData->vars[var][4].fval=Complex(cosl(num));
                        m_threadData->vars[var][5].fval=Complex(sinl(num));
                        m_threadData->vars[var][6].fval=Complex(0.0);
                        m_threadData->vars[var][7].fval=Complex(-1.0*sinl(num));
                        m_threadData->vars[var][8].fval=Complex(cosl(num));
                    }
                    break;
                case 4:                    //y-rotate matrix
                    resizeVar(var,size,size);
                    num=runCalc(input1->text(),&m_pref,m_vars);
                    if(m_pref.angle==DEG)
                        num*=PI/180.0;
                    else if(m_pref.angle==GRA)
                        num*=PI/200.0;
                    for(int c=0; c<m_threadData->numlen[var]; c++)
                        m_threadData->vars[var][c].type=NFLOAT;
                    
                    m_threadData->vars[var][0].fval=Complex(cosl(num));
                    m_threadData->vars[var][1].fval=Complex(0.0);
                    m_threadData->vars[var][2].fval=Complex(-1.0*sinl(num));
                    m_threadData->vars[var][3].fval=Complex(0.0);
                    m_threadData->vars[var][4].fval=Complex(1.0);
                    m_threadData->vars[var][5].fval=Complex(0.0);
                    m_threadData->vars[var][6].fval=Complex(sinl(num));
                    m_threadData->vars[var][7].fval=Complex(0.0);
                    m_threadData->vars[var][8].fval=Complex(cosl(num));
                    break;
                case 5:                    //z-rotate matrix
                    resizeVar(var,size,size);
                    num=runCalc(input1->text(),&m_pref,m_vars);
                    if(m_pref.angle==DEG)
                        num*=PI/180.0;
                    else if(m_pref.angle==GRA)
                        num*=PI/200.0;
                    for(int c=0; c<m_threadData->numlen[var]; c++)
                        m_threadData->vars[var][c].type=NFLOAT;
                    
                    m_threadData->vars[var][0].fval=Complex(cosl(num));
                    m_threadData->vars[var][1].fval=Complex(sinl(num));
                    m_threadData->vars[var][2].fval=Complex(0.0);
                    m_threadData->vars[var][3].fval=Complex(-1.0*sinl(num));
                    m_threadData->vars[var][4].fval=Complex(cosl(num));
                    m_threadData->vars[var][5].fval=Complex(0.0);
                    m_threadData->vars[var][6].fval=Complex(0.0);
                    m_threadData->vars[var][7].fval=Complex(0.0);
                    m_threadData->vars[var][8].fval=Complex(1.0);
                    break;
                case 6:                    //scale matrix
                    resizeVar(var,size,size);
                    for(int c=0; c<m_threadData->numlen[var]; c++)
                    {
                        m_threadData->vars[var][c].type=NFLOAT;
                        m_threadData->vars[var][c].fval=Complex(0.0);
                    }
                    
                    if(size==2)
                    {
                        num=runCalc(input1->text(),&m_pref,m_vars);
                        m_threadData->vars[var][0].fval=Complex(num);
                        num=runCalc(input2->text(),&m_pref,m_vars);
                        m_threadData->vars[var][2].fval=Complex(num);
                    }
                    else if(size==3)
                    {
                        num=runCalc(input1->text(),&m_pref,m_vars);
                        m_threadData->vars[var][0].fval=Complex(num);
                        num=runCalc(input2->text(),&m_pref,m_vars);
                        m_threadData->vars[var][4].fval=Complex(num);
                        num=runCalc(input3->text(),&m_pref,m_vars);
                        m_threadData->vars[var][8].fval=Complex(num);
                    }
                    break;
            }
            currentVar=var;
            setVarTable();
            setOutputTable(currentVar);
            break;
        }
        case MATANALYSE:
        {
            matrixBox->setCurrentIndex(currentVar);
            matrixBoxSlot(currentVar);
            break;
        }
        case MATINV:
        {
            long double*matrix;
            long double mainDet;
            int size=size1Box->value(),effIndex;
            int var=matrixBox->currentIndex();
            
            if(m_threadData->dimension[var][0]!=m_threadData->dimension[var][1])
            {
                size2Label->setText(MATRIXWIDGETC_STR31);
                resultTable->setRowCount(1);
                resultTable->setColumnCount(1);
                setHeader(resultTable);
                break;
            }
            else size2Label->setText("");
            
            resultTable->setRowCount(size);
            resultTable->setColumnCount(size);
            setHeader(resultTable);

            matrix=(long double*)malloc(size*size*sizeof(long double));
            for(int c1=0; c1<size; c1++)
                for(int c2=0; c2<size; c2++)
            {
                effIndex=c1+c2*m_threadData->dimension[var][0];
                if(effIndex<m_threadData->numlen[var])
                {
                    convertToFloat(&m_threadData->vars[var][effIndex]);
                    matrix[c1+size*c2]=m_threadData->vars[var][effIndex].fval.real();
                }
            }
            mainDet=gauss(size,size,matrix);
            if(mainDet==0.0)
            {
                size2Label->setText(MATRIXWIDGETC_STR32);
                resultTable->setRowCount(1);
                resultTable->setColumnCount(1);
                setHeader(resultTable);
                break;
            }
            mainDet=1.0/mainDet;
            
            resizeVar(27,size,size);
            m_threadData->dimension[27][0]=m_threadData->dimension[27][1]=size;
            
            int pos1,pos2,effSrcIndex,effDestIndex,vz;
            for(int c3=0; c3<size; c3++)
            {
                for(int c4=0; c4<size; c4++)
                {
                    effIndex=c3+c4*m_threadData->dimension[var][0];
                    pos1=0;
                    for(int c1=0; c1<size; c1++)
                    {
                        if(c1!=c3)
                        {
                            pos2=0;
                            for(int c2=0; c2<size; c2++)
                            {
                                effDestIndex=pos1+(size-1)*pos2;
                                effSrcIndex=c1+c2*m_threadData->dimension[var][0];
                                if(c2!=c4)
                                {
                                    if(effSrcIndex<m_threadData->numlen[var])
                                        matrix[effDestIndex]=m_threadData->vars[var][effSrcIndex].fval.real();
                                    else matrix[effDestIndex]=NAN;
                                    pos2++;
                                }
                            }
                            pos1++;
                        }
                    }
                    vz=(c3+c4)%2;
                    if(vz==0)
                        vz=1;
                    else vz=-1;
                    effDestIndex=c4+c3*size;
                    long double subDet=gauss(size-1,size-1,matrix);
                    m_threadData->vars[27][effDestIndex].fval=Complex(mainDet*(long double)vz*subDet);
                    m_threadData->vars[27][effDestIndex].type=NFLOAT;
                    resultTable->setText(c4,c3,formatOutput(m_threadData->vars[27][effDestIndex].fval.real(),&m_pref));
                    if(c4==size-1)
                        resultTable->adjustColumn(c3);
                }
                
            }
            free(matrix);
            break;
        }
    }
}
