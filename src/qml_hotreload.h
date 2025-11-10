#pragma once

#include <QFileSystemWatcher>
#include <qml_url_interceptor.h>
#include <QQmlApplicationEngine>
#include <QQuickView>

namespace bip
{
class qml_hotreload : public QObject
{
    Q_OBJECT
  private:
    QQmlEngine& engine_;
    QQuickView& view_;
    qml_url_interceptor& url_interceptor_;
    QUrl root_qml_;
    QFileSystemWatcher watcher_;

    void load_();

  public:
    qml_hotreload(QQmlEngine& engine,
                  QQuickView& view,
                  qml_url_interceptor& url_interceptor,
                  QObject* parent = nullptr);
    qml_hotreload(const qml_hotreload&)            = delete;
    qml_hotreload& operator=(const qml_hotreload&) = delete;

    void set_root(const QUrl& root_url);
};
} // namespace bip
