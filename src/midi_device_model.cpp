#include "midi_device_model.h"
#include <windows.h>

midi_device_model::midi_device_model(QObject* parent)
    : QAbstractListModel(parent)
{
    refresh_devices();
}

int midi_device_model::rowCount(const QModelIndex& /*parent*/) const
{
    return devices_.size();
}

QVariant midi_device_model::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= devices_.size())
        return {};

    if (role == device_name_role)
        return devices_.at(index.row());

    return {};
}

QHash<int, QByteArray> midi_device_model::roleNames() const
{
    return {{device_name_role, "deviceName"}};
}

QStringList midi_device_model::probe_devices()
{
    QStringList device_names;
    UINT num_devices = midiInGetNumDevs();
    for (UINT i = 0; i < num_devices; ++i)
    {
        MIDIINCAPS caps;
        if (midiInGetDevCaps(i, &caps, sizeof(MIDIINCAPS)) == MMSYSERR_NOERROR)
        {
            device_names << QString::fromWCharArray(caps.szPname);
        }
    }
    return device_names;
}

void midi_device_model::refresh_devices()
{
    QStringList new_devices = probe_devices();

    if (new_devices != devices_)
    {
        beginResetModel();
        devices_ = new_devices;
        endResetModel();

        // After devices updated
        if (!current_device_.isEmpty() && devices_.contains(current_device_))
        {
            // current device is still valid, do nothing
        }
        else
        {
            // device disappeared or not selected yet -> pick first one
            if (!devices_.isEmpty())
            {
                current_device_ = devices_.first();
            }
            else
            {
                current_device_.clear(); // no devices available
            }
        }

        emit current_device_changed();
    }
}

void midi_device_model::set_current_device(const QString& device)
{
    if (current_device_ != device)
    {
        current_device_ = device;
        emit current_device_changed();
    }
}

QString midi_device_model::current_device() const
{
    return current_device_;
}

int midi_device_model::current_index() const
{
    return devices_.indexOf(current_device_);
}
