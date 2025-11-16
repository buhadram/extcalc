/*/////////////////////////////////////////Extcalc////////////////////////////////////////////
/////////////////////////////////Scientific Graphic Calculator////////////////////////////////

File:         scriptedit.h
Author:       Rainer Strobel
Email:        rainer1223@users.sourceforge.net
Homepage:     http://extcalc-linux.sourceforge.net

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

The class for the script editor tab window. It does the script file management.

////////////////////////////////////////////////////////////////////////////////////////////*/
#ifndef SCRIPTEDITH
#define SCRIPTEDITH

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <qsplitter.h>
#include <qfont.h>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QDockWidget>
#include <QMainWindow>
#include <QFileDialog>
#include <qinputdialog.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qicon.h>
#include <qaction.h>
//Added by qt3to4:
#include <QContextMenuEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <list.h>
#include "buttons.h"
#include "catalog.h"
#include "tabwidget.h"


class LineNumberView :public QTextEdit
{
	
public:
	LineNumberView(QWidget*parent) :QTextEdit(parent)
	{
	}
	
protected:
	virtual void wheelEvent(QWheelEvent*);
	virtual void contentsMousePressEvent(QMouseEvent*);
	virtual void contentsMouseMoveEvent(QMouseEvent*);
	virtual void contentsMouseDoubleClickEvent(QMouseEvent*);
	virtual void keyPressEvent(QKeyEvent*);
	virtual void contentsContextMenuEvent(QContextMenuEvent*);
	virtual void contentsDragEnterEvent(QDragEnterEvent*);
	virtual void contentsDragMoveEvent(QDragMoveEvent*);
	virtual void contentsDragLeaveEvent(QDragLeaveEvent*);
	virtual void contentsDropEvent(QDropEvent*);
};

class ScriptWidget :public TabWidget
{
//	Preferences pref;
//	Variable * vars;
//	StandardButtons* standardButtons;
//	ExtButtons* extButtons;
	Catalog *catalog;
	QTextEdit*editor;
	QListWidget*fileBrowser;
	LineNumberView*lineNumbers;
//	QLabel*lineNumbers;
	QSplitter*splitter;
//	QPushButton*maximizeButton,*runButton,*saveButton;
	QFont *stdFont;
	int fontWidth,fontHeight;
//	int menuBottom;
	QMenu *fileBrowserMenu;
	QPixmap*modifiedIcon;
//	bool maximized;
	List <QString> activeFiles;
	bool currentTextChanged;
	QListWidget*activeFileItem;
	QListWidget*clickedFileItem;
	QToolBar*editorToolBar;
//	Q3DockArea*dockArea;
	QPixmap*newIcon,*saveIcon,*saveallIcon,*undoIcon,*redoIcon,*cutIcon,*copyIcon,*pasteIcon,*importIcon,*exportIcon,*runIcon,*minimizeIcon,*catalogIcon;
//	QAction*newAction,*saveAction,*saveallAction,*undoAction,*redoAction,*cutAction,*copyAction,*pasteAction,*importAction,*exportAction,*runAction,*minimizeAction,*catalogAction;
	QPushButton *newButton,*saveButton,*saveallButton,*undoButton,*redoButton,*cutButton,*copyButton,*pasteButton,*importButton,*exportButton,*runButton,*minimizeButton,*catalogButton;
	
	
	
	Q_OBJECT
public:

	ScriptWidget(QWidget*parent,Preferences p,Variable*va);


	void setPref(Preferences);
	
	void updateFileList();
	void readDir(QString,QListWidget*);
	QString getFileName(QListWidget*);
	QListWidget* getFileItem(QString);
	bool quitProgram();
	bool saveFile(QString path,QString content);
	void createNumbers();
	
public slots:
	void getPref(Preferences);
	void buttonInputSlot(QString);
	void maximizeButtonSlot();
	void editSlot(int);
	void showFile(QListWidget*);
	void createFileBrowserMenu(QListWidget*,const QPoint&,int);
	void fileBrowserMenuSlot(int item);
	void textChangedSlot();
	void saveSlot();
	void runButtonSlot();
	void lineNumSlot(int,int);
	void saveallSlot();
	void newSlot();
	void importSlot();
	void exportSlot();
	void dockWindowSlot();
	void catalogSlot();

protected:
//virtual void resizeEvent(QResizeEvent*);
	
signals:
	void prefChange(Preferences);
	void runScript(QString*);
	void controlScriptMenu(int);
};


#endif


