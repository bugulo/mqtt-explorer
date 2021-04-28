/*!
 * @file utils.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Implementation of Utils functions
 */

#include "utils.h"

#include <QFile>
#include <QString>
#include <QWidget>
#include <QByteArray>
#include <QFileDialog>

QByteArray Utils::readFile(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    auto data = file.readAll();
    file.close();
    return data;
}

QByteArray Utils::loadFile(QWidget *parent, QString name, QString filter)
{
    auto fileName = QFileDialog::getOpenFileName(parent, name, "", filter);
    return readFile(fileName);
}

void Utils::centerWidget(QWidget *widget, QWidget *parent)
{
    auto geometry = parent->geometry();
    widget->move(geometry.center() - widget->rect().center());
}