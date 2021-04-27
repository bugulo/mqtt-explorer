#include "widget.h"

#include "../explorer.h"

Widget::Widget(Explorer* explorer) : QWidget(explorer)
{
    this->explorer = explorer;

    connect(explorer, SIGNAL(messageReceived(QString, QVariant, bool)), this, SLOT(messageReceived(QString, QVariant, bool)));
}