#ifndef E_WIDGET_H
#define E_WIDGET_H

#include <QWidget>

#include "../explorer.h"

class Explorer;
class Widget;

struct IWidgetFactory { virtual Widget* create(Explorer* explorer) = 0; };

template<typename Type> struct WidgetFactory : public IWidgetFactory {
   virtual Type* create(Explorer* explorer) {
      return new Type(explorer);
   }
};

class Widget : public QWidget 
{
    Q_OBJECT

public:
    Widget(Explorer* explorer);

    virtual bool Setup() = 0;
    virtual bool Setup(QJsonObject data) = 0;
    virtual bool Render() = 0;

    virtual QJsonObject ExtractConfig() = 0;

private slots:
    virtual void messageReceived(QString topic, QVariant data, bool local) = 0;

protected:
    Explorer* explorer;
};

#endif