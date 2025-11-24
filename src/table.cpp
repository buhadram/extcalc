/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         table.cpp
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.


////////////////////////////////////////////////////////////////////////////////////////////*/
#include "table.h"
//Added by qt3to4:
#include <QResizeEvent>
//#include <QList>
#include <QVector>
#include <QTableWidgetItem>

/*
void TableWidget::resizeEvent(QResizeEvent*)
{
    int width=geometry().right() - geometry().left();
    int height=geometry().bottom() - geometry().top();


    if(fullscreen)
    {
//        outputTable->setGeometry(20,50,width-40,height-100);
        horzSplit->setGeometry(20,menuBottom+40,width-40,height-100);
        dockArea->setGeometry(0,menuBottom,width,35);
        
//        calculateButton->setGeometry(20,height-45,90,35);
//        typeBox->setGeometry(120,height-45,90,35);
//        maximizeButton->setGeometry(220,height-45,90,35);
    }
    else
    {
//        functionTable->setGeometry(20,50,width/2-30,height-320);
//        outputTable->setGeometry(width/2+10,50,width/2-30,height-290);
//        inputLine->setGeometry(20,height-260,width/2-30,20);
        
        horzSplit->setGeometry(20,50,width-40,height-290);
    
        standardButtons->setGeometry(20,height-220,280,200);
        m_extButtons->setGeometry(width/2+10,height-180,300,160);
        
//        calculateButton->setGeometry(width/2+15,height-220,90,35);
//        typeBox->setGeometry(width/2+115,height-220,90,35);
//        maximizeButton->setGeometry(width/2+215,height-220,90,35);
        dockArea->setGeometry(width/2+15,height-220,width/2-35,35);
    }
}
*/


void TableWidget::setPref(Preferences p)
{
    m_pref=p;
    if(m_pref.tableType==TABLENORMAL)
        typeBox->setCurrentText(TABLEH_STR3);
    else if(m_pref.tableType==TABLEPOLAR)
        typeBox->setCurrentText(TABLEH_STR4);
    else if(m_pref.tableType==TABLEPARAMETER)
        typeBox->setCurrentText(TABLEH_STR5);
    else if(m_pref.tableType==TABLEINEQUALITY)
        typeBox->setCurrentText(TABLEH_STR6);
    else if(m_pref.tableType==TABLE3D)
        typeBox->setCurrentText(TABLEH_STR7);
    else if(m_pref.tableType==TABLECOMPLEX)
        typeBox->setCurrentText(TABLEH_STR9);
    
    
    outputTable->setRowCount(m_pref.tableXSteps);
    double tableXStep=(m_pref.tableXEnd-m_pref.tableXStart)/(m_pref.tableXSteps-1);
    double tableZStep=(m_pref.tableZEnd-m_pref.tableZStart)/(m_pref.tableZSteps-1);

    int oldNum=vertValues.GetLen();
    
    if(oldNum<m_pref.tableXSteps)
    {
        for(int c=oldNum; c<m_pref.tableXSteps; c++)
            vertValues.NewItem(c*tableXStep+m_pref.tableXStart);
    }
    else {
        for(int c=m_pref.tableXSteps; c<oldNum; c++)
            vertValues.DeleteItem(m_pref.tableXSteps);
    }
    for(int c=0; c<m_pref.tableXSteps; c++)
    {
        vertValues[c]=c*tableXStep+m_pref.tableXStart;
        outputTable->setVerticalHeaderItem(c, new QTableWidgetItem(QString::number(vertValues[c],'g',5)));
    }
    
    oldNum=horzValues.GetLen();
    if(oldNum<m_pref.tableZSteps)
    {
        for(int c=oldNum; c<m_pref.tableZSteps; c++)
            horzValues.NewItem(c*tableZStep+m_pref.tableZStart);
    }
    else {
        for(int c=m_pref.tableZSteps; c<oldNum; c++)
            horzValues.DeleteItem(m_pref.tableZSteps);
    }
    for(int c=0; c<m_pref.tableZSteps; c++)
        horzValues[c]=c*tableZStep+m_pref.tableZStart;


    m_calcButtons->setPref(m_pref);
    m_extButtons->setPref(m_pref);
    functionTable->setPref(m_pref);
}

void TableWidget::getPref(Preferences p)
{
    m_pref=p;
    emit prefChange(m_pref);
}

void TableWidget::inputTextChanged()
{
    functionTable->setFunctionText(inputLine->text());
    inputLine->clearFocus();
}

void TableWidget::selectionChangedSlot(int row,int)
{
    if(functionTable->text(row,0).length() > 0)        //10000th code line
        inputLine->setText(functionTable->text(row,0));
    else inputLine->clear();
}

void TableWidget::tableEditSlot(QString string)
{
    if(string.length() > 0)
        inputLine->setText(string);
    else inputLine->clear();
    inputLine->setFocus();
}

void TableWidget::calculateButtonSlot()
{
    outputTable->setColumnCount(0);
    
    if(m_pref.tableType==TABLENORMAL)
    {
        m_vars[0]=m_pref.tableAValue;
        for(int c=0; c<20;c++)
        {
            if(m_pref.functionTypes[c]==GRAPHSTD &&m_pref.activeFunctions[c])
            {
                outputTable->setColumnCount(outputTable->columnCount()+1);
                outputTable->setHorizontalHeaderItem(outputTable->columnCount()-1, new QTableWidgetItem("y"+QString::number(c+1)+"(x)"));
                char*cleanString=preprocessor(&m_pref.functions[c],&m_pref,false);
                Calculate ca(nullptr,cleanString,&m_pref,m_vars);
                
                for(int c=0; c<m_pref.tableXSteps;c++)
                {
                    m_vars[23]=vertValues[c];
                    outputTable->setText(c,outputTable->columnCount()-1,formatOutput(ca.calc(),&m_pref));
                }
                if(cleanString!=nullptr)
                    free(cleanString);
            }
        }
    }
    else if(m_pref.tableType==TABLEPOLAR)
    {
        m_vars[0]=m_pref.tableAValue;
        for(int c=0; c<20;c++)
        {
            if(m_pref.functionTypes[c]==GRAPHPOLAR &&m_pref.activeFunctions[c])
            {
                outputTable->setColumnCount(outputTable->columnCount()+1);
                outputTable->setHorizontalHeaderItem(outputTable->columnCount()-1, new QTableWidgetItem("r"+QString::number(c+1)+"(x)"));
                char*cleanString=preprocessor(&m_pref.functions[c],&m_pref,false);
                Calculate ca(nullptr,cleanString,&m_pref,m_vars);
                for(int c=0; c<m_pref.tableXSteps;c++)
                {
                    m_vars[23]=vertValues[c];
                    outputTable->setText(c,outputTable->columnCount()-1,formatOutput(ca.calc(),&m_pref));
                }
                if(cleanString!=nullptr)
                    free(cleanString);
            }
        }
    }
    else if(m_pref.tableType==TABLEPARAMETER)
    {
        m_vars[0]=m_pref.tableAValue;
        for(int c=0; c<20;c++)
        {
            if(m_pref.functionTypes[c]==GRAPHPARAMETER &&m_pref.activeFunctions[c])
            {
                outputTable->setColumnCount(outputTable->columnCount()+2);
                outputTable->setHorizontalHeaderItem(outputTable->columnCount()-2, new QTableWidgetItem("X"+QString::number(c+1)+"(T)"));
                outputTable->setHorizontalHeaderItem(outputTable->columnCount()-1, new QTableWidgetItem("Y"+QString::number(c+1)+"(T)"));
                QString input=m_pref.functions[c].left(m_pref.functions[c].indexOf("\\"));
                char*cleanStringX=preprocessor(&input,&m_pref,false);
                input=m_pref.functions[c].right(m_pref.functions[c].length()-1-m_pref.functions[c].indexOf("\\"));
                char*cleanStringY=preprocessor(&input,&m_pref,false);
                Calculate caX(nullptr,cleanStringX,&m_pref,m_vars);
                Calculate caY(nullptr,cleanStringY,&m_pref,m_vars);
                for(int c=0; c<m_pref.tableXSteps;c++)
                {
                    m_vars[19]=vertValues[c];
                    outputTable->setText(c,outputTable->columnCount()-2,formatOutput(caX.calc(),&m_pref));
                    outputTable->setText(c,outputTable->columnCount()-1,formatOutput(caY.calc(),&m_pref));
                }
                if(cleanStringX!=nullptr)
                    free(cleanStringX);
                if(cleanStringY!=nullptr)
                    free(cleanStringY);
            }
        }
    }
    else if(m_pref.tableType==TABLEINEQUALITY)
    {
        m_vars[0]=m_pref.tableAValue;
        for(int c=0; c<20;c++)
        {
            if((m_pref.functionTypes[c]==GRAPHIEGE || 
                         m_pref.functionTypes[c]==GRAPHIEG || 
                         m_pref.functionTypes[c]==GRAPHIELE || 
                         m_pref.functionTypes[c]==GRAPHIEG )&&m_pref.activeFunctions[c])
            {
                outputTable->setColumnCount(outputTable->columnCount()+1);
                outputTable->setHorizontalHeaderItem(outputTable->columnCount()-1, new QTableWidgetItem("y"+QString::number(c+1)+"(x)"));
                char*cleanString=preprocessor(&m_pref.functions[c],&m_pref,false);
                Calculate ca(nullptr,cleanString,&m_pref,m_vars);
                for(int c=0; c<m_pref.tableXSteps;c++)
                {
                    m_vars[23]=vertValues[c];
                    outputTable->setText(c,outputTable->columnCount()-1,formatOutput(ca.calc(),&m_pref));
                }
                if(cleanString!=nullptr)
                    free(cleanString);
            }
        }
    }
    else if(m_pref.tableType==TABLE3D)  
    {
        m_vars[0]=m_pref.tableAValue;
        for(int c=0; c<20;c++)
        {
            if(m_pref.functionTypes[c]==GRAPH3D &&m_pref.activeFunctions[c])
            {
                outputTable->setColumnCount(outputTable->columnCount()+m_pref.tableZSteps);
                for(int c1=0; c1<m_pref.tableZSteps;c1++)
                    outputTable->setHorizontalHeaderItem(outputTable->columnCount()-m_pref.tableZSteps+c1, new QTableWidgetItem("y"+QString::number(c+1)+"(x"+QString::number(horzValues[c1],'g',5)+")"));
                char*cleanString=preprocessor(&m_pref.functions[c],&m_pref,false);
                Calculate ca(nullptr,cleanString,&m_pref,m_vars);
                for(int c2=0; c2<m_pref.tableXSteps;c2++)
                {
                    for(int c1=0; c1<m_pref.tableZSteps;c1++)
                    {
                        m_vars[23]=vertValues[c2];
                        m_vars[25]=horzValues[c1];
                        outputTable->setText(c2,outputTable->columnCount()-m_pref.tableZSteps+c1,formatOutput(ca.calc(),&m_pref));
                    }
                }
                if(cleanString!=nullptr)
                    free(cleanString);
            }
        }
    }
    else if(m_pref.tableType==TABLECOMPLEX)
    {
        m_threadData->vars[0][0].fval=Complex(m_pref.tableAValue);
        bool complexPref=m_pref.complex;
        m_pref.complex=true;
        for(int c=0; c<20;c++)
        {
            if((m_pref.functionTypes[c]==GRAPHCOMPLEX || m_pref.functionTypes[c]==GRAPHCOMP3D) &&m_pref.activeFunctions[c])
            {
                outputTable->setColumnCount(outputTable->columnCount()+1);
                outputTable->setHorizontalHeaderItem(outputTable->columnCount()-1, new QTableWidgetItem("F"+QString::number(c+1)+"(z)"));
                char*cleanString=preprocessor(&m_pref.functions[c],&m_pref,false);
                if(cleanString==nullptr || strlen(cleanString)<=0)
                {
                    for(int c=0; c<m_pref.tableXSteps; c++)
                        outputTable->setText(c,outputTable->columnCount()-1,"nan");
                    continue;
                }
                Script ca(nullptr,cleanString,&m_pref,m_vars,m_threadData);
                m_threadData->vars[25][0].type=NFLOAT;
                for(int c=0; c<m_pref.tableXSteps;c++)
                {
                    m_threadData->vars[25][0].fval=Complex(vertValues[c]);
                    outputTable->setText(c,outputTable->columnCount()-1,formatOutput(ca.exec(),&m_pref));
                }
                if(cleanString!=nullptr)
                    free(cleanString);
            }
        }
        m_pref.complex=complexPref;
    }
}


void TableWidget::maximizeButtonSlot()
{
    
    if(isMaximized())
    {
        maximizeSlot(false);
        maximizeButton->setIcon(*maximizeIcon);
    }
    else {
        maximizeSlot(true);
        maximizeButton->setIcon(*minimizeIcon);
    }

}

void TableWidget::typeBoxSlot(const QString&str)
{

    if(str==TABLEH_STR3)
        m_pref.tableType=TABLENORMAL;
    else if(str==TABLEH_STR4)
        m_pref.tableType=TABLEPOLAR;
    else if(str==TABLEH_STR5)
        m_pref.tableType=TABLEPARAMETER;
    else if(str==TABLEH_STR6)
        m_pref.tableType=TABLEINEQUALITY;
    else if(str==TABLEH_STR7)
        m_pref.tableType=TABLE3D;
    else if(str==TABLEH_STR9)
        m_pref.tableType=TABLECOMPLEX;
    emit prefChange(m_pref);
}


void TableWidget::buttonInputSlot(QString text)
{
    
    if(text == "calculate")
    {
        calculateButtonSlot();
    }
    else if(text == "backkey")
    {
        if(inputLine->cursorPosition() > 0)
            inputLine->backspace();
        else inputLine->del();
        functionTable->setText(functionTable->currentRow(),0,inputLine->text());
        if((inputLine->text().length())<=0)
        {
            QTableWidgetItem *checkItem=functionTable->item(functionTable->currentRow(),2);
            if(checkItem) checkItem->setCheckState(Qt::Unchecked);
        }
    }
    else if(text == "clearall")
    {
    //    functionTable->clearCell(functionTable->currentRow(),0);
    //    inputLine->clear();
    //    QCheckTableItem *checkItem=(QCheckTableItem*)functionTable->item(functionTable->currentRow(),2);
    //    checkItem->setChecked(false);
        functionTable->setFunctionText("");
    }
    else {
        QString fullText=inputLine->text();
        int cursorPos=inputLine->cursorPosition();
        fullText.insert(cursorPos,text);
        inputLine->setText(fullText);
        inputLine->setCursorPosition(cursorPos+text.length());
        functionTable->setText(functionTable->currentRow(),0,inputLine->text());
        QTableWidgetItem *checkItem=functionTable->item(functionTable->currentRow(),2);
        if(checkItem) checkItem->setCheckState(Qt::Checked);
    }

}

void TableWidget::editSlot(int type)
{
    if(inputLine->hasFocus())
    {
        switch(type)
        {
            case EDITUNDO:
                inputLine->undo();
                break;
            case EDITREDO:
                inputLine->redo();
                break;
            case EDITCUT:
                inputLine->cut();
                break;
            case EDITCOPY:
                inputLine->copy();
                break;
            case EDITPASTE:
                inputLine->paste();
                break;
        }
    }
    else {
        if(type==EDITCOPY)
        {
            
            if(functionTable->hasFocus())
            {
                if(functionTable->currentColumn()==0)
                    (QApplication::clipboard())->setText(functionTable->text(functionTable->currentRow(),0),QClipboard::Clipboard);
            }
            else if(outputTable->hasFocus())
            {
                (QApplication::clipboard())->setText(outputTable->text(outputTable->currentRow(),outputTable->currentColumn()),QClipboard::Clipboard);
            }
        }
    }
}

void TableWidget::horzHeaderSlot(int index)
{
    bool ok;

    if(m_pref.tableType!=TABLE3D)
        return;
    index=index%m_pref.tableZSteps;
    QString input = QInputDialog::getText(
            this, TABLEH_STR10, TABLEH_STR11+QString::number(index+1)+":", QLineEdit::Normal,
    QString(), &ok );
    if ( ok && input.length()>0 )
    {
        horzValues[index]=runCalc(input,&m_pref,m_vars);
        calculateButtonSlot();
    }
    
}

void TableWidget::vertHeaderSlot(int index)
{
    bool ok;
    QString input = QInputDialog::getText(
            this, TABLEH_STR10, TABLEH_STR12+QString::number(index+1)+":", QLineEdit::Normal,
    QString(), &ok );
    if ( ok && input.length()>0 )
    {
        vertValues[index]=runCalc(input,&m_pref,m_vars);
        outputTable->setVerticalHeaderItem(index, new QTableWidgetItem(QString::number(vertValues[index],'g',5)));
        calculateButtonSlot();
    }
}

void TableWidget::tableMenuSlot(int item)
{
    if(item==RESETTABLE)
    {
        double tableXStep=(m_pref.tableXEnd-m_pref.tableXStart)/(m_pref.tableXSteps-1);
        double tableZStep=(m_pref.tableZEnd-m_pref.tableZStart)/(m_pref.tableZSteps-1);
        
        for(int c=0; c<vertValues.GetLen(); c++)
        {
            vertValues[c]=m_pref.tableXStart+c*tableXStep;
            outputTable->setVerticalHeaderItem(c, new QTableWidgetItem(QString::number(vertValues[c],'g',5)));
        }
        for(int c=0; c<horzValues.GetLen(); c++)
            horzValues[c]=m_pref.tableZStart+c*tableZStep;
        calculateButtonSlot();
    }
}

void TableWidget::dockWindowSlot()
{
}


void TableWidget::catalogSlot()
{
    catalog->exec(toolBar->mapToGlobal(QPoint(catalogButton->x(),catalogButton->y()+catalogButton->height())));
}
