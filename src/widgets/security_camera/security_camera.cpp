/*!
 * @file security_camera.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief WidgetSecurityCamera implementation
 */

#include "security_camera.h"

#include <QString>
#include <QPixmap>
#include <QDateTime>
#include <QPushButton>
#include <QJsonObject>

#include "../../utils.h"
#include "../../explorer.h"

#include "ui_security_camera_settings.h"

WidgetSecurityCamera::WidgetSecurityCamera(Explorer* explorer) : Widget(explorer) {}

void WidgetSecurityCamera::onShowImageButtonClicked()
{
    Utils::openImage(image, explorer);
}

void WidgetSecurityCamera::onRemoveButtonClicked()
{
    explorer->removeWidget(this);
}

bool WidgetSecurityCamera::Setup(QJsonObject data)
{
    name = data.value("name").toString();
    topic = data.value("topic").toString();
    return true;
}

bool WidgetSecurityCamera::Setup()
{
    auto dialog = new QDialog(explorer);
    Ui_SecurityCameraWidgetSettings settings;
    settings.setupUi(dialog);
    Utils::centerWidget(dialog, explorer);
    auto result = dialog->exec();

    topic = settings.topic->text();
    name = settings.name->text();

    delete dialog;
    return result;
}

QJsonObject WidgetSecurityCamera::ExtractConfig()
{
    QJsonObject result;
    result.insert("widget", "SecurityCamera");
    result.insert("name", name);
    result.insert("topic", topic);
    return result;
}

void WidgetSecurityCamera::messageReceived(QString topic, QVariant data, [[maybe_unused]] bool local)
{
    if(this->topic != topic || data.userType() != QMetaType::QPixmap)
        return;

    image = qvariant_cast<QPixmap>(data);

    labelStatus->setText("Working");
    labelDate->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
    buttonShowImage->setEnabled(true);
}

bool WidgetSecurityCamera::Render()
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

    connect(buttonRemove, &QPushButton::clicked, this, &WidgetSecurityCamera::onRemoveButtonClicked);
    connect(buttonShowImage, &QPushButton::clicked, this, &WidgetSecurityCamera::onShowImageButtonClicked);
    return true;
}