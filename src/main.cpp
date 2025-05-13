#include <QAbstractNativeEventFilter>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>
#include <windows.h>

#include "device_change_event_filter.h"
#include "midi_device_model.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    midi_device_model midiModel;

    device_change_event_filter device_filter(&midiModel);
    app.installNativeEventFilter(&device_filter);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("midiDeviceModel", &midiModel);

    const QUrl url(u"qrc:/qt/qml/Bitcrackle/qml/Main.qml"_qs);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    QObject::connect(&engine,
                     &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError>& warnings) {
                         for (const QQmlError& error : warnings)
                         {
                             qWarning() << error.toString();
                         }
                     });

    engine.load(url);
    auto* window = qobject_cast<QWindow*>(engine.rootObjects().first());
    device_filter.register_device_notifications(reinterpret_cast<HWND>(window->winId()));

    return app.exec();
}
