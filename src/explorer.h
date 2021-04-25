#ifndef EXPLORER_H
#define EXPLORER_H

#include "ui_explorer.h"

#include <QWidget>
#include <QDebug>

class Explorer : public QMainWindow, public Ui::Explorer
{
    Q_OBJECT

public:
    Explorer(QWidget *parent = Q_NULLPTR);

private slots:
    void connectClicked();
    void disconnectClicked();
    
private:
    Ui::Explorer *ui;
};

#endif