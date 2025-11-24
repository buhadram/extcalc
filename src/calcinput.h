#ifndef CALCINPUT_H
#define CALCINPUT_H

#include "global.h"

#include <QTextEdit>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QThread>
#include <QEvent>
#include <QResizeEvent>
#include <QKeyEvent>
#include <QTextOption>

#include "list.h"

class CalcInput : public QTextEdit
{
    Q_OBJECT

    int lineLength;
    int charLength;
    QString line;
    int lineCursor;
    QFont *stdFont;
    int lineCount;
    Variable *vars;
    bool scriptExec;
    List<int> resultParagraphs;
    Script *scriptObject;
    ThreadSync *threadData;
    bool ansAvailable, ansDone, autoBrace;

    Preferences pref;

public:
    void setPref(Preferences newPref);
    void setBold(bool isBold);
    explicit CalcInput(QWidget *parentWin,
                       Variable *va,
                       ThreadSync *td,
                       bool aB = false)
    : QTextEdit(parentWin),
    lineLength(0),
    charLength(0),
    lineCursor(0),
    stdFont(nullptr),
    lineCount(0),
    vars(va),
    scriptExec(false),
    scriptObject(nullptr),
    threadData(td),
    ansAvailable(false),
    ansDone(false),
    autoBrace(aB)
    {
        stdFont = new QFont(QStringLiteral("monospace"), 10);
        stdFont->setFixedPitch(true);
        setFont(*stdFont);

        // Qt3: setTextFormat(Qt::PlainText);
        // Qt5/6 equivalent:
        setAcceptRichText(false);

        QFontMetrics fontSize = fontMetrics();
        charLength = fontSize.size(0, QStringLiteral("m")).width();

        // Qt3: setWrapPolicy(QTextEdit::AtWordOrDocumentBoundary);
        setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        lineLength = width() / charLength - 1;

        line.clear();
        lineCursor = 0;
        lineCount = 0;

        // Qt3: clicked(int,int) signal is gone.
        // Use cursorPositionChanged() and compute para/pos in a slot.
        connect(this, &QTextEdit::cursorPositionChanged,
                this, &CalcInput::cursorSlot);
    }

    void calculateKey();
    void deleteKey();
    void backKey();
    void clearAll();
    void cursorKey(int key);
    void textInput(const QString &text);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    // In Qt5/6 there is no virtual createPopupMenu(const QPoint&).
    // If your implementation relies on this being called automatically,
    // you probably want to override contextMenuEvent(QContextMenuEvent*) instead.
    QMenu *createPopupMenu(const QPoint &pos);  // keep declaration for your .cpp

public slots:
    // New signature: we will compute para/pos ourselves from textCursor()
    void cursorSlot();

    void menuSlot(int item);
    //  void scriptSlot(QString *code);

signals:
    void prefChange(Preferences);
    void calcSignal();

    // Optional: if you still want the old (para,pos) semantics,
    // you can expose this signal and emit it from cursorSlot().
    void cursorMoved(int para, int pos);
};

#endif // CALCINPUT_H
