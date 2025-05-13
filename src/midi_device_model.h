#pragma once

#include <QAbstractListModel>
#include <QStringList>

class midi_device_model : public QAbstractListModel
{
    Q_OBJECT
  public:
    enum roles
    {
        device_name_role = Qt::UserRole + 1
    };

    Q_PROPERTY(QString current_device READ current_device WRITE
                   set_current_device NOTIFY current_device_changed)
    Q_PROPERTY(
        int current_index READ current_index NOTIFY current_device_changed)

    explicit midi_device_model(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    void set_current_device(const QString& device);
    QString current_device() const;
    int current_index() const;

  public slots:
    void refresh_devices();

  signals:
    void current_device_changed();

  private:
    QStringList probe_devices();
    QStringList devices_;
    QString current_device_;
};
