#include <QtGui>
#include <QtQuick>
#include <QObject>
#include <QApplication>

#include "control_top.h"
#include "qml_utils.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickView view;

    CControlTop *ctrl_top  = new CControlTop (&view);
    CQmlUtils *qml_utils = new CQmlUtils (&view);

    view.engine()->rootContext()->setContextProperty(QLatin1String("ctrlTop"), ctrl_top);
    view.engine()->rootContext()->setContextProperty(QLatin1String("qmlUtils"), qml_utils);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    QObject::connect ((QObject*)view.engine(), SIGNAL(quit()), &app, SLOT(quit()));

    view.show();

    return app.exec();
}
