#include <QAbstractNativeEventFilter>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QWindow>
#include <windows.h>

#include "device_change_event_filter.h"
#include "midi_device_model.h"
#include "qml_hotreload.h"
#include "qml_url_interceptor.h"
#include <print>
#include <QDirIterator>
#include <QTimer>

void listQtResourceFiles(const QString& path = ":/")
{
    QStringList files;
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        std::print(stderr, "{}\n", it.next().toStdString());
    }
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/Bitcrackle/qml/Bitcrackle/robot.png"));

    midi_device_model midiModel;
    listQtResourceFiles();
    device_change_event_filter device_filter(&midiModel);
    app.installNativeEventFilter(&device_filter);

    qml_url_interceptor interceptor;
    interceptor.add_replacable_module(BITCRACKLE_COMPONENTS_DIR);

    QQmlEngine engine;
    engine.addUrlInterceptor(std::addressof(interceptor));

    QQuickView view(&engine, nullptr);
    assert(view.engine() == &engine);

    engine.rootContext()->setContextProperty("midiDeviceModel", &midiModel);

    const QUrl url(u"qrc:/qt/qml/Bitcrackle/qml/Bitcrackle/Main.qml"_qs);

    QObject::connect(&engine,
                     &QQmlApplicationEngine::warnings,
                     [](const QList<QQmlError>& warnings) {
                         for (const QQmlError& error : warnings)
                         {
                             qWarning() << error.toString();
                         }
                     });
    view.setSource(url);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    bip::qml_hotreload hotreload(engine, view, interceptor);
    hotreload.set_root(url);
    device_filter.register_device_notifications(
        reinterpret_cast<HWND>(view.winId()));

    view.show();
    return app.exec();
}
