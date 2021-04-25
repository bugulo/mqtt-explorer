#ifndef EXPLORER_H
#define EXPLORER_H

#include "ui_explorer.h"

#include <string>

#include <QWidget>
#include <QDebug>

#include "mqtt/async_client.h"

class Explorer : public QMainWindow, public Ui::Explorer
{
    Q_OBJECT

public:
    Explorer(QWidget *parent = Q_NULLPTR);

private slots:
    void connectClicked();
    void disconnectClicked();
    void subscribeClicked();
    void publishClicked();
    void treeItemClicked(QModelIndex index);
    
private:
    Ui::Explorer *ui;

    mqtt::async_client *client;
    mqtt::connect_options *options;

    std::string generate_id();

    void message_received(mqtt::const_message_ptr message);
};

#endif