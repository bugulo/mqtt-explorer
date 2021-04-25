#include <QApplication>
#include <QLabel>
#include <QDebug>

#include "mqtt/async_client.h"

#include "explorer.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Explorer explorer;
    explorer.show();

    return app.exec();
}