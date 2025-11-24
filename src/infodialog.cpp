#include "infodialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

InfoDialog::InfoDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Information"));

    auto *layout = new QVBoxLayout(this);
    auto *label  = new QLabel(tr("Information"), this);
    auto *button = new QPushButton(tr("OK"), this);

    layout->addWidget(label);
    layout->addWidget(button);

    connect(button, &QPushButton::clicked,
            this, &QDialog::accept);
}

void InfoDialog::showInfo(const QString &text, QWidget *parent)
{
    InfoDialog dlg(parent);

    // Replace the generic label text with the actual info:
    if (auto *label = dlg.findChild<QLabel*>())
        label->setText(text);

    dlg.exec();
}

