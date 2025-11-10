#pragma once
#include <filesystem>
#include <QDir>
#include <QQmlAbstractUrlInterceptor>
#include <QString>
#include <set>
#include <unordered_map>

class qml_url_interceptor : public QQmlAbstractUrlInterceptor
{
    // Q_OBJECTT
  public:
    using watched_modules = std::unordered_map<QString, QDir>;
    using watched_dirs    = std::set<QString>;

  private:
    watched_modules modules_;
    watched_dirs dirs_;

  public:
    QUrl intercept(const QUrl& url, DataType type) override;
    void add_replacable_module(const QString& module_path);
    [[nodiscard]] bool has_modules() const;
    [[nodiscard]] watched_dirs get_watched_dirs() const;
    void reset_watched_dirs();
};
