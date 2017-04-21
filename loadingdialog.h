#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>


class LoadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadingDialog(const QString& loadingMessage, QWidget *parent = 0);
};

#endif // LOADINGDIALOG_H
