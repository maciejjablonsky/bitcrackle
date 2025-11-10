#include "qml_url_interceptor.h"
#include <QUrl>
#include <ranges>

QUrl qml_url_interceptor::intercept(const QUrl& url, DataType type)
{
    if (constexpr auto allowed = {DataType::QmlFile, DataType::UrlString};
        !std::ranges::contains(allowed, type))
    {
        return url;
    }

    QString qrc_path = url.path();
    if (qrc_path.startsWith('/'))
    {
        qrc_path.remove(0, 1);
    }

    for (const auto& [module, module_dir] : modules_)
    {
        const QString prefix = "qt/qml/";
        if (qrc_path.startsWith(prefix + module))
        {
            qrc_path.remove(prefix + module + '/');
            qrc_path.remove(QString("qml/") + module + '/');
            auto new_path         = module_dir.absoluteFilePath(qrc_path);
            auto enclosing_folder = QFileInfo(new_path).dir().path();
            dirs_.insert(enclosing_folder);
            auto local = QUrl::fromLocalFile(new_path);
            auto s     = local.toString().toStdString();
            return local;
        }
    }
    return url;
}

void qml_url_interceptor::add_replacable_module(const QString& module_path)
{
    auto name      = QDir(module_path).dirName();
    modules_[name] = module_path;
}

[[nodiscard]] bool qml_url_interceptor::has_modules() const
{
    return not modules_.empty();
}

[[nodiscard]] qml_url_interceptor::watched_dirs qml_url_interceptor::
    get_watched_dirs() const
{
    return dirs_;
}

void qml_url_interceptor::reset_watched_dirs()
{
    dirs_.clear();
}
