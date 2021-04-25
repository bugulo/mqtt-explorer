#include "explorer.h"

#include <string>
#include <random>

#include <QDebug>
#include <QUuid>

#include "mqtt/async_client.h"

Explorer::Explorer(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);

    options = new mqtt::connect_options();
    options->set_clean_session(true);

    connect(buttonConnect, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(buttonDisconnect, SIGNAL(clicked()), this, SLOT(disconnectClicked()));
    connect(buttonSubscribe, SIGNAL(clicked()), this, SLOT(subscribeClicked()));
    connect(buttonPublish, SIGNAL(clicked()), this, SLOT(publishClicked()));
    //connect(treeWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(treeItemClicked(QModelIndex)));

    treeWidget->hideColumn(2);
}

void Explorer::treeItemClicked(QModelIndex index)
{
    /*auto selected = treeWidget->selectedItems()[0];

    if(selected->isSelected())
        selected->setSelected(false);*/
}

void Explorer::subscribeClicked()
{
    if(treeWidget->selectedItems().count() == 0)
    {
        statusBar()->showMessage("Select node before subscribing!");
        return;
    }

    auto selected = treeWidget->selectedItems()[0];

    auto path = selected->text(2) + "/" + inputSubscribeTopic->text();

    if(treeWidget->findItems(path, Qt::MatchExactly|Qt::MatchCaseSensitive|Qt::MatchRecursive, 2).count() != 0)
    {
        statusBar()->showMessage("This topic is already subscibed", 3000);
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, inputSubscribeTopic->text());
    //item->setData(0, Qt::UserRole, path);
    item->setText(2, path);
    selected->addChild(item);
    selected->setExpanded(true);

    client->subscribe(path.remove(0, 1).toStdString(), 1);
}

void Explorer::publishClicked()
{
    if(treeWidget->selectedItems().count() == 0)
    {
        statusBar()->showMessage("Select node before publishing!", 3000);
        return;
    }

    auto selected = treeWidget->selectedItems()[0];
    auto path = selected->text(2);
    client->publish(path.remove(0, 1).toStdString(), inputPublishMessage->text().toStdString());
}

void Explorer::connectClicked()
{
    inputServerAddress->setEnabled(false);
    buttonConnect->setEnabled(false);

    client = new mqtt::async_client(this->inputServerAddress->text().toStdString(), QUuid::createUuid().toString().toStdString());
    client->set_message_callback(std::bind(&Explorer::message_received, this, std::placeholders::_1));

    try
    {
        statusBar()->showMessage("Connecting to server...");
        client->connect(*options)->wait();
    }
    catch(const std::exception& e)
    {
        statusBar()->showMessage("Could not connect to this server", 3000);
        buttonConnect->setEnabled(true);
        inputServerAddress->setEnabled(true);
        delete client;
        return;
    }

    statusBar()->showMessage("Successfuly connected to server!", 3000);
    buttonConnect->setEnabled(false);
    buttonDisconnect->setEnabled(true);
    tabWidget->setEnabled(true);
}

void Explorer::disconnectClicked()
{
    tabWidget->setEnabled(false);
    buttonDisconnect->setEnabled(false);
    client->disconnect()->wait();
    delete client;

    buttonConnect->setEnabled(true);
    inputServerAddress->setEnabled(true);
    statusBar()->showMessage("Successfuly disconnected from server!", 3000);
}

void Explorer::message_received(mqtt::const_message_ptr message)
{
    auto topic = QString::fromStdString(message->get_topic());
    auto data = QString::fromStdString(message->to_string());

    auto items = treeWidget->findItems("/" + topic, Qt::MatchExactly|Qt::MatchCaseSensitive|Qt::MatchRecursive, 2);

    if(items.count() == 0)
        return;

    items[0]->setText(1, data);
}

std::string Explorer::generate_id()
{
    static const std::string allowed_chars {"123456789BCDFGHJKLMNPQRSTVWXZbcdfghjklmnpqrstvwxz"};

    static thread_local std::default_random_engine randomEngine(std::random_device{}());
    static thread_local std::uniform_int_distribution<int> randomDistribution(0, allowed_chars.size() - 1);

    std::string id(32, '\0');

    for (std::string::value_type& c : id)
        c = allowed_chars[randomDistribution(randomEngine)];

    return id;
}