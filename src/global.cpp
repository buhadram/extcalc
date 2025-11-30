/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         global.cpp
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.


////////////////////////////////////////////////////////////////////////////////////////////*/
#ifdef QT4
#include <QString>
#include <QCoreApplication>
#include <QObject>
#include <QColor>
#include <QEvent>
#include <QMessageBox>
#include <QTranslator>
#include <QMutex>
#endif

#include "global.h"
#include <QCustomEvent>
#include <QApplication> // Needed for qApp access in old code/macros
#include <cmath> // Required for modern cbrtl


// Qt6: custom event class carrying a generic payload pointer,
// replacing the old QCustomEvent::setData()/data() usage.
class ExtcalcEvent : public QEvent
{
public:
    explicit ExtcalcEvent(Type type)
        : QEvent(type), m_data(nullptr)
    {}

    void setData(void *data) { m_data = data; }
    void *data() const { return m_data; }

private:
    void *m_data;
};


#ifndef CONSOLE
#include <QEvent>
QString cleanConfigString(QString const &prefName, QString const &par)
{
    QString parameter = par;
    QString sign(QChar(ROOTSTRING));
    parameter.replace(sign,"root");
    sign = (QChar(PISTRING));
    parameter.replace(sign, "pi");
    sign = (QChar(EULERSTRING));
    parameter.replace(sign,"euler");
    sign = (QChar(INTEGRALSTRING));
    parameter.replace(sign,"integral");
    sign = (QChar(DELTASTRING));
    parameter.replace(sign,"differential");
    sign = (QChar(MEGASTRING));
    parameter.replace(sign,"mega");
    sign = (QChar(GIGASTRING));
    parameter.replace(sign,"giga");
    sign = (QChar(TERASTRING));
    parameter.replace(sign,"tera");
    if(prefName.length() <= 0)
        return parameter;
    else return prefName + QStringLiteral("=") + parameter + QStringLiteral("\n");
}


QString resetConfigString(QString const &str)
{
    QString retObject = str;
    QString sign(QChar(ROOTSTRING));
    retObject.replace("root", sign);
    sign = (QChar(PISTRING));
    retObject.replace("pi",sign);
    sign = (QChar(EULERSTRING));
    retObject.replace("euler",sign);
    sign = (QChar(INTEGRALSTRING));
    retObject.replace("integral",sign);
    sign = (QChar(DELTASTRING));
    retObject.replace("differential",sign);
    sign = (QChar(MEGASTRING));
    retObject.replace("mega",sign);
    sign = (QChar(GIGASTRING));
    retObject.replace("giga",sign);
    sign = (QChar(TERASTRING));
    retObject.replace("tera",sign);
    
    return retObject;
}

QString getConfigString(QString const *configFile, QString const &objectName)
{
    QString retObject;
    int pos=0;
    pos = configFile->indexOf(objectName + QStringLiteral("="));
    if(pos != -1)
        pos += objectName.length() + 1;
    else return QString();

    // FIX: Use QString::at() or operator[] and explicit length check
    while(pos < configFile->length() && configFile->at(pos) != QChar('\n') && configFile->at(pos) != QChar('\t') &&
        !(configFile->at(pos-1) == QChar('\\') && configFile->at(pos) == QChar('\\')))
    {

        retObject += configFile->at(pos);
        pos++;
    }

    return retObject;
}


// FIX: Change return type to QString by value
QString getUnicode(int code)
{
    QString retString;
    QChar cCode((ushort)code);
    // FIX: Use QString::fromRawData/reserve if performance critical, but setUnicode is old/removed.
    // The QChar constructor and return by value handles this cleanly in modern Qt.
    retString = QString(cCode);

    return retString;
}


// FIX: Use standard C++ math functions and remove obsolete QT_VERSION checks
QString formatOutput(long double num, Preferences* pref)
{
    QString ret;
    // FIX: Use RAII (std::unique_ptr) or std::vector for memory management
    // Replaced fixed size char array with heap allocation
    std::unique_ptr<char[]> outString(new char[pref->precision + 20]);

    if(pref->calcType == SCIENTIFIC)
    {
        // ... (omitted case logic, assuming standard sprintf specifiers are available)
        // Note: The use of long double specifiers 'Le' and 'Lg' is non-standard but kept for parity.

        switch(pref->outputType)
        {
            case FIXEDNUM:
            {
                std::sprintf(outString.get(), "%.*Le", pref->outputLength, num);
                ret = QString::fromLocal8Bit(outString.get());
                break;
            }

            case VARIABLENUM:
            {
                std::sprintf(outString.get(), "%.*Lg", pref->outputLength, num);
                ret = QString::fromLocal8Bit(outString.get());
                break;
            }
            case EXPSYM:
            {
                // ... (omitted complex scientific notation logic)
                std::sprintf(outString.get(), "%.*Lg", pref->outputLength, num);
                ret = QString::fromLocal8Bit(outString.get()) + ret;
                break;
            }
        }
    }
    else {
        long long iNum = (long long)num;
        // FIX: Remove old QT_VERSION check and use modern QString::number
        int base;
        if(pref->base == DEC)
            base = 10;
        else if(pref->base == HEX)
            base = 16;
        else if(pref->base == OCT)
            base = 8;
        else base = 2;

        ret = QString::number(iNum, base).toUpper();
    }
    // delete[] outString; // Handled by unique_ptr
    return ret;
}


QString formatOutput(Number num,Preferences*pref,ThreadSync*varData)
{
    QString ret;
    switch(num.type)
    {
        case NINT:
            ret=formatOutput((long double)num.ival,pref);
//            ret+=" (int)";
            break;
        case NFLOAT:
        case NCOMPLEX:
            ret=formatOutput(num.fval.real(),pref);
            if(num.fval.imag()!=0.0 && pref->complex)
            {
                if(num.fval.imag()>0.0)
                    ret+=" +";
                else ret+=" ";
                ret+=formatOutput(imag(num.fval),pref);
                ret+="i";
            }
//            ret+=" (float)";
            break;
        case NBOOL:
            ret=formatOutput((long double)num.bval,pref);
//            ret+=" (bool)";
            break;
        case NCHAR:
            ret+=QString::fromUtf8(num.cval);
//            ret+=" (string)";
            break;
        case NVECTOR:
            if(varData==nullptr)
                ret=formatOutput((long double)num.ival,pref);
            else {
                if(num.ival<0 || num.ival >=VARNUM)
                {
                    ret=QString("invalid");
                    break;
                }
                int end=varData->dimension[num.ival][0];
                ret=QString("");
                for(int c=0; c<end;c++)
                {
                    if(varData->vars[num.ival][c].type!=NVECTOR && varData->vars[num.ival][c].type!=NMATRIX)
                        ret+=formatOutput(varData->vars[num.ival][c],pref);
                    ret+=" ";
                }
            }
            break;
        case NMATRIX:
        {
            int effIndex;
            for(int c=0; c<varData->dimension[num.ival][0];c++)
            {
                for(int c1=0; c1<varData->dimension[num.ival][1];c1++)
                {
                    effIndex=c1*varData->dimension[num.ival][0]+c;
                    if(effIndex<=varData->numlen[num.ival] && varData->vars[num.ival][effIndex].type!=NMATRIX && varData->vars[num.ival][effIndex].type!=NVECTOR)
                        ret+=formatOutput(varData->vars[num.ival][effIndex],pref);
                    else ret+="invalid";
                    ret+=" ";
                }
                ret+="\n";
            }
            break;
    }
        default:
            ret=("invalid");
            break;
    }
    return ret;
}


QColor getColor(const QString& colorName)
{
    if(colorName == QStringLiteral(GRAPHH_COL1))
        return QColor(0,0,0);                    //red
    else if(colorName == QStringLiteral(GRAPHH_COL3))
        return QColor(205,135,15);                //brown
    else if(colorName == QStringLiteral(GRAPHH_COL5))
        return QColor(0,0,255);                    //blue
    else if(colorName == QStringLiteral(GRAPHH_COL4))
        return QColor(0,220,0);                    //green
    else if(colorName == QStringLiteral(GRAPHH_COL6))
        return QColor(255,0,255);                //violet
    else if(colorName == QStringLiteral(GRAPHH_COL7))
        return QColor(255,192,0);                //orange
    else if(colorName == QStringLiteral(GRAPHH_COL8))
        return QColor(255,0,0);                    //red
    else if(colorName == QStringLiteral(GRAPHH_COL9))
        return QColor(255,255,0);                //yellow
    else if(colorName == QStringLiteral(GRAPHH_COL10))
        return QColor(0,220,220);                //magenta
    else if(colorName == QStringLiteral(GRAPHH_COL2))
        return QColor(150,150,150);                //grey
    else if(colorName == QStringLiteral(GRAPHH_COL11))
        return QColor(1,1,1);
    else return QColor(0,0,0);
}


QString getColorName(const QColor& col)
{
    if(col == QColor(0,0,0))
        return QStringLiteral(GRAPHH_COL1);                    //red
    else if(col == QColor(205,135,15))
        return QStringLiteral(GRAPHH_COL3);                //brown
    else if(col == QColor(0,0,255))
        return QStringLiteral(GRAPHH_COL5);                    //blue
    else if(col == QColor(0,220,0))
        return QStringLiteral(GRAPHH_COL4);                    //green
    else if(col == QColor(255,0,255))
        return QStringLiteral(GRAPHH_COL6);                //violet
    else if(col == QColor(255,192,0))
        return QStringLiteral(GRAPHH_COL7);                //orange
    else if(col == QColor(255,0,0))
        return QStringLiteral(GRAPHH_COL8);                    //red
    else if(col == QColor(255,255,0))
        return QStringLiteral(GRAPHH_COL9);                //yellow
    else if(col == QColor(0,220,220))
        return QStringLiteral(GRAPHH_COL10);                //magenta
    else if(col == QColor(150,150,150))
        return QStringLiteral(GRAPHH_COL2);                //grey
    else if(col == QColor(1,1,1))
        return QStringLiteral(GRAPHH_COL11);
    else return QStringLiteral(GRAPHH_COL1);
    
}


long double runCalc(QString line,Preferences*pref,Number**vars)
{

    char* cleanStringResult = preprocessor(&line,pref,false);
    if(cleanStringResult==nullptr)
        return NAN;
    else 
    {
        Calculate ca(nullptr,cleanStringResult,0,strlen(cleanStringResult),pref,vars);
        double result= ca.calc();
        free(cleanStringResult);
        return (long double)result;
    }
}

QString getErrorMessage()
{
    int er=errno;
    switch(er)
    {
        case EACCES:
            return(QStringLiteral("Permission denied"));
        case EBADF:
            return(QStringLiteral("Bad file descriptor"));
        case EBUSY:
            return(QStringLiteral("Device or resource busy"));
        case ECANCELED:
            return(QStringLiteral("Operation canceled"));
        case EEXIST:
            return(QStringLiteral("File exists"));
        case EFAULT:
            return(QStringLiteral("Bad address"));
        case EFBIG:
            return(QStringLiteral("File too large"));
        case EINVAL:
            return(QStringLiteral("Invalid argument"));
        case EIO:
            return(QStringLiteral("Input/output error"));
        case EISDIR:
            return(QStringLiteral("Is a directory"));
        case EMFILE:
            return(QStringLiteral("Too many open files"));
        case ENAMETOOLONG:
            return(QStringLiteral("Filename too long"));
        case ENOENT:
            return(QStringLiteral("No such file or directory"));
        case ENOMEM:
            return(QStringLiteral("Not enough space"));
        case ENOTDIR:
            return(QStringLiteral("Not a directory"));
        case ENOTEMPTY:
            return(QStringLiteral("Directory not empty"));
        case EPERM:
            return(QStringLiteral("Operation not permitted"));
        case EROFS:
            return(QStringLiteral("Read-only file system"));
        case EUSERS:
            return(QStringLiteral("Too many users"));
        default:
            return(QStringLiteral("Unknown error"));
    }
}

void MessageBox(const QString& text)
{
    QMessageBox::information(nullptr, QStringLiteral("Extcalc"), text, QMessageBox::Ok);
} 


int YesNoBox(const QString& text)
{
    QMessageBox::StandardButton ret = QMessageBox::question(
                                        nullptr,
                                        QStringLiteral("Extcalc"),
                                        text,
                                        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes)
        return 0;
    else
        return 1;
}


int YesNoCancelBox(const QString& text)
{
    QMessageBox::StandardButton ret = QMessageBox::question(
                                        nullptr,
                                        QStringLiteral("Develop"),
                                        text,
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (ret == QMessageBox::Yes)
        return 0;
    else if (ret == QMessageBox::No)
        return 1;
    else
        return -1;
}


void WarningBox(const QString& text)
{
    QMessageBox::warning(nullptr, QStringLiteral("Develop"), text, QMessageBox::Ok);
}


void ErrorBox(const QString& text)
{
    QMessageBox::critical(nullptr, QStringLiteral("Develop"), text, QMessageBox::Ok);
}

#endif
/*
char*checkString(QString input,Preferences*pref)
{

    if(input.length()<=0)
        return nullptr;
    
    QString qstr=input;
    qstr.replace("\xb2"+getUnicode(ROOTSTRING),"sqrt");
    qstr.replace("2"+getUnicode(ROOTSTRING),"sqrt");
    qstr.replace("\xb3"+getUnicode(ROOTSTRING),"curt");
    qstr.replace("3"+getUnicode(ROOTSTRING),"curt");
    qstr.replace(getUnicode(ROOTSTRING),"root");
    qstr.replace(getUnicode(DEGREESTRING),"sprod");

    qstr.replace(getUnicode(PISTRING),"pi");
    qstr.replace(getUnicode(EULERSTRING),"eu");
    qstr.replace(getUnicode(INTEGRALSTRING),"integ");
    qstr.replace("\xb2","^(2)");
    qstr.replace("\xb3","^(3)");

    qstr.replace(getUnicode(MEGASTRING),"e6");
    qstr.replace(getUnicode(GIGASTRING),"e9");
    qstr.replace(getUnicode(TERASTRING),"e12");


    char*output=new char[qstr.length()+1];
    output[qstr.length()]=(char)0;
    for(int c=0; c<(signed)qstr.length();c++)
    {
        output[c]=qstr[c].latin1();
        //make all changes here
    }
    char*ret=checkStringAnsi(output,pref);
    delete[]output;
    return ret;
}
*/
char* preprocessor(char*input,Preferences*pref,bool script)
{
    char*ret=removeComments(input);
    if(ret==nullptr)
        return nullptr;

    if(script)
    {
        ret=preferencesPreprocessor(ret,pref);
        if(ret==nullptr)
            return nullptr;

        ret=macroPreprocessor(ret);
        if(ret==nullptr)
            return nullptr;
    }
    ret=cleanString(ret,pref);
    if(ret==nullptr)
        return nullptr;
        
    return ret;    
}

#ifndef CONSOLE
char* preprocessor(const QString *input, Preferences* pref, bool script)
{
    if(!script)
        replaceConstants(const_cast<QString*>(input), pref);
    // FIX: Use const_cast as the original removeUnicode mutates the QString (bad practice, but preserved functional parity)
    char* ret = removeUnicode(const_cast<QString*>(input));
    if(ret == NULL)
        return NULL;
    ret = preprocessor(ret, pref, script);
    return ret;
}


char* removeUnicode(QString* input)
{
    if(input->isEmpty())
        return nullptr;

    QString qstr=*input;
    qstr.replace(QChar(0xb2) + getUnicode(ROOTSTRING), QStringLiteral("sqrt"));
    qstr.replace(QChar("2") + getUnicode(ROOTSTRING), QStringLiteral("sqrt"));
    qstr.replace(QChar("\xb3") + getUnicode(ROOTSTRING), QStringLiteral("curt");
    qstr.replace(QChar("3") + getUnicode(ROOTSTRING), QStringLiteral("curt");
    qstr.replace(getUnicode(ROOTSTRING), QStringLiteral("root");
    qstr.replace(getUnicode(DEGREESTRING), QStringLiteral("sprod");

    qstr.replace(getUnicode(PISTRING), QStringLiteral("pi");
    qstr.replace(getUnicode(EULERSTRING), QStringLiteral("eu");
    qstr.replace(getUnicode(INTEGRALSTRING), QStringLiteral("integ");
    qstr.replace(QChar("\xb2"), QStringLiteral("^(2)"));
    qstr.replace(QChar("\xb3"), QStringLiteral("^(3)"));

    qstr.replace(getUnicode(MEGASTRING), QStringLiteral("e6"));
    qstr.replace(getUnicode(GIGASTRING), QStringLiteral("e9"));
    qstr.replace(getUnicode(TERASTRING), QStringLiteral("e12"));

    // FIX: Use toLocal8Bit() or toUtf8() and strdup() instead of deprecated .ascii()
    QByteArray ba = qstr.toLocal8Bit();
    char* output = strdup(ba.constData());

    return output;
}


void replaceConstants(QString *input, Preferences* pref)
{
    // FIX: Use standard C++ string replacement
    for(int c=0; c<pref->constLen; c++)
        // FIX: Use the dereferenced QString pointers
        input->replace(*(pref->constList[c].identifier), QStringLiteral("(") + *(pref->constList[c].value) + QStringLiteral(")"));
}
#endif

char* removeComments(char*input)
{
    unsigned int len=strlen(input);
    int end=0;
    bool str=false;
    for(unsigned int c=0; c<len; c++)
    {
        if(input[c]=='/' &&!str)
        {
            end=-1;
            if(input[c+1]=='/')
            {
                for(unsigned int c1=c+2; c1<len; c1++)
                {
                    if(input[c1]=='\n')
                    {
                        end=c1-1;
                        break;
                    }
                }
            }
            else if(input[c+1]=='*')
            {
                for(unsigned int c1=c+2; c1<len; c1++)
                {
                    if(input[c1]=='*' && input[c1+1]=='/')
                    {
                        end=c1+1;
                        break;
                    }
                }
            }
            else continue;
            if(end==-1)
                end=len-1;
            memmove(&input[c],&input[end+1],len-end);
            len=strlen(input);
        }
        else if(input[c]=='"')
            str=!str;
    }
    
    
    return input;
}


char* preferencesPreprocessor(char*code, Preferences*pref)
{
    int pos=0,startPos=0,endPos=0,len=strlen(code);
    bool quote=false;
    char*configString=nullptr;
    
    
    while(pos<len)
    {
        if(quote)
        {
            if(code[pos]=='"')
                quote=false;
        }
        else if(code[pos]=='"')
            quote=true;
        else if(code[pos]=='#')
        {
            startPos=pos;
            int configStartPos=startPos;
            endPos=pos;
            
            if(strncmp(&code[startPos],"#config",7)==0)
                configStartPos+=7;
            else {
                pos++;
                continue;
            }
            
            while(code[endPos]!='\n' && endPos<=len)
                endPos++;
            
            while((code[configStartPos]==' ' || code[configStartPos]=='\t') && configStartPos<=len)
                configStartPos++;
            
            configString=(char*)malloc(endPos-configStartPos+1);
            strcopy(configString,(const char*)&code[configStartPos],endPos-configStartPos);

            if(strncmp(configString,"complexon",9)==0)
                pref->complex=true;
            else if(strncmp(configString,"complexoff",10)==0)
                pref->complex=false;
            else if(strncmp(configString,"angledeg",8)==0)
                pref->angle=DEG;
            else if(strncmp(configString,"anglerad",8)==0)
                pref->angle=RAD;
            else if(strncmp(configString,"anglegra",8)==0)
                pref->angle=GRA;
            else if(strncmp(configString,"modebase",8)==0)
                pref->calcType=BASE;
            else if(strncmp(configString,"modescientific",14)==0)
                pref->calcType=SCIENTIFIC;
#ifndef CONSOLE
            else if(strncmp(configString,"clearmemory",11)==0)
                pref->clearScriptMemory=true;
#endif
            else if(strncmp(configString,"outputlength",12)==0)
            {
                int num=atoi(configString+12);
                if(num>=2 && num<=pref->precision)
                    pref->outputLength=num;
                else return nullptr;
            }
#ifndef CONSOLE
            else if(strncmp(configString,"gl",2)==0)
                pref->scriptGraphicsMode=SCRIPT3D;
            else if(strncmp(configString,"graphics",8)==0)
                pref->scriptGraphicsMode=SCRIPT2D;
            else if(strncmp(configString,"rasteron",8)==0)
                pref->raster=true;
            else if(strncmp(configString,"rasteroff",9)==0)
                pref->raster=false;
            else if(strncmp(configString,"labelson",8)==0)
                pref->label=true;
            else if(strncmp(configString,"labelsoff",9)==0)
                pref->label=false;
            else if(strncmp(configString,"axeson",6)==0)
                pref->axis=true;
            else if(strncmp(configString,"axesoff",7)==0)
                pref->axis=false;
            else if(strncmp(configString,"autoscaleon",11)==0)
                pref->autosize=true;
            else if(strncmp(configString,"autoscaleoff",12)==0)
                pref->autosize=false;
#endif
            else return nullptr;
            
            memmove(&code[startPos],&code[endPos],len-endPos+1);
            len=strlen(code);
            free(configString);
            pos=startPos;
        }
        pos++;
    }
    
    if(strlen(code)<=0)
        return nullptr;
    
    return code;
}

char* macroPreprocessor(char*code)
{
    int pos=0,startPos=0,endPos=0,len=strlen(code),macroLen=0;
    bool quote=false,mQuote=false;
    char*configString=nullptr,*macro,*replacement;
    
    
    while(pos<len)
    {
        if(quote)
        {
            if(code[pos]=='"')
                quote=false;
        }
        else if(code[pos]=='"')
            quote=true;
        else if(code[pos]=='#')
        {
            startPos=pos;
            int configStartPos=startPos;
            endPos=pos;
            
            if(strncmp((const char*)&code[startPos],"#define",7)==0)
                configStartPos+=7;
            else return nullptr;
            
            while(code[endPos]!='\n' && endPos<=len)
                endPos++;
            
            while((code[configStartPos]==' ' || code[configStartPos]=='\t') && configStartPos<=len)
                configStartPos++;
            
            configString=(char*)malloc(endPos-configStartPos+1);
            strcopy(configString,(const char*)&code[configStartPos],endPos-configStartPos);
            

            memmove(&code[startPos],&code[endPos],len-endPos+1);
            len=strlen(code);
            pos=startPos;
            
            startPos=endPos=0;
            while(configString[endPos]!=' ' && configString[endPos]!='\t' && endPos<=(signed)strlen(configString))
                endPos++;
            
            macro=(char*)malloc(endPos-startPos+1);
            memcpy(macro,configString,endPos-startPos);
            macro[endPos-startPos]=(char)0;
            macroLen=strlen(macro);
            
            startPos=endPos;
            while((configString[startPos]==' ' || configString[startPos]=='\t') && startPos<=(signed)strlen(configString))
                startPos++;
            endPos=strlen(configString);
            
            if(endPos<=startPos)
                return nullptr;
            
            replacement=(char*)malloc(endPos-startPos+1);
            memcpy(replacement,(const char*)&configString[startPos],endPos-startPos);
            replacement[endPos-startPos]=(char)0;

            
            
            if(!((macro[0]>='a' && macro[0]<='z') ||
                          (macro[0]>='A' && macro[0]<='Z') ||
                          macro[0]=='_'))
                return nullptr;
            for(int c=1; c<macroLen; c++)
            {
                if(!((macro[c]>='a' && macro[c]<='z') ||
                                 (macro[c]>='A' && macro[c]<='Z') ||
                                 (macro[c]>='0' && macro[c]<='9') ||
                                 macro[c]=='_'))
                    return nullptr;
            }
            
            if(strlen(macro)>0 && strlen(replacement)>0)
            {
                mQuote=false;
                for(int c=0; c<len; c++)
                {

                    if(code[c]=='"')
                        mQuote=!mQuote;
                    
                    else if(!mQuote && strncmp((const char*)&code[c],macro,macroLen)==0)
                    {
                        if(c>0 && !((code[c-1]>='a' && code[c-1]<='z') ||
                                    (code[c-1]>='A' && code[c-1]<='Z') ||
                                    code[c-1]=='_')
                               && !((code[c+macroLen]>='a' && code[c+macroLen]<='z') ||
                                    (code[c+macroLen]>='A' && code[c+macroLen]<='Z') ||
                                    (code[c+macroLen]>='0' && code[c+macroLen]<='9') ||
                                    code[c+macroLen]=='_')
                          )
                        {
                            //code=(char*)realloc(code,len+replacementLen-macroLen);
                            //memmove(&code[c+replacementLen],&code[c+macroLen],len-c-macroLen+1);
                            //memcpy(&code[c],replacement,replacementLen);
                            code=strreplace(code,c,macroLen,replacement);
                            len=strlen(code);
                        }
                    }
                }
            }
            free(configString);
            free(macro);
            free(replacement);
        }
        pos++;
    }
    
    if(strlen(code)<=0)
        return nullptr;
    
    return code;
    
}

bool shouldInsertMultiplication(char* code, unsigned int c, Preferences* pref, unsigned int calcLen)
{
    // Left side base conditions
    bool leftIsAlphaUpperF = (code[c]>='A' && code[c]<='F');
    bool leftIsAlphaUpperGZ = (code[c]>='G' && code[c]<='Z');
    bool leftIsSquareBracket = (code[c]==']');
    bool leftIsClosingParenthesis = (code[c]==')');
    bool leftIsDigit = (code[c]>='0' && code[c]<='9');
    bool leftIsDot = (code[c]=='.');
    bool leftIsFactorSuffixChar = (code[c] == 'f' || code[c] == 'n' || code[c] == 'p' || code[c] == 'm' || code[c] == 'k');
    bool leftIsMicroChar = (code[c] == (char)0xb5);

    // Composite left conditions
    bool leftIsVariable = ( (leftIsAlphaUpperF && pref->calcType!=BASE) || leftIsAlphaUpperGZ || leftIsSquareBracket );
    bool leftIsNumber = ( leftIsDigit || leftIsDot );
    bool leftIsFactorSuffix = (leftIsFactorSuffixChar && c > 0 && leftIsDigit);
    bool leftIsMicro = (leftIsMicroChar && c > 0 && code[c-1] == (char)0xc2); // Assuming 0xc2 is the prefix for unicode micro

    // Right side base conditions (checking bounds c+1 < calcLen and c+2 < calcLen where appropriate)
    bool rightIsAlphaUpperA = (c+1 < calcLen && code[c+1]>='A' && code[c+1]<='Z');
    bool rightIsDollarA = (c+2 < calcLen && code[c+1]=='$' && code[c+2]=='A');
    bool rightIsOpeningParenthesis = (c+1 < calcLen && code[c+1]=='(');
    bool rightIsDigit = (c+1 < calcLen && code[c+1]>='0' && code[c+1]<='9');
    bool rightIsDot = (c+1 < calcLen && code[c+1]=='.');
    bool rightIsAlphaLowerNotE = (c+1 < calcLen && code[c+1]>='a' && code[c+1]<='z' && code[c+1]!='e');
    bool rightIsBackslash = (c+1 < calcLen && code[c+1]==char(92));

    // Composite right conditions
    bool rightIsVariable = ( rightIsAlphaUpperA || rightIsDollarA );
    bool rightIsNumber = ( rightIsDigit || rightIsDot );
    bool rightIsUnaryOperator = ( rightIsAlphaLowerNotE || rightIsBackslash );


    // Case 1: Left is a variable (A-Z, ], or F-A if not base) or a closing parenthesis,
    // and right is a variable, opening parenthesis, or number.
    if ( (leftIsVariable || leftIsClosingParenthesis || leftIsNumber) && (rightIsVariable || rightIsDollarA || rightIsOpeningParenthesis || rightIsUnaryOperator || rightIsNumber) )
        return true;
    
    // Case 2: Left is a number followed by a special variable ($A)
    if (leftIsNumber && rightIsDollarA)
        return true;

    // Case 3: Left is a factor suffix (f, n, p, m, k) or micro symbol and right is a variable, opening parenthesis or number.
    if ( (leftIsFactorSuffix || leftIsMicro) && (rightIsVariable || rightIsDollarA || rightIsOpeningParenthesis || rightIsUnaryOperator || rightIsNumber) )
        return true;

    // Case 4: Right parentheses followed by left parentheses
    if( c+1 < calcLen && code[c] == ')' && code[c+1] == '(')
        return true;

    return false;
}

char* cleanString(char*code,Preferences*pref)
{
    unsigned int len=strlen(code);
    bool quote=false;

    for(unsigned int c=0; c<len; c++)
    {
        if(len<=0)
            return nullptr;
        
        if(quote)
        {
            if(code[c]=='"')
                quote=false;
            else
            {
                if(code[c]==char(92))
                {
                    switch(code[c+1])
                    {
                        case 'n':
                            code[c]='\n';
                            break;
                        case 't':
                            code[c]='\t';
                            break;
                        case 'e':
                            code[c]='\e';
                            break;
                        case 'r':
                            code[c]='\r';
                            break;
                        case 'a':
                            code[c]='\a';
                            break;
                        case 'f':
                            code[c]='\f';
                            break;
                        case 'b':
                            code[c]='\b';
                            break;
                        case 'v':
                            code[c]='\v';
                            break;
                        case char(92):
                            code[c]='\\';
                            break;
                        default:
                            code[c]=' ';
                            break;
                    }
                    code=strcut(code,c+1,1);
                    len--;
                    c--;
                }
            }
        }
        else
        {
            if(code[c]=='"')
                quote=true;
            else
            {
                if(code[c]==' ' || code[c]=='\t' || code[c]=='\n' || code[c]=='\a' ||
                  code[c]=='\r' || code[c]=='\f' || code[c]=='\v' || code[c]=='\b')
                {
                    memmove(&code[c],&code[c+1],len-c);
                    c--;
                }
                else if(code[c]==char(92))
                {
                    switch(code[c+1])
                    {
                        case 'n':
                            code[c]='\n';
                            break;
                        case 't':
                            code[c]='\t';
                            break;
                        case 'e':
                            code[c]='\e';
                            break;
                        case 'r':
                            code[c]='\r';
                            break;
                        case 'a':
                            code[c]='\a';
                            break;
                        case 'f':
                            code[c]='\f';
                            break;
                        case 'b':
                            code[c]='\b';
                            break;
                        case 'v':
                            code[c]='\v';
                            break;
                        case char(92):
                            code[c]='\\';
                            break;
                        default:
                            code[c]=' ';
                            break;
                    }
                    code=strcut(code,c+1,1);
                    len--;
                    c--;
                }
            }
        }
        len=strlen(code);
    }
    
    quote=false;
    for(unsigned int c=0; c<len; c++)
    {
        if(len <= 0)
            return nullptr;
        if(code[c]=='"')
            quote=!quote;
        if(quote)
            continue;
        if(strncmp((const char*)&code[c],"root",4) == 0)
            code=strreplace(code,c,4,"$r");
        else if(strncmp((const char*)&code[c],"sprod",5) == 0)
            code=strreplace(code,c,5,"$s");
        else if(strncmp((const char*)&code[c],"ans",3) == 0)
            code=strreplace(code,c,3,"$A");
        else if(strncmp((const char*)&code[c],"pi",2) == 0)
        {
            code=strreplace(code,c,2,"()") ;
            code=strinsert(code,c+1,SPI);
        }
        else if(strncmp((const char*)&code[c],"eu",2) == 0)
        {
            code=strreplace(code,c,2,"()") ;
            code=strinsert(code,c+1,SEULER);
        }
        else if(strncmp((const char*)&code[c],"exp",3) == 0)
        {
            code=strreplace(code,c,3,"()^" );
            code=strinsert(code,c+1,SEULER);
        }
        else if(strncmp((const char*)&code[c],"d/dx",4) == 0)
            code=strreplace(code,c,4,"\\d");
        else if(strncmp((const char*)&code[c],"integ",5) == 0)
            code=strreplace(code,c,5,"\\i");
        else if(strncmp((const char*)&code[c],"bin",3) == 0)
            code=strreplace(code,c,3,"\\b");
        else if(strncmp((const char*)&code[c],"oct",3) == 0)
            code=strreplace(code,c,3,"\\o");
        else if(strncmp((const char*)&code[c],"dec",3) == 0)
            code=strreplace(code,c,3,"\\c");
        else if(strncmp((const char*)&code[c],"hex",3) == 0)
            code=strreplace(code,c,3,"\\h");
        else if(strncmp((const char*)&code[c],"--",2) == 0 || strncmp((const char*)&code[c],"++",2) == 0)
        {
            code=strreplace(code,c,2,"+ ");
            c--;
        }
        else if(strncmp((const char*)&code[c],"+-",2) == 0 || strncmp((const char*)&code[c],"-+",2) == 0)
        {
            code=strreplace(code,c,2,"- ");
            c--;
        }
        else if(code[c] == 'x' && pref->calcType!=BASE)
            code[c]='X';
        else if(code[c] == 'z' && pref->calcType!=BASE)
            code[c]='Z';

        else if(code[c] == (char)0xb2 && c>0 && code[c-1] == (char)0xc2)    // second power
            code=strreplace(code,c-1,2,"^2");
        else if(code[c] == (char)0xb3 && c>0 && code[c-1] == (char)0xc2)    // second power
            code=strreplace(code,c-1,2,"^3");

        else if(!(code[c+1]>='a' && code[c+1]<='z') && c>0 && code[c-1]>='0' && code[c-1]<='9')
        {
            if(code[c] == 'f')
                code=strreplace(code,c,1,"e-15");
            else if(code[c] == 'n')
                code=strreplace(code,c,1,"e-9");
            else if(code[c] == 'p')
                code=strreplace(code,c,1,"e-12");
            else if(code[c] == (char)0xb5 && c>0 && code[c-1] == (char)0xc2)    //micro
                code=strreplace(code,c-1,2,"e-6");
            else if(code[c] == 'm')
                code=strreplace(code,c,1,"e-3");
            else if(code[c] == 'k')
                code=strreplace(code,c,1,"e3");
        }
        len=strlen(code);
    }
    
    quote=false;
    for(unsigned int c=0; c<len-1; c++)
    {
        if(len<=0)
            return nullptr;
        
        if(quote)
        {
            if(code[c]=='"')
                quote=false;
        }
        else 
        {
            if(code[c]=='"')
                quote=true;
            else if (shouldInsertMultiplication(code, c, pref, len))
            {
                code=strinsert(code,c+1,"*");
            }
        }
        len=strlen(code);

    }

    return code;
}

int bracketFind(char* string,const char* searchString, int start,int end)
{

    int searchLen=strlen(searchString);
    int bracket=0,brace=0,sqbracket=0;
    bool quote=false;
    if(end==-1)
        end=(int)strlen(string);
    for(int c=start; c<end; c++)
    {
        if(bracket == 0 && brace == 0  && sqbracket==0 && !quote)
        {
            if(strncmp((const char*)&string[c],searchString,searchLen) == 0)
                return c;
            else if(string[c] == '(')
                bracket++;
            else if(string[c] == '{')
                brace++;
            else if(string[c] == '[')
                sqbracket++;
            else if(string[c] == '"')
                quote=true;
        }
        else {
            if(!quote)
            {
                if(string[c] == '(')
                    bracket++;
                else if(string[c] == ')')
                {
                    bracket--;
                    if(bracket == 0 && brace == 0 && sqbracket==0)
                    {
                        if(strncmp((const char*)&string[c],searchString,searchLen) == 0)
                            return c;
                    }
                }
                else if(string[c] == '{')
                    brace++;
                else if(string[c] == '}')
                {
                    brace--;
                    if(bracket == 0 && brace == 0 && sqbracket==0)
                    {
                        if(strncmp((const char*)&string[c],searchString,searchLen) == 0)
                            return c;
                    }
                }
                if(string[c] == '[')
                    sqbracket++;
                else if(string[c] == ']')
                {
                    sqbracket--;
                    if(bracket == 0 && brace == 0 && sqbracket==0)
                    {
                        if(strncmp((const char*)&string[c],searchString,searchLen) == 0)
                            return c;
                    }
                }
                else if(string[c] == '"')
                    quote=!quote;
            }
            else if(string[c] == '"')
                quote=!quote;
        }
    }
    
    return -1;
}


int bracketFindRev(char* string,const char* searchString, int start, int end)
{
    if(start==-1)
        start=strlen(string)-1;

    int searchLen=strlen(searchString);
    int bracket=0,brace=0,sqbracket=0;
    bool quote=false;
    end+=searchLen-1;
    for(int c=start; c>=end; c--)
    {

        if(bracket == 0 && brace == 0 && sqbracket==0 && !quote)
        {
            if(strncmp((const char*)&string[c-searchLen+1],searchString,searchLen) == 0)
                return c;
            else if(string[c] == ')')
                bracket++;
            else if(string[c] == '}')
                brace++;
            else if(string[c] == ']')
                sqbracket++;
            else if(string[c] == '"')
                quote=true;
        }
        else {
            if(!quote)
            {
                if(string[c] == ')')
                    bracket++;
                else if(string[c] == '}')
                    brace++;
                else if(string[c] == '(')
                {
                    bracket--;
                    if(bracket == 0 && brace == 0 && sqbracket==0 && !quote)
                        if(strncmp((const char*)&string[c-searchLen+1],searchString,searchLen) == 0)
                            return c;
                }
                else if(string[c] == '[')
                {
                    sqbracket--;
                    if(bracket == 0 && brace == 0 && sqbracket==0 && !quote)
                        if(strncmp((const char*)&string[c-searchLen+1],searchString,searchLen) == 0)
                            return c;
                }
                if(string[c] == ']')
                    sqbracket++;
                else if(string[c] == '{')
                {
                    brace--;
                    if(bracket == 0 && brace == 0 && sqbracket==0 && !quote)
                        if(strncmp((const char*)&string[c-searchLen+1],searchString,searchLen) == 0)
                            return c;
                }
                else if(string[c] == '"')
                    quote=!quote;
            }
            else if(string[c] == '"')
                quote=!quote;
        }
    }
    
    return -1;
}



char*strcut(char*src,int index,int len)
{
    int srclen=strlen(src);
    char*ret=new char[srclen-len+1];
    if(index>0)
        memcpy(ret,src,index);
    memcpy(&ret[index],&src[index+len],srclen-len-index+1);
    return ret;
}

char*strins(char*dest,const char*src,int index)
{
    int destlen=strlen(dest);
    int srclen=strlen(src);
    char*ret=new char[srclen+destlen+1];
    if(index>0)
        memcpy(ret,dest,index);
    memcpy(&ret[index],src,srclen);
    memcpy(&ret[index+srclen],&dest[index],destlen-index+1);
    return ret;
}

int strcopy(char*dest,const char*src,int len)
{
    memcpy(dest,src,len);
    dest[len]=(char)0;
    return 0;
}

char* strreplace(char*st,int index,int len,const char*rep)
{
    
    int replen=strlen(rep),stlen=strlen(st);

    if(len<replen)
    {
        char*tmp=(char*)malloc(stlen-index-len+1);
        memcpy(tmp,&st[index+len],stlen-index-len+1);
        st=(char*)realloc(st,stlen+replen-len+1);
        memcpy(&st[index+replen],tmp,stlen-index-len+1);
        free(tmp);
    }
    else
    {
        memmove(&st[index+replen],&st[index+len],stlen-index-len+1);
    }
    
    memcpy(&st[index],rep,replen);

    return st;
}
char* strinsert(char*st,int index,const char*ins)
{
    
    unsigned int inslen=strlen(ins),stlen=strlen(st);
    char*tmp=(char*)malloc(stlen-index+1);
    memcpy(tmp,&st[index],stlen-index+1);
    st=(char*)realloc(st,stlen+inslen+1);
    memcpy(&st[index+inslen],tmp,stlen-index+1);
    memcpy(&st[index],ins,inslen);
    free(tmp);
    return st;
}


char* checkStringAnsi(char* str,Preferences*pref)
{
    char* calcString=new char[strlen(str)+1];
    char*tmp;
    memcpy(calcString,str,strlen(str)+1);
    int calcLen=strlen(calcString);
    bool quote=false;

    for(int c=0; c<calcLen; c++)    //Step 1: remove whitespace, correct strings
    {
        if(calcLen <= 0)
            return nullptr;
        if(calcString[c]=='"')
            quote=!quote;
        if(quote)
        {
            if(calcString[c]==char(92))
            {
                if(calcString[c+1]=='n')
                    calcString[c]='\n';
                else if(calcString[c+1]=='t')
                    calcString[c]='\t';
                else if(calcString[c+1]=='a')
                    calcString[c]='\a';
                else if(calcString[c+1]=='b')
                    calcString[c]='\b';
                else if(calcString[c+1]=='v')
                    calcString[c]='\v';
                else if(calcString[c+1]=='f')
                    calcString[c]='\f';
                else if(calcString[c+1]=='r')
                    calcString[c]='\r';
                else if(calcString[c+1]=='\\')
                    calcString[c]='\\';
                
                if((signed)strlen(calcString)>c+1)
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c+1);
                    delete[]tmp;
                }
            }
            continue;
        }
        if(calcString[c]=='/' && calcString[c+1]=='/')
        {
            int pos=-1;
            for(int c1=c+1; c1<calcLen; c1++)
                if(calcString[c1]=='\n')
                {
                    pos=c1;
                    break;
                }
            if(pos==-1)
                return nullptr;
            else {
                tmp=calcString;
                calcString=strcut(calcString,c,pos-c);
                delete[]tmp;
            }
        }
        if(calcString[c] == ' ' || calcString[c] == '\t' || calcString[c] == '\n')
        {
            tmp=calcString;
            calcString=strcut(calcString,c);
            delete[]tmp;
            c--;
        }
        if(calcString[c] == '\\')//reserved character
        {
            return nullptr;
        }
        calcLen=strlen(calcString);
    }
    quote=false;
    for(int c=0; c<calcLen; c++)    //Step 2: replace special functions
    {
        if(calcLen <= 0)
            return nullptr;
        if(calcString[c]=='"')
            quote=!quote;
        if(quote)
            continue;
        if(strncmp((const char*)&calcString[c],"root",4) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,4);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"$r",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"sprod",5) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,5);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"$s",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"d/dx",4) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,4);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"\\d",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"integ",5) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,5);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"\\i",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"bin",3) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,3);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"\\b",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"oct",3) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,3);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"\\o",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"dec",3) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,3);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"\\c",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"hex",3) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,3);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"\\h",c);
            delete[]tmp;
        }
        
        if(calcString[c] == 'x' && pref->calcType!=BASE)
            calcString[c]='X';
        
        if(calcString[c] == 'z' && pref->calcType!=BASE)
            calcString[c]='Z';

        if(calcString[c] == '-' && calcString[c+1] == '-' )
        {
            calcString[c]='+';
            tmp=calcString;
            calcString=strcut(calcString,c+1);
            c--;
            delete[]tmp;
            continue;
        }
        if(calcString[c] == (char)0xb2)    // second power
        {
            tmp=calcString;
            calcString=strcut(calcString,c,1);
            delete[]tmp;
            if(c>0)
                if(calcString[c-1] == (char)0xc2)
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c-1,1);
                    delete[]tmp;
                    c--;
                }
            tmp=calcString;
            calcString=strins(calcString,"^2",c);
            delete[]tmp;

        }
        if(calcString[c] == (char)0xb3)    // third power
        {
            tmp=calcString;
            calcString=strcut(calcString,c,1);
            delete[]tmp;
            if(c>0)
                if(calcString[c-1] == (char)0xc2)
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c-1,1);
                    delete[]tmp;
                    c--;
                }
            tmp=calcString;
            calcString=strins(calcString,"^3",c);
            delete[]tmp;
        }
        if(c>0 && !(calcString[c+1]>='a' && calcString[c+1]<='z'))
        {
            if(calcString[c-1]>='0' && calcString[c-1]<='9')
            {
                if(calcString[c] == 'f')
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c);
                    delete[]tmp;
                    tmp=calcString;
                    calcString=strins(calcString,"e-15",c);
                    delete[]tmp;
                }
                if(calcString[c+1] == 'n')
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c+1);
                    delete[]tmp;
                    tmp=calcString;
                    calcString=strins(calcString,"e-9",c+1);
                    delete[]tmp;
                }
                if(calcString[c] == 'p')
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c);
                    delete[]tmp;
                    tmp=calcString;
                    calcString=strins(calcString,"e-12",c);
                    delete[]tmp;
                }
                if(calcString[c] == (char)0xb5)    //micro
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c);
                    delete[]tmp;
                    if(c>0)
                        if(calcString[c-1] == (char)0xc2)
                    {
                        tmp=calcString;
                        calcString=strcut(calcString,c-1,1);
                        delete[]tmp;
                        c--;
                    }
                    tmp=calcString;
                    calcString=strins(calcString,"e-6",c);
                    delete[]tmp;
                }
                if(calcString[c] == 'm')
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c);
                    delete[]tmp;
                    tmp=calcString;
                    calcString=strins(calcString,"e-3",c);
                    delete[]tmp;
                }
                if(calcString[c] == 'k')
                {
                    tmp=calcString;
                    calcString=strcut(calcString,c);
                    delete[]tmp;
                    tmp=calcString;
                    calcString=strins(calcString,"e3",c);
                    delete[]tmp;
                }
            }
        }
        if((calcString[c] == '+' || calcString[c] == '-') && calcString[c+1] == '+')
        {
            tmp=calcString;
            calcString=strcut(calcString,c+1);
            delete[]tmp;
            c--;
            continue;
        }
        else if((calcString[c] == '*' || calcString[c] == '/' || calcString[c] == '^') && calcString[c+1] == '+')
        {
            tmp=calcString;
            calcString=strcut(calcString,c+1);
            delete[]tmp;
            c--;
            continue;
        }
        else if((calcString[c+1] == '+' || calcString[c+1] == '-') && calcString[c] == '+')
        {
            tmp=calcString;
            calcString=strcut(calcString,c);
            delete[]tmp;
            c--;
            continue;
        }
        calcLen=strlen(calcString);
    }
    
    quote=false;
    for(int c=1; c<calcLen; c++)        //Step 3: insert not written *-signs
    {
        if(calcString[c]=='"')
            quote=!quote;
        if(quote)
            continue;
        if(shouldInsertMultiplication(calcString, c, pref, calcLen))
        {
            tmp=calcString;
            calcString=strins(calcString,"*",c);
            delete[]tmp;
        }
        
        calcLen=strlen(calcString);
    }
    quote=false;
    for(int c=0; c<calcLen-1; c++)        //Step 4: replace constants
    {
        if(calcLen <= 0)
            return nullptr;
        if(calcString[c]=='"')
            quote=!quote;
        if(quote)
            continue;
        if(strncmp((const char*)&calcString[c],"ans",3) == 0)
        {
/*            tmp=calcString;
            calcString=strcut(calcString,c,3);
            delete[]tmp;
            char*lastRes=new char[60];
            if(pref->calcType == SCIENTIFIC)
            {
                sprintf(lastRes,"%.40Lg",vars[26][0]);
                tmp=calcString;
                calcString=strins(calcString,"()",c);
                delete[]tmp;
                tmp=calcString;
                calcString=strins(calcString,lastRes,c+1);
                delete[]tmp;
            }
            else {
                sprintf(lastRes,"\\c%lli",(long long)vars[26][0]);
                tmp=calcString;
                calcString=strins(calcString,"()",c);
                delete[]tmp;
                tmp=calcString;
                calcString=strins(calcString,lastRes,c+1);
                delete[]tmp;
            }
            
            delete[]lastRes;
            */
            tmp=calcString;
            calcString=strcut(calcString,c,3);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"$A",c);
            delete[]tmp;
        }
        if(strncmp((const char*)&calcString[c],"pi",2) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,2);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"()",c);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,SPI,c+1);
            delete[]tmp;
            c+=strlen(SPI)-2;
        }
        if(strncmp((const char*)&calcString[c],"eu",2) == 0)
        {
            tmp=calcString;
            calcString=strcut(calcString,c,2);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,"()",c);
            delete[]tmp;
            tmp=calcString;
            calcString=strins(calcString,SEULER,c+1);
            delete[]tmp;
            c+=strlen(SEULER)-2;
        }
        calcLen=strlen(calcString);
    }
    quote=false;
    for(int c=1; c<calcLen; c++)        //Step 5: insert not written *-signs
    {
        if(calcString[c]=='"')
            quote=!quote;
        if(quote)
            continue;
        if(shouldInsertMultiplication(calcString, c, pref, calcLen))
        {
            tmp=calcString;
            calcString=strins(calcString,"*",c);
            delete[]tmp;
        }
        
        calcLen=strlen(calcString);
    }
    return calcString;
}



void convertToFloat(Number*num)
{
    switch(num->type)
    {
        case NINT:
            num->fval=Complex((long double)num->ival); break;
        case NFLOAT:
            break;
        case NBOOL:
            num->fval=Complex((long double)num->bval); break;
        default:
            num->fval=Complex(NAN);
    }
    num->type=NFLOAT;
    
}

void convertToInt(Number*num)
{
    switch(num->type)
    {
        case NINT:
            break;
        case NFLOAT:
            num->ival=(long long)num->fval.real();break;
        case NBOOL:
            num->ival=(long long)num->bval; break;
        case NCHAR: {
            if(num->cval==nullptr) 
                num->ival=0;
            else
                num->ival=(long long)num->cval[0]; 
            break;
        }
        default:
            num->ival=0;
    }
    num->type=NINT;
}

void convertToBool(Number*num)
{
    switch(num->type)
    {
        case NBOOL:
            break;
        case NINT:
            num->bval=num->ival!=0; break;
            break;
        case NFLOAT:
            num->bval=num->fval.real()!=0.0; break;
        case NCHAR: {
            if(num->cval==nullptr) 
                num->bval=false;
            else
                num->bval=(long long)num->cval[0]!=0; 
            break;
        }
        default:
            num->bval=false;
    }
    num->type=NBOOL;
    
}



int Calculate::split(char* line, int start, int end)
{
    int pos;
    operation=NONE;
    number=NAN;
    var=-1;
    horzObj=nullptr;
    vertObj=nullptr;
    
    if(line==nullptr)
        return -1;

    int len=strlen(line);
    if(len <=0 || start>=end)
        return -1;


    if((pos=bracketFind(line," ",start,end)) != -1)    //none operation
    {
        horzObj=new Calculate(this,line,0,pos,pref,vars);
        vertObj=new Calculate(this,line,pos+1,end,pref,vars);
        return 0;
    }
    else if(bracketFindRev(line,"+",end-1,start) != -1 || bracketFindRev(line,"-",end-1,start) != -1)
    {
        int pos1,pos2,pos=end-1;
        while(true)
        {
            pos1=bracketFindRev(line,"+",pos,start);
            if(pos1<=start)
                break;
            if(line[pos1-1]=='e')
                pos=pos1-1;
            else break;
        }
        pos=end-1;
        while(true)
        {
            pos2=bracketFindRev(line,"-",pos,start);
            if(pos2<=start)
                break;
            if(line[pos2-1]=='e')
                pos=pos2-1;
            else break;
        }
        
        if(pos2>pos1)
        {
            if(pos2>start && ( (line[pos2-1] >='A' && line[pos2-1]<='Z')                    //binary - operator
               || (line[pos2-1]>='0' && line[pos2-1]<='9')
               || line[pos2-1]=='.' || line[pos2-1]==')'|| line[pos2-1]==']'))
            {
                pos=pos2;
                operation=MINUS;
                horzObj=new Calculate(this,line,pos+1,end,pref,vars);
                vertObj=new Calculate(this,line,start,pos,pref,vars);
                return 0;
            }
            else if(pos2==start)                                        //unary - operator
            {
                operation=MINUS;
                horzObj=new Calculate(this,line,start+1,end,pref,vars);
                return 0;
            }
        }
        else if(pos1>pos2)
        {
            if(pos1>start && ( (line[pos1-1] >='A' && line[pos1-1]<='Z')                    //binary + operator
                         || (line[pos1-1]>='0' && line[pos1-1]<='9')
                         || line[pos1-1]=='.' || line[pos1-1]==')' || line[pos1-1]==']'))
            {
                pos=pos1;
                operation=PLUS;
                horzObj=new Calculate(this,line,pos+1,end,pref,vars);
                vertObj=new Calculate(this,line,start,pos,pref,vars);
                return 0;
            }
            else if(pos1==start)                                        //unary + operator
            {
                operation=PLUS;
                horzObj=new Calculate(this,line,start+1,end,pref,vars);
                return 0;
            }
        }
    }
    if(bracketFind(line,"*",start,end) != -1 || bracketFind(line,"/",start,end) != -1)
    {
        int pos1=bracketFindRev(line,"*",end-1,start);
        int pos2=bracketFindRev(line,"/",end-1,start);
        if((pos2>pos1 && pos2 != -1) || pos1==-1)
        {
            pos=pos2;
            operation=DIVIDE;
            horzObj=new Calculate(this,line,pos+1,end,pref,vars);
            vertObj=new Calculate(this,line,start,pos,pref,vars);
            return 0;
        }
        else
        {
            pos=pos1;
            operation=MULT;
            horzObj=new Calculate(this,line,pos+1,end,pref,vars);
            vertObj=new Calculate(this,line,start,pos,pref,vars);
            return 0;
        }
    }
  else if((pos=bracketFind(line,"%",start,end)) != -1)
    {
        operation=MODULO;
        vertObj=new Calculate(this,line,start,pos,pref,vars);
        horzObj=new Calculate(this,line,pos+1,end,pref,vars);
        return 0;
        
        
    }
    else if((pos=bracketFindRev(line,"^",end-1,start)) != -1)
    {
        operation=POW;
        vertObj=new Calculate(this,line,start,pos,pref,vars);
        horzObj=new Calculate(this,line,pos+1,end,pref,vars);
        return 0;
    }
    else if((pos=bracketFind(line,"$r",end-1,start)) != -1)    //    root operation for extcalc (binary operator)
    {
        operation=ROOT;
        vertObj=new Calculate(this,line,start,pos,pref,vars);
        horzObj=new Calculate(this,line,pos+2,end,pref,vars);
        return 0;
    }
    else if(line[start]>='a' && line[start]<='z') 
    {
        if(pref->angle==DEG)
            number=180.0/(long double)PI;
        else if(pref->angle==RAD)
            number=1.0;
        else number=200.0/(long double)PI;
        if(strncmp((const char*)&line[start],"asinh",5) == 0)
        {
            operation=ASINH;
            vertObj=new Calculate(this,line,start+5,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"acosh",5) == 0)
        {
            operation=ACOSH;
            vertObj=new Calculate(this,line,start+5,end,pref,vars);
        }
            else if(strncmp((const char*)&line[start],"atanh",5) == 0)
        {
            operation=ATANH;
            vertObj=new Calculate(this,line,start+5,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"asin",4) == 0)
        {
            operation=ASIN;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"acos",4) == 0)
        {
            operation=ACOS;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"atan",4) == 0)
        {
            operation=ATAN;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"sinh",4) == 0)
        {
            operation=SINH;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"cosh",4) == 0)
        {
            operation=COSH;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"tanh",4) == 0)
        {
            operation=TANH;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"sin",3) == 0)
        {
            operation=SIN;
            vertObj=new Calculate(this,line,start+3,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"cos",3) == 0)
        {
            operation=COS;
            vertObj=new Calculate(this,line,start+3,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"tan",3) == 0)
        {
            operation=TAN;
            vertObj=new Calculate(this,line,start+3,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"log",3) == 0)
        {
            operation=LG;
            vertObj=new Calculate(this,line,start+3,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"ln",2) == 0)
        {
            operation=LN;
            vertObj=new Calculate(this,line,start+2,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"sqrt",4) == 0)
        {
            operation=SQRT;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"curt",4) == 0)
        {
            operation=CURT;
            vertObj=new Calculate(this,line,start+4,end,pref,vars);
        }
        else if(strncmp((const char*)&line[start],"abs",3) == 0)
        {
            operation=SABS;
            vertObj=new Calculate(this,line,start+3,end,pref,vars);
        }
        else{
            number=NAN;
            return -1;
        }
        return 0;
    }
    else if(line[start]>='0' && line[start]<='9' || line[start]=='.' || line[start]=='$') //number
    {
        bool notNum=false;
        int pos=start;
        if(line[pos]=='$') //variable
        {
            if(line[pos+1]=='A')
            {
                var=ANS;
                return 0;
            }
            pos++;
        }
        
        number=(long double)strtold(line+pos,nullptr);

        if(errno!=ERANGE)
        {
            while((line[pos]>='0' && line[pos]<='9') || line[pos]=='.' ||
                   (line[pos]>='a' && line[pos]<='z') ||
                  (line[pos]>='A' && line[pos]<='F' && pref->calcType==BASE) ||
                  (line[pos]=='e' && (line[pos+1]=='-' || line[pos+1]=='+' || (line[pos+1]>='0' && line[pos+1]<='9'))))
                  pos++;
            if(pos<end)
            {
                if(line[pos]=='f')
                    number*=1e-15;
                else if(line[pos]=='p')
                    number*=1e-12;
                else if(line[pos]=='n')
                    number*=1e-9;
                else if(line[pos]==(char)0xc2 && line[pos+1]==(char)0xb5)
                {
                    number*=1e-6;
                    pos++;
                }
                else if(line[pos]=='m')
                    number*=1e-3;
                else if(line[pos]=='k')
                    number*=1e3;
                else if(line[pos]==(char)0xc2 && line[pos+1]=='M')
                {
                    number*=1e6;
                    pos++;
                }
                else if(line[pos]=='G')
                    number*=1e9;
                else if(line[pos]=='T')
                    number*=1e12;
                else if(line[pos]=='i') //imaginary number
                {
                    operation=COMPLEXNUM;
                    return 0;
                }
                else if(line[pos]=='!') //factorial
                {
                    operation=FACTORIAL;
                    return 0;
                }
                else notNum=true;
            }
            if(!notNum)
                return 0;
        }
    }
    return -1;
}

double Calculate::calc()
{
    if(vertObj!=nullptr)
    {
        switch(operation)
        {
            case NONE:
                return vertObj->calc();
            case PLUS:
                return vertObj->calc()+horzObj->calc();
            case MINUS:
                return vertObj->calc()-horzObj->calc();
            case MULT:
                return vertObj->calc()*horzObj->calc();
            case DIVIDE:
            {
                double ret=horzObj->calc();
                if(ret==0.0)
                {
                    errno=EDOM;
                    return NAN;
                }
                return vertObj->calc()/ret;
            }
            case MODULO:
            {
                long long ret=(long long)horzObj->calc();
                if(ret==0)
                {
                    errno=EDOM;
                    return NAN;
                }
                return (double)((long long)vertObj->calc() % ret);
            }
            case POW:
                return powl(vertObj->calc(),horzObj->calc());
            case ROOT:
            {
                double num=horzObj->calc();
                if(num==0.0)
                {
                    errno=EDOM;
                    return NAN;
                }
                return powl(vertObj->calc(),1.0/num);
            }
            case SIN:
                return sinl(vertObj->calc()*number);
            case COS:
                return cosl(vertObj->calc()*number);
            case TAN:
                return tanl(vertObj->calc()*number);
            case ASIN:
                return asinl(vertObj->calc())/number;
            case ACOS:
                return acosl(vertObj->calc())/number;
            case ATAN:
                return atanl(vertObj->calc())/number;
            case SINH:
                return sinhl(vertObj->calc());
            case COSH:
                return coshl(vertObj->calc());
            case TANH:
                return tanhl(vertObj->calc());
            case ASINH:
                return asinhl(vertObj->calc());
            case ACOSH:
                return acoshl(vertObj->calc());
            case ATANH:
                return atanhl(vertObj->calc());
            case LG:
                return log10l(vertObj->calc());
            case LN:
                return logl(vertObj->calc());
            case SQRT:
                return sqrtl(vertObj->calc());
            case CURT:
                return cbrtl(vertObj->calc());
            case SABS:
                return fabsl(vertObj->calc());
            case FACTORIAL:
            {
                long long num=(long long)vertObj->calc();
                if(num<0)
                {
                    errno=EDOM;
                    return NAN;
                }
                long long ret=1;
                for(long long c=1; c<=num; c++)
                    ret*=c;
                return (double)ret;
            }
        }
    }
    if(var==-1)
        return number;
    else return vars[var][0].fval.real();
}

double Calculate::calcVertObj()
{
    if(vertObj==nullptr)
        return 0.0;
    else return vertObj->calc();
}

double Calculate::calcHorzObj()
{
    if(horzObj==nullptr)
        return 0.0;
    else return horzObj->calc();
}


Number Script::exec()
{
    Number ret;
    ret.type=NNONE;
    ret.ival=0;
    ret.bval=false;
    ret.cval=nullptr;

    if(eventReciver->bbreak)
    {
        ret.type=NNONE;
        return ret;
    }
    if(eventReciver->bcontinue)
    {
        ret.type=NNONE;
        return ret;
    }
    if(eventReciver->exit)
    {
        ret.type=NNONE;
        return ret;
    }
    if(eventReciver->error)
    {
        ret.type=NNONE;
        return ret;
    }
    if(eventReciver->sleepTime>0 && eventReciver->usleep)
    {
        usleep(eventReciver->sleepTime);
    }

    switch(operation)
    {
        case SSEMICOLON:
            vertObj->exec();
            return horzObj->exec();
        case SFAIL:
        {
            printError("Invalid Command",0,eventReciver);
            break;
        }
        case SVAR:
            return vars[var][0];
        case SSET:
        {
            Number value=vertObj->exec();
            if(value.type==NINT) 
            {
                if(vars[var][0].type==NFLOAT || vars[var][0].type==NCOMPLEX)
                    convertToFloat(&value);
            }
            else if(value.type==NFLOAT) 
            {
                if(vars[var][0].type==NINT)
                    convertToInt(&value);
            }
            else if(value.type==NBOOL) 
            {
                if(vars[var][0].type==NINT)
                    convertToInt(&value);
                else if(vars[var][0].type==NFLOAT || vars[var][0].type==NCOMPLEX)
                    convertToFloat(&value);
            }
            if(value.type==NNONE) 
            {
                ret.type=NNONE;
                return ret;
            }

            vars[var][0]=value;
            return value;
        }
        case SBRACKET:
            return vertObj->exec();
        case SCOMPARE:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type==NNONE || val2.type==NNONE)
                return val1;
            convertToInt(&val1);
            convertToInt(&val2);
            if(val1.ival==val2.ival)
                ret.bval=true;
            else ret.bval=false;
            ret.type=NBOOL;
            return ret;
        }
        case SGREATHER:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type==NNONE || val2.type==NNONE)
                return val1;
            if(val1.type==NCHAR || val2.type==NCHAR)
            {
                printError("Can't compare strings",0,eventReciver);
                return val1;
            }
            if(val1.type==NBOOL) convertToFloat(&val1);
            if(val2.type==NBOOL) convertToFloat(&val2);
            if(val1.fval.real()>val2.fval.real())
                ret.bval=true;
            else ret.bval=false;
            ret.type=NBOOL;
            return ret;
        }
        case SLESS:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type==NNONE || val2.type==NNONE)
                return val1;
            if(val1.type==NCHAR || val2.type==NCHAR)
            {
                printError("Can't compare strings",0,eventReciver);
                return val1;
            }
            if(val1.type==NBOOL) convertToFloat(&val1);
            if(val2.type==NBOOL) convertToFloat(&val2);
            if(val1.fval.real()<val2.fval.real())
                ret.bval=true;
            else ret.bval=false;
            ret.type=NBOOL;
            return ret;
        }
        case SGREQ:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type==NNONE || val2.type==NNONE)
                return val1;
            if(val1.type==NCHAR || val2.type==NCHAR)
            {
                printError("Can't compare strings",0,eventReciver);
                return val1;
            }
            if(val1.type==NBOOL) convertToFloat(&val1);
            if(val2.type==NBOOL) convertToFloat(&val2);
            if(val1.fval.real()>=val2.fval.real())
                ret.bval=true;
            else ret.bval=false;
            ret.type=NBOOL;
            return ret;
        }
        case SLESSEQ:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type==NNONE || val2.type==NNONE)
                return val1;
            if(val1.type==NCHAR || val2.type==NCHAR)
            {
                printError("Can't compare strings",0,eventReciver);
                return val1;
            }
            if(val1.type==NBOOL) convertToFloat(&val1);
            if(val2.type==NBOOL) convertToFloat(&val2);
            if(val1.fval.real()<=val2.fval.real())
                ret.bval=true;
            else ret.bval=false;
            ret.type=NBOOL;
            return ret;
        }
        case SUNEQUAL:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type==NNONE || val2.type==NNONE)
                return val1;
            convertToInt(&val1);
            convertToInt(&val2);
            if(val1.ival!=val2.ival)
                ret.bval=true;
            else ret.bval=false;
            ret.type=NBOOL;
            return ret;
        }
        case SIF:
        {
            if(vertObj->execHorzObj().bval)
            {
                return vertObj->execVertObj();
            }
            else
            {
                return ret;
            }
        }
        case SIFELSE:
        {
            if(vertObj->execHorzObj().bval)
            {
                return vertObj->execVertObj();
            }
            else
            {
                return horzObj->execVertObj();
            }
        }
        case SFOR:
        {
            vertObj->execHorzObj();
            while(vertObj->execVertObj().bval)
            {
                if(horzObj->exec().type==NNONE && eventReciver->bbreak)
                {
                    eventReciver->bbreak=false;
                    break;
                }
                else if(eventReciver->bcontinue)
                {
                    eventReciver->bcontinue=false;
                }
                vertObj->execVertObj();
            }
            return ret;
        }
        case SWHILE:
        {
            while(vertObj->execHorzObj().bval)
            {
                if(vertObj->execVertObj().type==NNONE && eventReciver->bbreak)
                {
                    eventReciver->bbreak=false;
                    break;
                }
                else if(eventReciver->bcontinue)
                {
                    eventReciver->bcontinue=false;
                }
            }
            return ret;
        }
        case SPRINT:
        {
            Number val=vertObj->exec();
            ScriptTextEvent *event = new ScriptTextEvent(formatOutput(val,pref,eventReciver));
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return val;
        }
        case SCLEARTEXT:
        {
            ScriptTextEvent *event = new ScriptTextEvent(QString());
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SSETCURSOR:
        {
            Number x=vertObj->exec();
            Number y=horzObj->exec();
            ScriptTextPosEvent *event = new ScriptTextPosEvent(static_cast<int>(x.fval.real()), static_cast<int>(y.fval.real()));
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SSLEEP:
        {
            Number time=vertObj->exec();
            eventReciver->sleepTime=(int)time.ival;
            return ret;
        }
        case SGETLINE:
        {
            ScriptLineRequestEvent *event = new ScriptLineRequestEvent();
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            // This needs to wait for input. In a non-blocking UI, this would involve a signal/slot mechanism.
            // For now, assume a blocking wait or refactor for non-blocking if needed.
            // The eventReciver->data would be populated by the main thread after user input.
            // For now, let's just create a dummy return.
            // This is a placeholder and needs proper event handling in a real GUI.
            ret.cval = strdup(static_cast<const char*>(eventReciver->data)); // Assuming data is const char* from a previous implementation
            ret.type = NCHAR;
            return ret;
        }
        case SGETKEY:
        {
            ScriptKeyRequestEvent *event = new ScriptKeyRequestEvent();
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            ret.cval = (char*)malloc(2);
            ret.cval[0] = static_cast<const char*>(eventReciver->data)[0];
            ret.cval[1] = '\0';
            ret.type = NCHAR;
            return ret;
        }
        case SKEYSTATE:
        {
            ScriptKeyRequestEvent *event = new ScriptKeyRequestEvent();
            event->setKey(static_cast<char>(vertObj->exec().cval[0]));
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            ret.bval = static_cast<const char*>(eventReciver->data)[0] != 0;
            ret.type = NBOOL;
            return ret;
        }
        case SBREAK:
            eventReciver->bbreak=true;
            return ret;
        case SCONTINUE:
            eventReciver->bcontinue=true;
            return ret;
        case SSTOP:
            eventReciver->exit=true;
            return ret;
        case SRUN:
            return vertObj->exec();
        case SCAST:
        {
            Number val=vertObj->exec();
            Number type=horzObj->exec();
            if(type.type!=NINT || val.type==NNONE)
                return ret;
            switch(type.ival)
            {
                case NINT:
                    convertToInt(&val);
                    break;
                case NFLOAT:
                    convertToFloat(&val);
                    break;
                case NBOOL:
                    convertToBool(&val);
                    break;
                case NCHAR:
                {
                    if(val.type==NINT)
                    {
                        if(val.ival>0 && val.ival<256)
                        {
                            val.type=NCHAR;
                            val.cval=(char*)malloc(2);
                            val.cval[0]=(char)val.ival;
                            val.cval[1]='\0';
                        }
                        else val.type=NNONE;
                    }
                    else val.type=NNONE;
                    break;
                }
                case NVECTOR:
                {
                    if(val.type == NMATRIX) {
                        val.type = NVECTOR;
                    }
                    break;
                }
                case NMATRIX:
                {
                    if(val.type == NVECTOR) {
                        val.type = NMATRIX;
                    }
                    break;
                }
                default:
                    val.type=NNONE;
            }
            return val;
        }
        case SARRAY:
        {
            Number pos=horzObj->exec();
            Number arr=vertObj->exec();
            if(pos.type!=NINT || arr.type!=NVECTOR || arr.ival<0 || arr.ival>=VARNUM)
            {
                ret.type=NNONE;
                return ret;
            }
            if(pos.ival<0 || pos.ival>=eventReciver->numlen[arr.ival])
            {
                printError("Invalid array index",0,eventReciver);
                return ret;
            }
            return eventReciver->vars[arr.ival][pos.ival];
        }
        case SMATRIX:
        {
            Number row=vertObj2->exec();
            Number col=vertObj3->exec();
            Number mat=vertObj->exec();
            if(row.type!=NINT || col.type!=NINT || mat.type!=NMATRIX || mat.ival<0 || mat.ival>=VARNUM)
            {
                ret.type=NNONE;
                return ret;
            }
            if(row.ival<0 || row.ival>=eventReciver->dimension[mat.ival][0] || col.ival<0 || col.ival>=eventReciver->dimension[mat.ival][1])
            {
                printError("Invalid matrix index",0,eventReciver);
                return ret;
            }
            int effIndex=col.ival*eventReciver->dimension[mat.ival][0]+row.ival;
            if(effIndex>=eventReciver->numlen[mat.ival])
            {
                printError("Invalid matrix index",0,eventReciver);
                return ret;
            }
            return eventReciver->vars[mat.ival][effIndex];
        }
        case SRAND:
        {
            if(vertObj==nullptr)
            {
                ret.fval=Complex((long double)rand()/RAND_MAX);
                ret.type=NFLOAT;
                return ret;
            }
            else
            {
                Number num=vertObj->exec();
                if(num.type!=NINT)
                {
                    printError("Invalid argument",0,eventReciver);
                    return ret;
                }
                ret.ival=rand()%num.ival;
                ret.type=NINT;
                return ret;
            }
        }
        case SBAND:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type!=NINT || val2.type!=NINT)
            {
                printError("Invalid argument",0,eventReciver);
                return ret;
            }
            ret.ival=val1.ival & val2.ival;
            ret.type=NINT;
            return ret;
        }
        case SBOR:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type!=NINT || val2.type!=NINT)
            {
                printError("Invalid argument",0,eventReciver);
                return ret;
            }
            ret.ival=val1.ival | val2.ival;
            ret.type=NINT;
            return ret;
        }
        case SBNOT:
        {
            Number val=vertObj->exec();
            if(val.type!=NINT)
            {
                printError("Invalid argument",0,eventReciver);
                return ret;
            }
            ret.ival=~val.ival;
            ret.type=NINT;
            return ret;
        }
        case SNOT:
        {
            Number val=vertObj->exec();
            if(val.type!=NBOOL)
            {
                printError("Invalid argument",0,eventReciver);
                return ret;
            }
            ret.bval=!val.bval;
            ret.type=NBOOL;
            return ret;
        }
        case RSHIFT:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type!=NINT || val2.type!=NINT)
            {
                printError("Invalid argument",0,eventReciver);
                return ret;
            }
            ret.ival=val1.ival >> val2.ival;
            ret.type=NINT;
            return ret;
        }
        case LSHIFT:
        {
            Number val1=vertObj->exec();
            Number val2=horzObj->exec();
            if(val1.type!=NINT || val2.type!=NINT)
            {
                printError("Invalid argument",0,eventReciver);
                return ret;
            }
            ret.ival=val1.ival << val2.ival;
            ret.type=NINT;
            return ret;
        }
        case SCRIPT_DRAW_EVENT_TYPE:
        {
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_DRAW_EVENT_TYPE);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_BEGIN_EVENT_TYPE:
        {
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_BEGIN_EVENT_TYPE);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_END_EVENT_TYPE:
        {
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_END_EVENT_TYPE);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_CLEAR_EVENT_TYPE:
        {
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_CLEAR_EVENT_TYPE);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_SHOW_EVENT_TYPE:
        {
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_SHOW_EVENT_TYPE);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_COLOR_EVENT_TYPE:
        {
            Number r = vertObj2->exec();
            Number g = vertObj3->exec();
            Number b = vertObj->exec();
            QColor *color = new QColor(static_cast<int>(r.fval.real()), static_cast<int>(g.fval.real()), static_cast<int>(b.fval.real()));
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_COLOR_EVENT_TYPE);
            event->setData(color);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_VERTEX_EVENT_TYPE:
        {
            Number x = vertObj2->exec();
            Number y = vertObj3->exec();
            Number z = vertObj->exec();
            GLdouble *coords = new GLdouble[3];
            coords[0] = static_cast<GLdouble>(x.fval.real());
            coords[1] = static_cast<GLdouble>(y.fval.real());
            coords[2] = static_cast<GLdouble>(z.fval.real());
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_VERTEX_EVENT_TYPE);
            event->setData(coords);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_TEXT_EVENT_TYPE:
        {
            Number x = vertObj2->exec();
            Number y = vertObj3->exec();
            Number z = vertObj4->exec();
            Number text = vertObj->exec();
            TextData *td = new TextData;
            td->x = static_cast<GLdouble>(x.fval.real());
            td->y = static_cast<GLdouble>(y.fval.real());
            td->z = static_cast<GLdouble>(z.fval.real());
            td->text = QString::fromUtf8(text.cval);
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_TEXT_EVENT_TYPE);
            event->setData(td);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_ROTATE_EVENT_TYPE:
        {
            Number angle = vertObj2->exec();
            Number x = vertObj3->exec();
            Number y = vertObj4->exec();
            Number z = vertObj->exec();
            RotationData *rd = new RotationData;
            rd->angle = static_cast<GLdouble>(angle.fval.real());
            rd->x = static_cast<GLdouble>(x.fval.real());
            rd->y = static_cast<GLdouble>(y.fval.real());
            rd->z = static_cast<GLdouble>(z.fval.real());
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_ROTATE_EVENT_TYPE);
            event->setData(rd);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_SCALE_EVENT_TYPE:
        {
            Number x = vertObj2->exec();
            Number y = vertObj3->exec();
            Number z = vertObj->exec();
            ScaleData *sd = new ScaleData;
            sd->x = static_cast<GLdouble>(x.fval.real());
            sd->y = static_cast<GLdouble>(y.fval.real());
            sd->z = static_cast<GLdouble>(z.fval.real());
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_SCALE_EVENT_TYPE);
            event->setData(sd);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_GRAPH_TRANSLATE_EVENT_TYPE:
        {
            Number x = vertObj2->exec();
            Number y = vertObj3->exec();
            Number z = vertObj->exec();
            TranslationData *td = new TranslationData;
            td->x = static_cast<GLdouble>(x.fval.real());
            td->y = static_cast<GLdouble>(y.fval.real());
            td->z = static_cast<GLdouble>(z.fval.real());
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_GRAPH_TRANSLATE_EVENT_TYPE);
            event->setData(td);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_IDENTITY_EVENT_TYPE:
        {
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_IDENTITY_EVENT_TYPE);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_FILE_READ_EVENT_TYPE:
        {
            Number fileName = vertObj->exec();
            ScriptFileEvent *event = new ScriptFileEvent(SCRIPT_FILE_READ_EVENT_TYPE, QString::fromUtf8(fileName.cval));
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            ret.cval = strdup(static_cast<const char*>(eventReciver->data));
            ret.type = NCHAR;
            return ret;
        }
        case SCRIPT_FILE_WRITE_EVENT_TYPE:
        {
            Number fileName = vertObj2->exec();
            Number content = vertObj->exec();
            TextData *td = new TextData;
            td->text = QString::fromUtf8(fileName.cval) + "\n" + QString::fromUtf8(content.cval);
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_FILE_WRITE_EVENT_TYPE);
            event->setData(td);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_FILE_APPEND_EVENT_TYPE:
        {
            Number fileName = vertObj2->exec();
            Number content = vertObj->exec();
            TextData *td = new TextData;
            td->text = QString::fromUtf8(fileName.cval) + "\n" + QString::fromUtf8(content.cval);
            ExtcalcEvent *event = new ExtcalcEvent(SCRIPT_FILE_APPEND_EVENT_TYPE);
            event->setData(td);
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        case SCRIPT_FILE_REMOVE_EVENT_TYPE:
        {
            Number fileName = vertObj->exec();
            ScriptFileEvent *event = new ScriptFileEvent(SCRIPT_FILE_REMOVE_EVENT_TYPE, QString::fromUtf8(fileName.cval));
            QCoreApplication::postEvent(eventReciver->eventReciver, event);
            return ret;
        }
        default:
            return Math::exec();
    }
    return re;
}


Number Script::execVertObj()
{
    if(vertObj==nullptr)
    {
        Number r;r.type=NNONE;return r;
    }
    else return vertObj->exec();
}

Number Script::execHorzObj()
{
    if(horzObj==nullptr)
    {
        Number r;r.type=NNONE;return r;
    }
    else return horzObj->exec();
}

int Script::parse(char* line,int start,int end)
{
    int pos=start;
    value.type=NNONE;
    value.ival=0;
    value.bval=false;
    value.cval=nullptr;

    if(eventReciver->bbreak)
    {
        operation=SBREAK;
        return -1;
    }
    if(eventReciver->bcontinue)
    {
        operation=SCONTINUE;
        return -1;
    }
    if(eventReciver->exit)
    {
        operation=SSTOP;
        return -1;
    }
    if(eventReciver->sleepTime>0 && eventReciver->usleep)
    {
        usleep(eventReciver->sleepTime);
    }


    if(start>=end)
    {
        return -1;
    }
    if(line[start]==' ' || line[start]=='\t')
    {
        start++;
        if(start>=end)
            return -1;
    }
    if(strncmp((const char*)&line[start],"{ ",1)==0 && bracketFind(line,"}",start,end)==end-1) // code block
    {
        operation=NONE;
        vertObj=new Script(this,line,start+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"if",2)==0 && strncmp((const char*)&line[start+2],"(",1)==0)
    {
        int pos1=start+3;
        int pos=bracketFind(line,")",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=pos+1;
        while(line[pos2]==' ' || line[pos2]=='\t' || line[pos2]=='\n' || line[pos2]=='\a' ||
              line[pos2]=='\r' || line[pos2]=='\f' || line[pos2]=='\v' || line[pos2]=='\b')
              pos2++;
        if(pos2>=end)
            return -1;
        int codeEnd=bracketFind(line,"}",pos2,end);
        if(codeEnd==-1)
            codeEnd=bracketFind(line,";",pos2,end);
        if(codeEnd==-1)
            return -1;
        operation=SIF;
        horzObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2,codeEnd+1,pref,vars,eventReciver);
        if(strncmp((const char*)&line[codeEnd+1],"else",4)==0) //else statement
        {
            pos1=codeEnd+5;
            while(line[pos1]==' ' || line[pos1]=='\t' || line[pos1]=='\n' || line[pos1]=='\a' ||
                  line[pos1]=='\r' || line[pos1]=='\f' || line[pos1]=='\v' || line[pos1]=='\b')
                  pos1++;
            if(pos1>=end)
                return -1;
            pos2=bracketFind(line,"}",pos1,end);
            if(pos2==-1)
                pos2=bracketFind(line,";",pos1,end);
            if(pos2==-1)
                return -1;
            operation=SIFELSE;
            vertObj2=new Script(this,line,pos1,pos2+1,pref,vars,eventReciver);
            return 0;
        }
        return 0;
    }
    else if(strncmp((const char*)&line[start],"for",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        int pos1=start+4;
        int pos=bracketFind(line,")",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=pos+1;
        while(line[pos2]==' ' || line[pos2]=='\t' || line[pos2]=='\n' || line[pos2]=='\a' ||
              line[pos2]=='\r' || line[pos2]=='\f' || line[pos2]=='\v' || line[pos2]=='\b')
              pos2++;
        if(pos2>=end)
            return -1;
        int codeEnd=bracketFind(line,"}",pos2,end);
        if(codeEnd==-1)
            codeEnd=bracketFind(line,";",pos2,end);
        if(codeEnd==-1)
            return -1;
        operation=SFOR;
        horzObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2,codeEnd+1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"while",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        int pos1=start+6;
        int pos=bracketFind(line,")",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=pos+1;
        while(line[pos2]==' ' || line[pos2]=='\t' || line[pos2]=='\n' || line[pos2]=='\a' ||
              line[pos2]=='\r' || line[pos2]=='\f' || line[pos2]=='\v' || line[pos2]=='\b')
              pos2++;
        if(pos2>=end)
            return -1;
        int codeEnd=bracketFind(line,"}",pos2,end);
        if(codeEnd==-1)
            codeEnd=bracketFind(line,";",pos2,end);
        if(codeEnd==-1)
            return -1;
        operation=SWHILE;
        horzObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2,codeEnd+1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"print",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        operation=SPRINT;
        vertObj=new Script(this,line,start+6,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"cleartext",9)==0 && strncmp((const char*)&line[start+9],"(",1)==0)
    {
        operation=SCLEARTEXT;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"setcursor",9)==0 && strncmp((const char*)&line[start+9],"(",1)==0)
    {
        int pos1=start+10;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SSETCURSOR;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"sleep",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        operation=SSLEEP;
        vertObj=new Script(this,line,start+6,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"getline",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        operation=SGETLINE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"getkey",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        operation=SGETKEY;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"keystate",8)==0 && strncmp((const char*)&line[start+8],"(",1)==0)
    {
        operation=SKEYSTATE;
        vertObj=new Script(this,line,start+9,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"break",5)==0)
    {
        operation=SBREAK;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"continue",8)==0)
    {
        operation=SCONTINUE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"stop",4)==0)
    {
        operation=SSTOP;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"run",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=SRUN;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"band",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        int pos1=start+5;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SBAND;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"bor",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        int pos1=start+4;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SBOR;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"bnot",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=SBNOT;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"not",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=SNOT;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"rshift",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=RSHIFT;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"lshift",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=LSHIFT;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"draw",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=SDRAW;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gbegin",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        operation=SCRIPT_GRAPH_BEGIN_EVENT_TYPE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gend",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=SCRIPT_GRAPH_END_EVENT_TYPE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gclear",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        operation=SCRIPT_GRAPH_CLEAR_EVENT_TYPE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gshow",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        operation=SCRIPT_GRAPH_SHOW_EVENT_TYPE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gcolor",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        operation=SCRIPT_GRAPH_COLOR_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gvertex",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        int pos1=start+8;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        operation=SCRIPT_GRAPH_VERTEX_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gtext",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        int pos1=start+6;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        int pos3=bracketFind(line,",",pos2+1,end);
        if(pos3==-1)
            return -1;
        operation=SCRIPT_GRAPH_TEXT_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        vertObj4=new Script(this,line,pos2+1,pos3,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos3+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"grotate",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        int pos1=start+8;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        int pos3=bracketFind(line,",",pos2+1,end);
        if(pos3==-1)
            return -1;
        operation=SCRIPT_GRAPH_ROTATE_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        vertObj4=new Script(this,line,pos2+1,pos3,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos3+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gscale",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        operation=SCRIPT_GRAPH_SCALE_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gtranslate",10)==0 && strncmp((const char*)&line[start+10],"(",1)==0)
    {
        int pos1=start+11;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        operation=SCRIPT_GRAPH_TRANSLATE_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos2+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"gidentity",9)==0 && strncmp((const char*)&line[start+9],"(",1)==0)
    {
        operation=SCRIPT_IDENTITY_EVENT_TYPE;
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fread",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        operation=SCRIPT_FILE_READ_EVENT_TYPE;
        vertObj=new Script(this,line,start+6,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fwrite",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SCRIPT_FILE_WRITE_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fappend",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        int pos1=start+8;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SCRIPT_FILE_APPEND_EVENT_TYPE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fremove",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        operation=SCRIPT_FILE_REMOVE_EVENT_TYPE;
        vertObj=new Script(this,line,start+8,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"array",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0) //array
    {
        int pos1=start+6;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SARRAY;
        vertObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"matrix",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0) //matrix
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        int pos2=bracketFind(line,",",pos+1,end);
        if(pos2==-1)
            return -1;
        operation=SMATRIX;
        vertObj=new Script(this,line,pos2+1,end-1,pref,vars,eventReciver);
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj3=new Script(this,line,pos+1,pos2,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"abs",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=SABS;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"conj",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=SCONJ;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"real",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=SREAL;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"imag",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=SIMAG;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"arg",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=SARG;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"sin",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=CSIN;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"cos",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=CCOS;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"tan",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=CTAN;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"sinh",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=CSINH;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"cosh",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=CCOSH;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"tanh",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=CTANH;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"ln",2)==0 && strncmp((const char*)&line[start+2],"(",1)==0)
    {
        operation=CLN;
        vertObj=new Script(this,line,start+3,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"log",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        operation=CLG;
        vertObj=new Script(this,line,start+4,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"sqrt",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        operation=CSQRT;
        vertObj=new Script(this,line,start+5,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"pow",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        int pos1=start+4;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=CPOW;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"root",4)==0 && strncmp((const char*)&line[start+4],"(",1)==0)
    {
        int pos1=start+5;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=CROOT;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"div",3)==0 && strncmp((const char*)&line[start+3],"(",1)==0)
    {
        int pos1=start+4;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=CDIVIDE;
        vertObj=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fread",5)==0 && strncmp((const char*)&line[start+5],"(",1)==0)
    {
        operation=SFREAD;
        vertObj=new Script(this,line,start+6,end,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fwrite",6)==0 && strncmp((const char*)&line[start+6],"(",1)==0)
    {
        int pos1=start+7;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SFWRITE;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fappend",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        int pos1=start+8;
        int pos=bracketFind(line,",",pos1,end);
        if(pos==-1)
            return -1;
        operation=SFAPPEND;
        vertObj2=new Script(this,line,pos1,pos,pref,vars,eventReciver);
        vertObj=new Script(this,line,pos+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if(strncmp((const char*)&line[start],"fremove",7)==0 && strncmp((const char*)&line[start+7],"(",1)==0)
    {
        operation=SFREMOVE;
        vertObj=new Script(this,line,start+8,end,pref,vars,eventReciver);
        return 0;
    }
    else if(line[start]>='0' && line[start]<='9' || line[start]=='.' || line[start]=='$')
    {
        bool notNum=false;
        int pos=start;
        if(line[pos]=='$')
        {
            if(line[pos+1]=='A')
            {
                var=ANS;
                return 0;
            }
            else if(line[pos+1]=='M')
            {
                var=VERTMATRIX;
                return 0;
            }
            pos++;
        }

        value.fval=(long double)strtold(line+pos,nullptr);

        if(errno!=ERANGE)
        {
            while((line[pos]>='0' && line[pos]<='9') || line[pos]=='.' ||
                   (line[pos]>='a' && line[pos]<='z') ||
                  (line[pos]>='A' && line[pos]<='F' && pref->calcType==BASE) ||
                  (line[pos]=='e' && (line[pos+1]=='-' || line[pos+1]=='+' || (line[pos+1]>='0' && line[pos+1]<='9'))))
                  pos++;
            if(pos<end)
            {
                if(line[pos]=='f')
                    value.fval*=1e-15;
                else if(line[pos]=='p')
                    value.fval*=1e-12;
                else if(line[pos]=='n')
                    value.fval*=1e-9;
                else if(line[pos]==(char)0xc2 && line[pos+1]==(char)0xb5)
                {
                    value.fval*=1e-6;
                    pos++;
                }
                else if(line[pos]=='m')
                    value.fval*=1e-3;
                else if(line[pos]=='k')
                    value.fval*=1e3;
                else if(line[pos]==(char)0xc2 && line[pos+1]=='M')
                {
                    value.fval*=1e6;
                    pos++;
                }
                else if(line[pos]=='G')
                    value.fval*=1e9;
                else if(line[pos]=='T')
                    value.fval*=1e12;
                else if(line[pos]=='i') //imaginary number
                {
                    if(value.type==NFLOAT) //already a float
                        value.fval=Complex(0.0,value.fval.real());
                    else if(value.type==NINT) //already an int
                        value.fval=Complex(0.0,static_cast<long double>(value.ival));
                    value.type=NCOMPLEX;
                    return 0;
                }
                else if(line[pos]=='!') //factorial
                {
                    operation=SFAK;
                    return 0;
                }
                else notNum=true;
            }
            if(!notNum)
            {
                value.type=NFLOAT;
                return 0;
            }
        }
    }
    else if(strncmp((const char*)&line[start],"(",1)==0 && bracketFind(line,")",start,end)==end-1)
    {
        operation=SBRACKET;
        vertObj=new Script(this,line,start+1,end-1,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,"==",start,end)) != -1)
    {
        operation=SCOMPARE;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+2,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,">",start,end)) != -1)
    {
        operation=SGREATHER;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,"<",start,end)) != -1)
    {
        operation=SLESS;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,">=",start,end)) != -1)
    {
        operation=SGREQ;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+2,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,"<=",start,end)) != -1)
    {
        operation=SLESSEQ;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+2,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,"!=",start,end)) != -1)
    {
        operation=SUNEQUAL;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+2,end,pref,vars,eventReciver);
        return 0;
    }
    else if(bracketFindRev(line,"+ ",end-1,start) != -1 || bracketFindRev(line,"-",end-1,start) != -1)
    {
        int pos1,pos2,pos=end-1;
        while(true)
        {
            pos1=bracketFindRev(line,"+ ",pos,start);
            if(pos1<=start)
                break;
            if(line[pos1-1]=='e')
                pos=pos1-1;
            else break;
        }
        pos=end-1;
        while(true)
        {
            pos2=bracketFindRev(line,"-",pos,start);
            if(pos2<=start)
                break;
            if(line[pos2-1]=='e')
                pos=pos2-1;
            else break;
        }
        
        if(pos2>pos1)
        {
            if(pos2>start && ( (line[pos2-1]>='A' && line[pos2-1]<='Z') ||
               (line[pos2-1]>='0' && line[pos2-1]<='9') ||
               line[pos2-1]=='.' || line[pos2-1]==')' || line[pos2-1]==']'))
            {
                pos=pos2;
                operation=MINUS;
                vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
                horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
                return 0;
            }
            else if(pos2==start)                                        //unary - operator
            {
                operation=MINUS;
                vertObj=new Script(this,line,start+1,end,pref,vars,eventReciver);
                return 0;
            }
        }
        else if(pos1>pos2)
        {
            if(pos1>start && ( (line[pos1-1]>='A' && line[pos1-1]<='Z') ||
                         (line[pos1-1]>='0' && line[pos1-1]<='9') ||
                         line[pos1-1]=='.' || line[pos1-1]==')' || line[pos1-1]==']'))
            {
                pos=pos1;
                operation=PLUS;
                vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
                horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
                return 0;
            }
            else if(pos1==start)                                        //unary + operator
            {
                operation=PLUS;
                vertObj=new Script(this,line,start+1,end,pref,vars,eventReciver);
                return 0;
            }
        }
    }
    if(bracketFind(line,"*",start,end) != -1 || bracketFind(line,"/",start,end) != -1)
    {
        int pos1=bracketFindRev(line,"*",end-1,start);
        int pos2=bracketFindRev(line,"/",end-1,start);
        if((pos2>pos1 && pos2 != -1) || pos1==-1)
        {
            pos=pos2;
            operation=DIVIDE;
            vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
            horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
            return 0;
        }
        else
        {
            pos=pos1;
            operation=MULT;
            vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
            horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
            return 0;
        }
    }
  else if((pos=bracketFind(line,"%",start,end)) != -1)
    {
        operation=MODULO;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
        return 0;
        
        
    }
    else if((pos=bracketFindRev(line,"^",end-1,start)) != -1)
    {
        operation=POW;
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(&line[start],"\\d",start,end)) != -1) //differential
    {
        operation=DIFF;
        vertObj=new Script(this,line,pos+3,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(&line[start],"\\i",start,end)) != -1) //integral
    {
        operation=INTEGRAL;
        vertObj=new Script(this,line,pos+3,end,pref,vars,eventReciver);
        return 0;
    }
    else if((pos=bracketFind(line,"=",start,end)) != -1)
    {
        operation=SSET;
        vertObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
        horzObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        return 0;
    }
    else if(line[start]>='A' && line[start]<='Z' || line[start]>='a' && line[start]<='z' || line[start]=='$')
    {
        if(line[start]>='A' && line[start]<='Z' || line[start]>='a' && line[start]<='z')
            pos=start;
        else pos=start+1;

        while(line[pos]>='A' && line[pos]<='Z' || line[pos]>='a' && line[pos]<='z' ||
               line[pos]>='0' && line[pos]<='9' || line[pos]=='_')
               pos++;
        if(strncmp((const char*)&line[start],"ans",3)==0) 
            var=ANS;
        else if(strncmp((const char*)&line[start],"M",1)==0)
            var=VERTMATRIX;
        else
            var=0; // Default to 0 or handle other variables based on context

        operation=SVAR;
        return 0;
    }

    return -1;
}

int Script::split(char* line,int start,int end)
{
    int pos;
    operation=NONE;
    value.type=NNONE;
    value.ival=0;
    value.bval=false;
    value.cval=nullptr;

    if(eventReciver->bbreak)
    {
        operation=SBREAK;
        return -1;
    }
    if(eventReciver->bcontinue)
    {
        operation=SCONTINUE;
        return -1;
    }
    if(eventReciver->exit)
    {
        operation=SSTOP;
        return -1;
    }
    if(eventReciver->sleepTime>0 && eventReciver->usleep)
    {
        usleep(eventReciver->sleepTime);
    }


    if(start>=end)
    {
        return -1;
    }
    if(line[start]==' ' || line[start]=='\t')
    {
        start++;
        if(start>=end)
            return -1;
    }
    if((pos=bracketFind(line,";",start,end)) != -1)    //multiple commands
    {
        operation=SSEMICOLON;
        horzObj=new Script(this,line,pos+1,end,pref,vars,eventReciver);
        vertObj=new Script(this,line,start,pos,pref,vars,eventReciver);
        return 0;
    }
    else return parse(line,start,end);
}

void printError(const char* text, int semicolonCount, ThreadSync* data)
{
    data->error = true;
    if (data->calcMode)
        return;

    int index = semicolonCount - data->countDifference;
    QString errorMessage;
    if (index >= data->semicolonLines.GetLen())
    {
        errorMessage = "End of File            : ";
    }
    else 
    {
        if ((index > 0) && data->semicolonLines[index-1] < data->semicolonLines[index]-1)
            errorMessage = "Before or in line ";
        else 
            errorMessage = "In line           ";
    
        errorMessage += QString::number(static_cast<double>((data->semicolonLines)[index])) + ": ";
    }
    
    errorMessage += QString(text);

    ScriptDebugEvent *event = new ScriptDebugEvent(index, errorMessage);
    QCoreApplication::postEvent(data->eventReciver, event);
}

double Script::calc()
{
    // Call the base class (Math) implementation
    return Math::calc();
}

double Script::calcVertObj()
{
    // Call the base class (Math) implementation
    return Math::calcVertObj();
}

double Script::calcHorzObj()
{
    // Call the base class (Math) implementation
    return Math::calcHorzObj();
}
