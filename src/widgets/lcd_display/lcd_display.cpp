/*!
 * @file lcd_display.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief WidgetLcdDisplay implementation
 */

#include "lcd_display.h"

#include <QString>
#include <QDateTime>
#include <QPushButton>
#include <QJsonObject>

#include "../../utils.h"
#include "../../explorer.h"

#include "ui_lcd_display_settings.h"

WidgetLcdDisplay::WidgetLcdDisplay(Explorer* explorer) : Widget(explorer) {}

void WidgetLcdDisplay::onUpdateButtonClicked()
{
    explorer->publishData(topic, inputScreenText->text());
}

void WidgetLcdDisplay::onRemoveButtonClicked()
{
    explorer->removeWidget(this);
}

bool WidgetLcdDisplay::Setup(QJsonObject data)
{
    name = data.value("name").toString();
    topic = data.value("topic").toString();
    return true;
}

bool WidgetLcdDisplay::Setup()
{
    auto dialog = new QDialog(explorer);
    Ui_LcdDisplayWidgetSettings settings;
    settings.setupUi(dialog);
    Utils::centerWidget(dialog, explorer);
    auto result = dialog->exec();

    topic = settings.topic->text();
    name = settings.name->text();

    delete dialog;
    return result;
}

QJsonObject WidgetLcdDisplay::ExtractConfig()
{
    QJsonObject result;
    result.insert("widget", "LcdDisplay");
    result.insert("name", name);
    result.insert("topic", topic);
    return result;
}

void WidgetLcdDisplay::messageReceived(QString topic, QVariant data, [[maybe_unused]] bool local)
{
    if(this->topic != topic || data.userType() != QMetaType::QString)
        return;

    auto string = qvariant_cast<QString>(data);

    labelScreenText->setText(string);
    labelLastUpdated->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
}

bool WidgetLcdDisplay::Render()
{
    auto result = explorer->subscribeTopic(topic);

    if(result == 1)
    {
        explorer->setStatus("Wildcard is not supported");
        return false;
    }
    else if(result == 2)
    {
        explorer->setStatus("Invalid topic format");
        return false;
    }
    
    if(name.isEmpty())
    {
        explorer->setStatus("Widget name can not be empty");
        return false;
    }

    setupUi(this);

    labelName->setText(name);

    connect(buttonRemove, &QPushButton::clicked, this, &WidgetLcdDisplay::onRemoveButtonClicked);
    connect(buttonUpdateText, &QPushButton::clicked, this, &WidgetLcdDisplay::onUpdateButtonClicked);
    return true;
}