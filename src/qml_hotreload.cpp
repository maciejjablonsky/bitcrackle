#include "qml_hotreload.h"
#include <chrono>
#include <print>
#include <QQmlComponent>
#include <QQuickItem>

namespace bip
{
void watch_recursively(QFileSystemWatcher& watcher,
                       const QString& directory_path)
{
    QDir directory(directory_path);
    if (!directory.exists())
    {
        return;
    }

    watcher.addPath(directory_path);
    QFileInfoList entries = directory.entryInfoList(QDir::Files | QDir::Dirs |
                                                    QDir::NoDotAndDotDot);

    for (const QFileInfo& entry : entries)
    {
        if (entry.isDir())
        {
            watch_recursively(watcher, entry.absoluteFilePath());
        }
        else if (entry.isFile())
        {
            watcher.addPath(entry.absoluteFilePath());
        }
    }
    for (const auto& entry : watcher.files())
    {

        std::println(stderr, "watching {}", entry.toStdString());
    }
}
void qml_hotreload::load_()
{
    engine_.clearComponentCache();
    url_interceptor_.reset_watched_dirs();
    view_.setSource(root_qml_);

    for (const auto& dir : url_interceptor_.get_watched_dirs())
    {
        watcher_.addPath(dir);
    }

    if (view_.errors().isEmpty())
    {
        return;
    }

    std::string error_text;
    for (const auto& err : view_.errors())
    {

        error_text += err.toString().toStdString() + '\n';
    }
    std::string err("QML errors:\n");
    err += error_text;

    err += '\n';
    std::print(stderr, "{}", err);
}

qml_hotreload::qml_hotreload(QQmlEngine& engine,
                             QQuickView& view,
                             qml_url_interceptor& url_interceptor,
                             QObject* parent)
    : engine_{engine}, view_{view}, url_interceptor_{url_interceptor}
{
    connect(&watcher_,
            &QFileSystemWatcher::directoryChanged,
            this,
            [this](const QString& path) {
                std::println(stderr,
                             "reloading because directory changed {}",
                             path.toStdString());
                load_();
            });
}
void qml_hotreload::set_root(const QUrl& root_url)
{
    root_qml_ = root_url;
    load_();
}

} // namespace bip
