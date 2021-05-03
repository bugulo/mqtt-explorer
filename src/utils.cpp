/*!
 * @file utils.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Implementation of Utils
 */

#include "utils.h"

#include <QFile>
#include <QDialog>
#include <QPixmap>
#include <QString>
#include <QWidget>
#include <QByteArray>
#include <QFileDialog>

#include "ui_preview_image.h"
#include "ui_preview_string.h"

QByteArray Utils::readFile(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    auto data = file.readAll();
    file.close();
    return data;
}

bool Utils::writeFile(QString fileName, QByteArray data)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    file.write(data);
    file.close();
    return true;
}

QByteArray Utils::loadFile(QWidget *parent, QString name, QString filter)
{
    auto fileName = QFileDialog::getOpenFileName(parent, name, "", filter);
    return readFile(fileName);
}

bool Utils::saveFile(QWidget *parent, QByteArray data, QString name, QString filter)
{
    auto fileName = QFileDialog::getSaveFileName(parent, name, "", filter);
    return !fileName.isEmpty() && writeFile(fileName, data);
}

void Utils::openImage(QPixmap image, QWidget *parent)
{
    auto dialog = new QDialog(parent);
    Ui_PreviewImage preview;
    preview.setupUi(dialog);
    preview.label->setPixmap(image);
    preview.label->setScaledContents(true);
    dialog->exec();
    delete dialog;
}

void Utils::openText(QString text, QWidget *parent)
{
    auto dialog = new QDialog(parent);
    Ui_PreviewString preview;
    preview.setupUi(dialog);
    preview.textBrowser->setText(text);
    dialog->exec();
    delete dialog;
}

void Utils::centerWidget(QWidget *widget, QWidget *parent)
{
    auto geometry = parent->geometry();
    widget->move(geometry.center() - widget->rect().center());
}