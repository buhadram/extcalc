#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QString>
#include <QGridLayout>
#include <QTabWidget>
#include <QTextEdit>
#include <QLabel>

class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InfoDialog(QWidget *parent = nullptr);

    static void showInfo(const QString &text,
                         QWidget *parent = nullptr);

private:
    QGridLayout *layout;
    QTabWidget *tabWidget;
    QTextEdit *licenseWidget;
    QLabel *authorInfo;
    QLabel *versionInfo;
};

#endif // INFODIALOG_H
