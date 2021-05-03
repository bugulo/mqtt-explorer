/*!
 * @file explorer.h
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Explorer, simulator and dashboard functionality
 */

#ifndef E_EXPLORER_H
#define E_EXPLORER_H

#include "ui_explorer.h"

#include <QMap>
#include <QList>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QDateTime>
#include <QByteArray>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QListWidgetItem>

#include "widgets/widget.h"
#include "simulator.h"
#include "client.h"

//! Stores information about topic
class TopicData
{
public:
    //! History of received messages
    QList<std::tuple<QVariant, bool, QDateTime>> messages;

    //! TreeWidget the topic is associated with
    QTreeWidgetItem *widgetItem;

    //! If the topic is subscribed
    bool isSubscribed = false;
};

//! Register TopicData as metatype
Q_DECLARE_METATYPE(TopicData);

// Forward declarations
class Client;
class Widget;
class Explorer;
class Simulator;
class FlowLayout;

//! Helper for WidgetFactory functionality
struct IWidgetFactory { 
    //! Create widget by type
    virtual Widget* create(Explorer* explorer) = 0; 
};

//! Widget factory
template<typename Type> struct WidgetFactory : public IWidgetFactory {
    //! Create widget by type
    virtual Type* create(Explorer* explorer) {
        return new Type(explorer);
    }
};

//! Main Window of the application
class Explorer : public QMainWindow, public Ui::Explorer
{
    Q_OBJECT

public:
    /*!
     * @brief Constructor
     * @param history How many messages to keep in the history
     * @param parent Parent widget
     */
    Explorer(int history, QWidget *parent = Q_NULLPTR);
    
    //! Destructor
    ~Explorer();

    /*!
     * @brief Subscribe topic
     * @param topic Topic name
     * @param root Subscription relative to which treeItem
     * @returns 0 if successful, 1 if contains wildcard, 2 if topic name is empty, 3 if topic is already subscribed
     */
    int subscribeTopic(QString topic, QTreeWidgetItem *root = Q_NULLPTR);

    /*!
     * @brief Publish text to topic
     * @param topic Topic name
     * @param data Text
     */
    void publishData(QString topic, QString data);

    /*!
     * @brief Publish ByteArray to topic
     * @param topic Topic name
     * @param data ByteArray
     */
    void publishData(QString topic, QByteArray data);

    /*!
     * @brief Set status in footer of main window
     * @param message Status message
     * @param seconds How long should be message displayed (in seconds)
     */
    void setStatus(QString message, int seconds = 3);

    /*!
     * @brief Remove widget from dashboard
     * @param widget Widget
     */
    void removeWidget(Widget *widget);

private slots:
    //! This slot is called when publish text button is clicked
    void onPublishButtonClicked();
    
    //! This slot is called when connect button is clicked
    void onConnectButtonClicked();

    //! This slot is called when save state button is clicked
    void onSaveStateButtonClicked();

    //! This slot is called when subscribe button is clicked
    void onSubscribeButtonClicked();

    //! This slot is called when disconnect button is clicked
    void onDisconnectButtonClicked();

    //! This slot is called when publish file button is clicked
    void onPublishFileButtonClicked();

    //! This slot is called when save dashboard button is clicked
    void onSaveDashboardButtonClicked();

    //! This slot is called when load dashboard button is clicked
    void onLoadDashboardButtonClicked();

    //! This slot is called when toggle simulator button is clicked
    void onToggleSimulatorButtonClicked();

    //! This slot is called when toggle subscribe button is clicked
    void onToggleSubscribeButtonClicked();

    //! This slot is called when user clicks on some topic
    void onTopicSelected();

    /*!
     * @brief This slot is called when user double clicks message in message history
     * @param item List item
     */
    void onMessageDoubleClicked(QListWidgetItem* item);

    //! This slot is called when widget add button is clicked
    void onAddWidgetButtonClicked();

    /*!
     * @brief This slot is called when client receives new message
     * @param topic Topic
     * @param data Image or string
     * @param local Whether the message was sent from us
     */
    void onReceivedMessage(QString topic, QVariant data, bool local);

signals:
    /*!
     * @brief Signal that new message was received
     * @param topic Topic
     * @param data Image or string
     * @param local Whether the message was sent from us
     */
    void messageReceived(QString topic, QVariant data, bool local);
    
private:
    /*!
     * @brief Update topic data for tree item
     * @param item tree item
     * @param data data
     */
    void setTopicData(QTreeWidgetItem* item, TopicData data);

    /*!
     * @brief Get topic data from tree item
     * @param item tree item
     * @returns topic data
     */
    TopicData getTopicData(QTreeWidgetItem* item);

    //! Reload text for toggle subscribe button
    void reloadToggleButton();

    //! Reload message history
    void reloadMessageList();

    //! Clear dashboard
    void clearDashboard();

    /*! 
     * @brief Get first selected tree item
     * @returns selected tree item
     */
    QTreeWidgetItem* getSelectedTopic();

    //! Client instance
    Client *client;

    //! Address of server the client is connected to
    QString address;

    //! Simulator instance
    Simulator *simulator;

    //! Dashboard layout
    FlowLayout *flowLayout;

    //! List of all available widgets
    QMap<QString, IWidgetFactory*> registeredWidgets;

    //! List of all created widgets
    QList<Widget*> widgets;

    //! How many messages to keep in the history
    int history = 10;
};

#endif