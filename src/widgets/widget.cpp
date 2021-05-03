/*!
 * @file widget.cpp
 * @author Michal Šlesár (xslesa01)
 * @author Erik Belko (xbelko02)
 * @brief Widget implementation
 */

#include "widget.h"

#include "../explorer.h"

Widget::Widget(Explorer* explorer) : QWidget(explorer)
{
    this->explorer = explorer;

    connect(explorer, &Explorer::messageReceived, this, &Widget::messageReceived);
}