#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QWidget>
#include <QString>
#include <QUrl>
#include <QTextBrowser>
#include <QToolBar>
#include <QToolButton>
#include <QPixmap>
#include <QVBoxLayout>
#include <QResizeEvent>

class HelpBrowser : public QWidget
{
  Q_OBJECT
public:
  explicit HelpBrowser(QWidget *parent = nullptr);
  void setContent(QString path);

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void zoominSlot();
  void zoomoutSlot();
  void sourceSlot(const QUrl &source);

private:
  QTextBrowser *browser;
  QToolBar     *toolBar;
  QToolButton  *backButton;
  QToolButton  *forwardButton;
  QToolButton  *zoominButton;
  QToolButton  *zoomoutButton;
  QPixmap      *forwardIcon;
  QPixmap      *backIcon;
  QPixmap      *zoominIcon;
  QPixmap      *zoomoutIcon;
  QString       currentSource;
};

#endif