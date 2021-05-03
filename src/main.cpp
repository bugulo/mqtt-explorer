/*!
 * @file main.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Main file of application
 */

#include <QApplication>
#include <QCommandLineParser>

#include "explorer.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("mqtt-explorer");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    QCommandLineOption historyOption(QStringList() << "h" << "history", "How many messages to keep in the history (Default: 10)", "history");
    parser.addOption(historyOption);
    parser.process(app);

    bool isInteger;
    int history = parser.value(historyOption).toInt(&isInteger);

    if(parser.isSet(historyOption) && (!isInteger || history < 1))
        qFatal("History option must contain numeric value >= 1");

    Explorer explorer(parser.isSet(historyOption) ? history : 10);
    explorer.show();

    return app.exec();
}