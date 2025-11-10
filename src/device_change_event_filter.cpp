#include "device_change_event_filter.h"
#include "midi_device_model.h"
#include <dbt.h>
#include <QDebug>

device_change_event_filter::device_change_event_filter(midi_device_model* model)
    : device_model_(model)
{
}

void device_change_event_filter::register_device_notifications(HWND hwnd)
{
    DEV_BROADCAST_DEVICEINTERFACE notification_filter = {};
    notification_filter.dbcc_size       = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    notification_filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    device_notify_handle_ = RegisterDeviceNotification(
        hwnd, &notification_filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    if (!device_notify_handle_)
    {
        qWarning() << "Failed to register for device notifications!";
    }
}

bool device_change_event_filter::nativeEventFilter(const QByteArray& event_type,
                                                   void* message,
                                                   qintptr* result)
{
    if (event_type != "windows_generic_MSG")
        return false;

    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_DEVICECHANGE)
    {
        qDebug() << "Device change detected: wParam=" << msg->wParam;

        if (device_model_)
            device_model_->refresh_devices();
    }

    return false;
}
