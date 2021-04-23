#include <QApplication>
#include <QLabel>

#include "mqtt/async_client.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QLabel hello("Hello world!");

    hello.show();
    return app.exec();
}