#include "light_switch.h"

#include <QDebug>
#include <QJsonObject>

#include "../../explorer.h"

#include "../../extensions/QFlowLayout.h"

#include "ui_light_switch_settings.h"

WidgetLightSwitch::WidgetLightSwitch(Explorer* explorer) : Widget(explorer)
{
}

void WidgetLightSwitch::onToggleButtonClicked()
{
    if(state)
        explorer->publishData(topic, "off");
    else
        explorer->publishData(topic, "on");
}

void WidgetLightSwitch::messageReceived(QString topic, QVariant data, bool local)
{
    if(this->topic != topic || data.userType() != QMetaType::QString)
        return;

    auto string = qvariant_cast<QString>(data);

    if(string != "on" && string != "off")
        return;

    labelStatus->setText(string);
    labelDate->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));

    state = string == "on" ? true : false;
}

QJsonObject WidgetLightSwitch::ExtractConfig()
{
    QJsonObject result;
    result.insert("widget", "LightSwitch");
    result.insert("topic", topic);
    return result;
}

bool WidgetLightSwitch::Setup()
{
    auto dialog = new QDialog();
    Ui_LightSwitchWidgetSettings settings;
    settings.setupUi(dialog);

    auto hostRect = explorer->geometry();
    dialog->move(hostRect.center() - dialog->rect().center());

    auto result = dialog->exec();

    topic = settings.topic->text();

    return result;
}

bool WidgetLightSwitch::Setup(QJsonObject data)
{
    topic = data.value("topic").toString();
    return true;
}

bool WidgetLightSwitch::Render()
{
    auto result = explorer->subscribeTopic(topic);

    if(result != 0 && result != 2)
    {
        explorer->setStatus("Invalid topic format");
        return false;
    }

    setupUi(this);

    //connect(explorer, SIGNAL(messageReceived(QString, QVariant, bool)), this, SLOT(messageReceived(QString, QVariant, bool)));
    connect(buttonToggleSwitch, SIGNAL(clicked()), this, SLOT(onToggleButtonClicked()));

    return true;
}