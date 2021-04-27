#ifndef E_WIDGET_LIGHT_SWITCH_H
#define E_WIDGET_LIGHT_SWITCH_H

#include "ui_light_switch.h"
#include "ui_light_switch_settings.h"

#include "../../explorer.h"
#include "../widget.h"

#include <QWidget>

class WidgetLightSwitch : public Widget, public Ui::LightSwitchWidget
{
    Q_OBJECT

public:
    WidgetLightSwitch(Explorer* explorer);

    virtual bool Setup() override;
    virtual bool Setup(QJsonObject data) override;
    virtual bool Render() override;

    virtual QJsonObject ExtractConfig() override;

private slots:   
    void onToggleButtonClicked();

    virtual void messageReceived(QString topic, QVariant data, bool local) override;

private:
    QString topic;

    bool state = false;

    void setState(bool);
};

#endif