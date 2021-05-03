/*!
 * @file thermostat.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief WidgetThermostat implementation
 */

#include "thermostat.h"

#include <QString>
#include <QPixmap>
#include <QDateTime>
#include <QPushButton>
#include <QJsonObject>

#include "../../utils.h"
#include "../../explorer.h"

#include "ui_thermostat_settings.h"

WidgetThermostat::WidgetThermostat(Explorer* explorer) : Widget(explorer) {}

void WidgetThermostat::onDecreaseTemperatureButtonClicked()
{
    temperature--;
    explorer->publishData(topic, QString::number(temperature));
}

void WidgetThermostat::onIncreaseTemperatureButtonClicked()
{
    temperature++;
    explorer->publishData(topic, QString::number(temperature));
}

void WidgetThermostat::onRemoveButtonClicked()
{
    explorer->removeWidget(this);
}

bool WidgetThermostat::Setup(QJsonObject data)
{
    name = data.value("name").toString();
    topic = data.value("topic").toString();
    return true;
}

bool WidgetThermostat::Setup()
{
    auto dialog = new QDialog(explorer);
    Ui_ThermostatWidgetSettings settings;
    settings.setupUi(dialog);
    Utils::centerWidget(dialog, explorer);
    auto result = dialog->exec();

    topic = settings.topic->text();
    name = settings.name->text();

    delete dialog;
    return result;
}

QJsonObject WidgetThermostat::ExtractConfig()
{
    QJsonObject result;
    result.insert("widget", "Thermostat");
    result.insert("name", name);
    result.insert("topic", topic);
    return result;
}

void WidgetThermostat::messageReceived(QString topic, QVariant data, [[maybe_unused]] bool local)
{
    if(this->topic != topic || data.userType() != QMetaType::QString)
        return;

    bool isNumber;
    auto number = qvariant_cast<QString>(data).toInt(&isNumber);

    if(!isNumber)
        return;

    temperature = number;

    labelTemperature->setText(qvariant_cast<QString>(data) + " °C");
    labelDate->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
}

bool WidgetThermostat::Render()
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

    connect(buttonRemove, &QPushButton::clicked, this, &WidgetThermostat::onRemoveButtonClicked);
    connect(buttonDecreaseTemperature, &QPushButton::clicked, this, &WidgetThermostat::onDecreaseTemperatureButtonClicked);
    connect(buttonIncreaseTemperature, &QPushButton::clicked, this, &WidgetThermostat::onIncreaseTemperatureButtonClicked);
    return true;
}