/*!
 * @file security_camera.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Generic Security Camera widget
 */

#ifndef E_WIDGET_SECURITY_CAMERA_H
#define E_WIDGET_SECURITY_CAMERA_H

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QVariant>
#include <QJsonObject>

#include "../widget.h"

#include "../../explorer.h"

#include "ui_security_camera.h"

// Forward declarations
class Explorer;

/*! 
 * @brief Widget for getting images from security camera
 * 
 * This widget listens on provided topic and accepts all image messages
 */
class WidgetSecurityCamera : public Widget, public Ui::SecurityCameraWidget
{
    Q_OBJECT

public:
    /*!
     * @brief Constructor
     * @param explorer Explorer instance
     */
    WidgetSecurityCamera(Explorer* explorer);

    /*!
     * @brief Setup widget when added to dashboard
     * @returns true if successful, false otherwise
     */
    virtual bool Setup() override;

    /*!
     * @brief Setup widget when loaded from configuration file
     * @param data JSON object loaded from file
     * @returns true if successful, false otherwise
     */
    virtual bool Setup(QJsonObject data) override;

    /*!
     * @brief Render widget to dashboard
     * @returns true if successful, false otherwise
     */
    virtual bool Render() override;

    /*!
     * @brief Return config created by widget so it can be saved to configuration file
     * @returns Config in JSON format
     */
    virtual QJsonObject ExtractConfig() override;

private slots:
    //! This slot is called when show image button is clicked
    void onShowImageButtonClicked();
    
    //! This slot is called when remove button is clicked
    void onRemoveButtonClicked();

    /*!
     * @brief This slot is called when client receives new message
     * @param topic Topic
     * @param data Image or string
     * @param local Whether the message was sent from us
     */
    virtual void messageReceived(QString topic, QVariant data, bool local) override;

private:
    //! Widget name
    QString name;

    //! Topic name
    QString topic;

    //! Last image
    QPixmap image;
};

#endif