#ifndef E_EXPLORER_H
#define E_EXPLORER_H

#include "ui_explorer.h"

#include <QWidget>
#include <QDate>

#include "mqtt/async_client.h"

#include "widgets/widget.h"
#include "simulator.h"

class TopicData
{
public:
    QList<std::tuple<QVariant, bool, QDateTime>> messages;
    QTreeWidgetItem *widgetItem;
    bool isSubscribed = false;
};

Q_DECLARE_METATYPE(TopicData);

class Widget;
class Simulator;
class FlowLayout;
struct IWidgetFactory;

class Explorer : public QMainWindow, public Ui::Explorer
{
    Q_OBJECT

public:
    Explorer(QWidget *parent = Q_NULLPTR);

    int subscribeTopic(QString topic, QTreeWidgetItem *root = NULL);
    void publishData(QString topic, QString data);

    void setStatus(QString message, int seconds = 3);

private slots:
    void onToggleButtonClicked();
    void onPublishButtonClicked();
    void onConnectButtonClicked();
    void onSaveStateButtonClicked();
    void onSubscribeButtonClicked();
    void onDisconnectButtonClicked();
    void onPublishImageButtonClicked();
    void onSaveDashboardButtonClicked();
    void onLoadDashboardButtonClicked();
    void onToggleSimulatorButtonClicked();

    void onTopicSelected();

    void onMessageDoubleClicked(QListWidgetItem* item);

    void onAddWidgetButtonClicked();

signals:
    void messageReceived(QString topic, QVariant data, bool local);
    void disconnected();
    
private:
    Ui::Explorer *ui;

    mqtt::async_client *client;
    mqtt::connect_options options;

    void onDisconnected(mqtt::properties properties, mqtt::ReasonCode reasonCode);
    void onConnectionLost(mqtt::string reason);
    void onMessageReceived(mqtt::const_message_ptr message);

    void setTopicData(QTreeWidgetItem* item, TopicData data);
    TopicData getTopicData(QTreeWidgetItem* item);

    void reloadToggleButton();
    void reloadMessageList();

    QTreeWidgetItem* getSelectedTopic();

    void processTopicMessage(QString topic, QVariant data, bool local = false);

    Simulator *simulator;

    FlowLayout *flowLayout;

    QList<Widget*> widgets;

    QMap<QString, IWidgetFactory*> registeredWidgets;
};

#endif