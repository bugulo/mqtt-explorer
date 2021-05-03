/*!
 * @file widget.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Base class for custom widgets
 */

#ifndef E_WIDGET_H
#define E_WIDGET_H

#include <QWidget>
#include <QString>
#include <QVariant>
#include <QJsonObject>

#include "../explorer.h"

// Foward declarations
class Explorer;

//! Abstract widget class
class Widget : public QWidget 
{
    Q_OBJECT

public:
    /*!
     * @brief Constructor
     * @param explorer Explorer instance
     */
    Widget(Explorer* explorer);

    /*!
     * @brief Setup widget when added to dashboard
     * @returns true if successful, false otherwise
     */
    virtual bool Setup() = 0;
    
    /*!
     * @brief Setup widget when loaded from configuration file
     * @param data JSON object loaded from file
     * @returns true if successful, false otherwise
     */
    virtual bool Setup(QJsonObject data) = 0;

    /*!
     * @brief Render widget to dashboard
     * @returns true if successful, false otherwise
     */
    virtual bool Render() = 0;

    /*!
     * @brief Return config created by widget so it can be saved to configuration file
     * @returns Config in JSON format
     */
    virtual QJsonObject ExtractConfig() = 0;

private slots:
    /*!
     * @brief This slot is called when client receives new message
     * @param topic Topic
     * @param data Image or string
     * @param local Whether the message was sent from us
     */
    virtual void messageReceived(QString topic, QVariant data, bool local) = 0;

protected:
    //! Explorer instance
    Explorer* explorer;
};

#endif