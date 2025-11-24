#include "helpbrowser.h"
#include <QDesktopServices>
#include <QUrl>

HelpBrowser::HelpBrowser(QWidget *parent)
: QWidget(parent)
{
    setWindowTitle(tr("Help Browser"));
    currentSource = "";

    // Toolbar
    toolBar = new QToolBar(this);
    toolBar->setMovable(false);

    // Icons
    forwardIcon = new QPixmap(INSTALLDIR + QString("/data/forward.png"));
    backIcon    = new QPixmap(INSTALLDIR + QString("/data/back.png"));
    zoominIcon  = new QPixmap(INSTALLDIR + QString("/data/zoomin.png"));
    zoomoutIcon = new QPixmap(INSTALLDIR + QString("/data/zoomout.png"));

    // Browser
    browser = new QTextBrowser(this);

    // Layout: toolbar on top, browser below
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(toolBar);
    layout->addWidget(browser);

    // Buttons on toolbar
    backButton = new QToolButton(toolBar);
    backButton->setIcon(QIcon(*backIcon));
    toolBar->addWidget(backButton);

    forwardButton = new QToolButton(toolBar);
    forwardButton->setIcon(QIcon(*forwardIcon));
    toolBar->addWidget(forwardButton);

    zoominButton = new QToolButton(toolBar);
    zoominButton->setIcon(QIcon(*zoominIcon));
    toolBar->addWidget(zoominButton);

    zoomoutButton = new QToolButton(toolBar);
    zoomoutButton->setIcon(QIcon(*zoomoutIcon));
    toolBar->addWidget(zoomoutButton);

    // Initially disabled (no history yet)
    forwardButton->setEnabled(false);
    backButton->setEnabled(false);

    // New-style connects for button clicks
    connect(backButton,   &QToolButton::clicked,
            browser,      &QTextBrowser::backward);
    connect(forwardButton,&QToolButton::clicked,
            browser,      &QTextBrowser::forward);
    connect(zoominButton, &QToolButton::clicked,
            this,         &HelpBrowser::zoominSlot);
    connect(zoomoutButton,&QToolButton::clicked,
            this,         &HelpBrowser::zoomoutSlot);

    // Keep old-style connects for QTextBrowser signals (these still work in Qt6)
    connect(browser, SIGNAL(forwardAvailable(bool)),
            forwardButton, SLOT(setEnabled(bool)));
    connect(browser, SIGNAL(backwardAvailable(bool)),
            backButton, SLOT(setEnabled(bool)));
    QObject::connect(browser, &QTextBrowser::sourceChanged, this, &HelpBrowser::sourceSlot);
}



void HelpBrowser::setContent(QString path)
{
    browser->setSource(path);
    currentSource=path;
}

void HelpBrowser::zoominSlot()
{
    browser->zoomIn(2);
    browser->repaint();
}

void HelpBrowser::zoomoutSlot()
{
        browser->zoomOut(2);
        browser->repaint();

}

void HelpBrowser::sourceSlot(const QUrl &source)
{
    if(currentSource!="" && source.toString().indexOf(currentSource)!=0)
        browser->setSource(currentSource);
}


void HelpBrowser::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}