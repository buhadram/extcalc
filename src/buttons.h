#ifndef BUTTONSH
#define BUTTONSH

#include "global.h"


#include <QPushButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QString>
#include <QFont>
#include <QResizeEvent>
#include <stdio.h>


// Qt6-compatible implementation

class StandardButtons : public QGroupBox
{
    Q_OBJECT

    int cols, rows;
    Preferences pref;
    QFont stdFont;
    QButtonGroup* group_;

public:
    explicit StandardButtons(QWidget *parent = nullptr);

    void setPref(Preferences newPref);

    void setStandardSet();

    void setAlphaSet();

    void setShiftSet();

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void buttonSlot(int);

signals:
    void emitText(QString);
    void prefChange(Preferences);
};


class ExtButtons : public QGroupBox
{
    Q_OBJECT

    int cols, rows;
    Preferences pref;
    QButtonGroup *group_;

public:
    explicit ExtButtons(QWidget *parent = nullptr);

    void setPref(Preferences newPref);

    void setStandardSet();

    void setShiftSet();

    void setAlphaSet();

    void setHypSet();

    void setBaseStandardSet();

    void setBaseShiftSet();

protected:
    void resizeEvent(QResizeEvent *event) override;

public slots:
    void buttonSlot(int);

signals:
    void emitText(QString);
    void prefChange(Preferences);
};


#endif // BUTTONSH