#include <QApplication>

#include "explorer.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Explorer explorer;
    explorer.show();

    return app.exec();
}