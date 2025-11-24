/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         calcinput.cpp
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.


////////////////////////////////////////////////////////////////////////////////////////////*/
#include "calcinput.h"
//Added by qt3to4:
#include <QResizeEvent>
#include <QMenu>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QEvent>
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QtGlobal>

void CalcInput::keyPressEvent(QKeyEvent *e)
{
    int para;
    int pos;
    bool noCheck = false;

    getCursorPosition(&para, &pos);
    if (para != paragraphs() - 1 &&
        !((e->modifiers() == Qt::ControlModifier)
          && e->key() != Qt::Key_V && e->key() != Qt::Key_X) &&
        !(e->key() == Qt::Key_Right || e->key() == Qt::Key_Left ||
          e->key() == Qt::Key_Up || e->key() == Qt::Key_Down))
    {
        QString content = text(para).trimmed();

        if (hasSelectedText())
        {
            int startPara, endPara, startPos, endPos;
            getSelection(&startPara, &startPos, &endPara, &endPos, 0);
            if (startPara != endPara)
            {
                setSelection(-1, -1, -1, -1, 0);
                return;
            }
            setSelection(-1, -1, -1, -1, 0);
            content.remove(startPos, endPos - startPos);
            pos = startPos;
            const int ascii = e->text().isEmpty() ? 0 : e->text().at(0).unicode();
            if (ascii == 8 || ascii == 127)
                noCheck = true;
        }

        para = paragraphs() - 1;
        setCursorPosition(para, 0);
        while (text(para).length() > 1)
            del();
        setBold(false);
        setAlignment(Qt::AlignLeft);
        insert(content);
        setCursorPosition(para, pos);
    }

    const QString keyText = e->text();
    const int ascii = keyText.isEmpty() ? 0 : keyText.at(0).unicode();

    if (!noCheck)
    {
        switch (e->key())
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            calculateKey();
            break;
        case Qt::Key_Backspace:
            backKey();
            break;
        case Qt::Key_Delete:
            deleteKey();
            break;
        case Qt::Key_Tab:
            insert(" ");
            line.append(" ");
            lineCursor++;
            break;
        case Qt::Key_Right:
            cursorKey(1);
            break;
        case Qt::Key_Left:
            cursorKey(3);
            break;
        case Qt::Key_Up:
            cursorKey(0);
            break;
        case Qt::Key_Down:
            cursorKey(2);
            break;
        default:
            setBold(false);
            if (e->modifiers() == Qt::ControlModifier)
            {
                switch (e->key())
                {
                case Qt::Key_V:
                    paste();
                    break;
                case Qt::Key_C:
                    copy();
                    break;
                case Qt::Key_X:
                    cut();
                    break;
                case Qt::Key_Y:
                    redo();
                    break;
                case Qt::Key_Z:
                    undo();
                    break;
                default:
                    break;
                }
            }
            else
            {
                if (pos == 0 && (keyText == "+" || keyText == "^" || keyText == "*" ||
                                 keyText == "/" || keyText == QString(QChar(0xb2)) || keyText == QString(QChar(0xb3)) ||
                                 keyText == "-" || keyText == getUnicode(ROOTSTRING) ||
                                 keyText == "%" || keyText == "!"))
                {
                    if (!ansDone && ansAvailable)
                    {
                        insert("ans");
                        ansDone = true;
                    }
                }
                if ((e->modifiers() & Qt::KeypadModifier) && ascii == ',')
                    insert(".");
                else
                    insert(keyText);

                const QString paraText = text(para);
                if (autoBrace && ((ascii >= 'A' && ascii <= 'Z') ||
                                  (ascii == 's' && pos > 1 && pos <= paraText.size() &&
                                   paraText[pos - 2] == 'a' && paraText[pos - 1] == 'n')))
                {
                    int var = ascii - 65;
                    if (threadData->dimension[var][0] != 1)
                        insert("[]");
                    if (threadData->dimension[var][1] != 1)
                        insert("[]");
                }
            }
        }
    }
    getCursorPosition(&para, &pos);
}


QMenu* CalcInput::createPopupMenu(const QPoint&)
{
    auto *menu = new QMenu(this);
    menu->addAction(CALCWIDGETC_MENU1, this, [this]{ menuSlot(1); });
    menu->addAction(CALCWIDGETC_MENU2, this, [this]{ menuSlot(2); });
    menu->addSeparator();
    menu->addAction(CALCWIDGETC_MENU3, this, [this]{ menuSlot(3); });
    menu->addAction(CALCWIDGETC_MENU4, this, [this]{ menuSlot(4); });
    menu->addAction(CALCWIDGETC_MENU5, this, [this]{ menuSlot(5); });

    return menu;
}

void CalcInput::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createPopupMenu(event->pos());
    if (menu)
    {
        menu->exec(event->globalPos());
        delete menu;
    }
}

void CalcInput::menuSlot(int item)
{
    switch(item)
    {
        case 1:
            undo();
            break;
        case 2:
            redo();
            break;
        case 3:
            cut();
            break;
        case 4:
            copy();
            break;
        case 5:
            paste();
            break;
    }

}


void CalcInput::calculateKey()
{
    ansAvailable=true;
    ansDone=false;
    textInput("");
    int para;
    int pos;
    getCursorPosition(&para,&pos);
    Number nResult;
    
    setCursorPosition(para,paragraphLength(para));
    QString strResult;
    setBold(true);
    if(text(para).length() <= 1)
    {
        para--;
        setCursorPosition(para,0);
        char*cleanString=nullptr;
        int c;
        for(c=resultParagraphs.GetLen()-1; c>=1; c--)
            if(resultParagraphs[c]-1 != resultParagraphs[c-1])
            {
                QString input=text(resultParagraphs[c]-1);
                cleanString=preprocessor(&input,&pref,false);
                break;
            }
        if(c<=0)
        {
            QString input=text(0);
            cleanString=preprocessor(&input,&pref,false);
        }
        Script s(nullptr,cleanString,&pref,vars,threadData);
        nResult=s.exec();
        if(cleanString!=nullptr)
            free(cleanString);
        setCursorPosition(paragraphs()-1,0);
    }
    else {
        insert(QString("\n"));
        QString input=text(para);
        char*cleanString=preprocessor(&input,&pref,false);
        Script s(nullptr,cleanString,&pref,vars,threadData);
        nResult=s.exec();
        if(cleanString!=nullptr)
            free(cleanString);
    }
    

    
    strResult=formatOutput(nResult,&pref,threadData);
    
    if(nResult.type==NVECTOR)
    {
        threadData->vars[26]=(Number*)realloc(threadData->vars[26],sizeof(Number)*threadData->numlen[nResult.ival]);
        for(int c=0; c<threadData->numlen[nResult.ival];c++)
        {
            convertToFloat(&threadData->vars[nResult.ival][c]);
            threadData->vars[26][c].type=NFLOAT;
            threadData->vars[26][c].fval=threadData->vars[nResult.ival][c].fval;
        }
        threadData->numlen[26]=threadData->numlen[nResult.ival];
        threadData->dimension[26][0]=threadData->dimension[nResult.ival][0];
        threadData->dimension[26][1]=threadData->dimension[nResult.ival][1];
    }
    else if(nResult.type==NMATRIX)
    {
        threadData->vars[26]=(Number*)realloc(threadData->vars[26],sizeof(Number)*threadData->numlen[nResult.ival]);
        for(int c=0; c<threadData->numlen[nResult.ival];c++)
        {
            convertToFloat(&threadData->vars[nResult.ival][c]);
            threadData->vars[26][c].type=NFLOAT;
            threadData->vars[26][c].cval=nullptr;
            threadData->vars[26][c].fval=threadData->vars[nResult.ival][c].fval;
        }
        threadData->numlen[26]=threadData->numlen[nResult.ival];
        threadData->dimension[26][0]=threadData->dimension[nResult.ival][0];
        threadData->dimension[26][1]=threadData->dimension[nResult.ival][1];
    }
    else{
        threadData->vars[26]=(Number*)realloc(threadData->vars[26],sizeof(Number));
        threadData->vars[26][0]=nResult;
        threadData->numlen[26]=1;
        threadData->dimension[26][0]=1;
        threadData->dimension[26][1]=1;
    }
//    while(strResult->length() < (unsigned)(lineLength-3))
//        strResult->insert(0,' ');
    setAlignment(Qt::AlignRight);
    insert(strResult);
    resultParagraphs.NewItem(paragraphs()-1);
    setBold(false);
    insert(QString("\n"));
    setAlignment(Qt::AlignLeft);
    line="";
    lineCursor=0;
    emit calcSignal();
}

void CalcInput::deleteKey()
{
    if(hasSelectedText())
    {
        
        textInput("");
        return;
    }
    else textInput("");

    del();
    line.remove(lineCursor,1);
}
void CalcInput::backKey()
{    
    if(hasSelectedText())
    {
        
        textInput("");
        return;
    }
    else textInput("");

    int para;
    int pos;
    getCursorPosition(&para,&pos);
    if(paragraphLength(para) > 0)
    {
        setCursorPosition(para,pos-1);
        del();

        line.remove(lineCursor-1,1);
        lineCursor--;

    }
}
void CalcInput::clearAll()
{
    ansAvailable=false;
    while(paragraphs() > 1)
        removeParagraph(0);
    removeParagraph(0);
    while(resultParagraphs.GetLen() > 0)
        resultParagraphs.DeleteItem(0);
    for(int c=0; c<VARNUM; c++)
    {
        threadData->vars[c]=(Number*)realloc(threadData->vars[c],sizeof(Number));
        convertToFloat(threadData->vars[c]);
        threadData->numlen[c]=1;
        threadData->dimension[c][0]=threadData->dimension[c][1]=1;
    }
}
void CalcInput::cursorKey(int key)
{
    //    0    =    up
    //    1    =    right
    //    2    =    down
    //    3    =    left
    int para;
    int pos;
    getCursorPosition(&para,&pos);
    
    
    if(key==1 && pos < paragraphLength(para))
    {
        setCursorPosition(para,pos+1);
        lineCursor++;
    }
    if(key==3 && pos > 0)
    {
        setCursorPosition(para,pos-1);
        lineCursor--;
    }
    if(key==0)
        setCursorPosition(para-1,pos);
    if(key==2 && para < paragraphs()-1)
        setCursorPosition(para+1,pos);
}



void CalcInput::resizeEvent(QResizeEvent*)
{

    QFontMetrics fontSize=fontMetrics();
    charLength=fontSize.size(0,QStringLiteral("m")).width();
    lineLength=(width())/charLength-1;

}
/*
void CalcInput::customEvent(QEvent *ev)
{
    if(scriptExec && script!=nullptr)
    {
        if(ev->type() == 50000)
        {
            char*text=(char*)ev->data();
        //    MessageBox("Text: "+QString(text));
            insert(QString(text));
        }
        else if(ev->type() == 50010)        //script stopped
        {
            insert(QString("\n"));
            insert(QString("*** finish ***"));
            insert(QString("\n"));
            scriptExec=false;
            script->wait();
            delete script;
            script=nullptr;
        }
    }
}
*/
void CalcInput::cursorSlot()
{
    int para;
    int pos;
    getCursorPosition(&para,&pos);
    lineCursor=pos;
    emit cursorMoved(para, pos);
}





void CalcInput::textInput(const QString &inputText)
{
    int para;
    int pos;
    getCursorPosition(&para,&pos);
    if(para != paragraphs()-1)
    {
        QString content=text(para).trimmed();
        
        if(hasSelectedText())
        {
            int startPara,endPara,startPos,endPos;
            getSelection(&startPara,&startPos,&endPara,&endPos,0);
            if(startPara != endPara)
            {
                setSelection(-1,-1,-1,-1,0);
                return;
            }
            else {
                setSelection(-1,-1,-1,-1,0);
                content.remove(startPos,endPos-startPos);
                pos=startPos;
            }
        }        
        
        para=paragraphs()-1;
        setCursorPosition(para,0);
        while(text(para).length() > 1)
            del();
        setBold(false);
        insert(content);
        setCursorPosition(para,pos);
    }
    if(!inputText.isEmpty() && pos==0 && ( inputText == "+" || inputText[0] == '^' || inputText == "*" ||
          inputText == "/" || inputText == "\xb2" || inputText == "\xb3" ||
          inputText[0] == '-' || inputText == getUnicode(ROOTSTRING) ||
          inputText == "%" || inputText == "!"))
    {
        if(!ansDone && ansAvailable)
        {
            insert("ans");
            ansDone=true;
        }
    }

    insert(inputText);
    
    if(autoBrace && !inputText.isEmpty() && ((inputText[0]>='A' && inputText[0]<='Z') || inputText=="ans"))
    {
        int var=(int)(inputText.at(0).unicode())-65;

        if(threadData->dimension[var][0]!=1)
            insert("[]");
        if(threadData->dimension[var][1]!=1)
            insert("[]");
    }
        
}
/*

void CalcInput::scriptSlot(QString*code)
{
    textInput("");
    int para;
    int pos;
    
    setCursorPosition(paragraphs()-1,paragraphLength(paragraphs()-1));
    setAlignment(Qt::AlignLeft);
    setBold(false);
    insert(QString("\n"));
    insert(QString("*** script ***"));
    insert(QString("\n"));
    QString *strResult;
    setBold(true);


    char*cleanString=checkString(*code,&pref,vars);
    perror("cleanString: "+QString(cleanString));
    scriptObject=new Script(nullptr,cleanString,&pref,vars,this);
//    Number result=testScript.exec();
    scriptExec=true;
    script=new ScriptThread(scriptObject,this);
    delete[]cleanString;
    script->start();
    

    line="";
    lineCursor=0;
    
}
*/





void CalcInput::setPref(Preferences newPref)
{
    pref = newPref;
}

int CalcInput::paragraphs() const
{
    return document()->blockCount();
}

QString CalcInput::text(int para) const
{
    QTextBlock block = document()->findBlockByNumber(para);
    return block.isValid() ? block.text() : QString();
}

int CalcInput::paragraphLength(int para) const
{
    return text(para).length();
}

void CalcInput::getCursorPosition(int *para, int *pos) const
{
    QTextCursor cur = textCursor();
    if (para)
        *para = cur.blockNumber();
    if (pos)
        *pos = cur.position() - cur.block().position();
}

void CalcInput::setCursorPosition(int para, int pos)
{
    QTextBlock block = document()->findBlockByNumber(para);
    if (!block.isValid())
        return;

    const int clampedPos = qBound(0, pos, block.length());
    QTextCursor cur(block);
    cur.setPosition(block.position() + clampedPos);
    setTextCursor(cur);
}

bool CalcInput::hasSelectedText() const
{
    return textCursor().hasSelection();
}

void CalcInput::getSelection(int *startPara, int *startPos, int *endPara, int *endPos, int) const
{
    QTextCursor cur = textCursor();
    if (!cur.hasSelection())
    {
        if (startPara)
            *startPara = cur.blockNumber();
        if (endPara)
            *endPara = cur.blockNumber();
        const int localPos = cur.position() - cur.block().position();
        if (startPos)
            *startPos = localPos;
        if (endPos)
            *endPos = localPos;
        return;
    }

    const int start = cur.selectionStart();
    const int end = cur.selectionEnd();
    QTextBlock startBlock = document()->findBlock(start);
    QTextBlock endBlock = document()->findBlock(end);
    if (startPara)
        *startPara = startBlock.isValid() ? startBlock.blockNumber() : 0;
    if (startPos)
        *startPos = start - (startBlock.isValid() ? startBlock.position() : 0);
    if (endPara)
        *endPara = endBlock.isValid() ? endBlock.blockNumber() : 0;
    if (endPos)
        *endPos = end - (endBlock.isValid() ? endBlock.position() : 0);
}

void CalcInput::setSelection(int startPara, int startPos, int endPara, int endPos, int)
{
    QTextCursor cur = textCursor();
    if (startPara < 0 || endPara < 0)
    {
        cur.clearSelection();
        setTextCursor(cur);
        return;
    }

    QTextBlock startBlock = document()->findBlockByNumber(startPara);
    QTextBlock endBlock = document()->findBlockByNumber(endPara);
    if (!startBlock.isValid() || !endBlock.isValid())
        return;

    const int start = startBlock.position() + qMax(0, startPos);
    const int end = endBlock.position() + qMax(0, endPos);
    cur.setPosition(start);
    cur.setPosition(end, QTextCursor::KeepAnchor);
    setTextCursor(cur);
}

void CalcInput::removeParagraph(int para)
{
    QTextBlock block = document()->findBlockByNumber(para);
    if (!block.isValid())
        return;
    QTextCursor cur(block);
    cur.select(QTextCursor::BlockUnderCursor);
    cur.removeSelectedText();
    cur.deleteChar();
}

void CalcInput::del()
{
    QTextCursor cur = textCursor();
    cur.deleteChar();
    setTextCursor(cur);
}

void CalcInput::insert(const QString &text)
{
    QTextCursor cur = textCursor();
    cur.insertText(text);
    setTextCursor(cur);
}
