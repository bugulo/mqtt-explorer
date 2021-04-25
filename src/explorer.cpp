#include "explorer.h"

#include <QDebug>

Explorer::Explorer(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    connect(this->buttonConnect, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(this->buttonDisconnect, SIGNAL(clicked()), this, SLOT(disconnectClicked()));
}

void Explorer::connectClicked()
{
    this->buttonConnect->setEnabled(false);
    this->buttonDisconnect->setEnabled(true);
    qDebug() << "Connect";
}

void Explorer::disconnectClicked()
{
    this->buttonConnect->setEnabled(true);
    this->buttonDisconnect->setEnabled(false);
    qDebug() << "Disconnect";
}