#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <windows.h>

class midi_device_model;

class device_change_event_filter : public QAbstractNativeEventFilter
{
public:
    explicit device_change_event_filter(midi_device_model *model);

    bool nativeEventFilter(const QByteArray &event_type, void *message, qintptr *result) override;

    void register_device_notifications(HWND hwnd);

private:
    HDEVNOTIFY device_notify_handle_ = nullptr;
    midi_device_model *device_model_ = nullptr;
};
