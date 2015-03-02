#ifndef ABOUTAPPDIALOG_H
#define ABOUTAPPDIALOG_H

#include <QDialog>

namespace yasem {

class AboutAppDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutAppDialog(QWidget *parent = 0);
    ~AboutAppDialog();

signals:

public slots:

private:
    void setupGui();
};

}

#endif // ABOUTAPPDIALOG_H
