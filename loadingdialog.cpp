#include "loadingdialog.h"
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>

LoadingDialog::LoadingDialog(const QString &loadingMessage, QWidget *parent) : QDialog(parent) {
    //set up dialog components
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* loadingMessageLabel = new QLabel(loadingMessage);
    QProgressBar* progressBar = new QProgressBar();
    layout->addWidget(loadingMessageLabel);
    layout->addWidget(progressBar);
    
    progressBar->setRange(0,0); //indeterminate progress
}
