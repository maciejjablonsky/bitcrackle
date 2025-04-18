#include <cassert>
#include <memory>
#include <print>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <dbt.h>
#include <fmt/format.h>
#include <fmt/core.h>
#include <iostream>
#include <tuple>
#include <fmt/xchar.h>

constexpr auto WND_CLASS_NAME = L"MidiListener";
constexpr auto app_name       = L"bitcrackle";
template <> struct fmt::formatter<MIDIINCAPS, wchar_t>
{
    constexpr auto parse(fmt::wformat_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const MIDIINCAPS& caps, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(),
                              L"Name: {}, Manufacturer ID: {}, Product ID: "
                              L"{}, Driver Version: {}.{}",
                              caps.szPname,
                              caps.wMid,
                              caps.wPid,
                              (caps.vDriverVersion >> 8),
                              (caps.vDriverVersion & 0xFF));
    }
};

void display_available_midi_in_devices()
{
    auto n = midiInGetNumDevs();
    std::println("There are {} available devices.", n);
    for (UINT i = 0; i < n; ++i)
    {
        MIDIINCAPS capabilities;
        auto result = midiInGetDevCaps(
            i, std::addressof(capabilities), sizeof(capabilities));
        
        fmt::println(L"-[{}] {}", i, capabilities);
        assert(result == MMSYSERR_NOERROR);
    }
}

LRESULT CALLBACK win_proc_callback(HWND hwnd,
                                   UINT msg,
                                   WPARAM wparam,
                                   LPARAM lparam)
{
    if (msg == WM_DEVICECHANGE)
    {
        display_available_midi_in_devices();
        if (wparam == DBT_DEVICEARRIVAL || wparam == DBT_DEVICEREMOVECOMPLETE)
        {
            auto dev = reinterpret_cast<DEV_BROADCAST_HDR*>(lparam);
            if (dev && dev->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                OutputDebugStringA("MIDI device change detected\n");
            }
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void error_handler(LPCTSTR lpszFunction)
{

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  dw,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0,
                  NULL);

    // Display the error message and exit the process.

    lpDisplayBuf = (LPVOID)LocalAlloc(
        LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) *
            sizeof(TCHAR));
    if (!lpDisplayBuf)
        return;
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction,
                    dw,
                    (LPCTSTR)lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, app_name, MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}

HWND init_window()
{
    WNDCLASSW wc     = {};
    wc.lpfnWndProc   = win_proc_callback;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = WND_CLASS_NAME;

    if (!RegisterClass(std::addressof(wc)))
    {
        error_handler(L"RegisterClass");
        return nullptr;
    }
    auto hwnd = CreateWindowEx(0,
                               wc.lpszClassName,
                               nullptr,
                               WS_OVERLAPPEDWINDOW,
                               0,
                               0,
                               0,
                               0,
                               nullptr,
                               nullptr,
                               GetModuleHandle(nullptr),
                               nullptr);
    if (hwnd == nullptr)
    {
        error_handler(L"CreateWindow");
        return nullptr;
    }

    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter = {};
    NotificationFilter.dbcc_size       = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    if (!RegisterDeviceNotification(
            hwnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE))
    {
        error_handler(L"RegisterDeviceNotification");
        return nullptr;
    }

    return hwnd;
}

void CALLBACK midi_in_callback(HMIDIIN midi_in_handle,
                               UINT message,
                               DWORD_PTR user_instance,
                               DWORD_PTR param1,
                               DWORD_PTR param2)
{
    if (message == MIM_DATA)
    {
        DWORD midi_message = static_cast<DWORD>(param1);

        BYTE status      = midi_message & 0xFF;
        BYTE note_number = (midi_message >> 8) & 0xFF;
        BYTE velocity    = (midi_message >> 16) & 0xFF;

        BYTE message_code = status & 0xF0;
        BYTE channel      = status & 0x0F;

        if (message_code == 0x90 && velocity != 0) // Note On
        {
            fmt::print("Note On: note={}, velocity={}, channel={}\n",
                       note_number,
                       velocity,
                       channel);
        }
        else if ((message_code == 0x90 && velocity == 0) ||
                 message_code == 0x80) // Note Off
        {
            fmt::print("Note Off: note={}, velocity={}, channel={}\n",
                       note_number,
                       velocity,
                       channel);
        }
        else
        {
            fmt::print("Other MIDI message received: status=0x{:02X}, "
                       "data1=0x{:02X}, data2=0x{:02X}\n",
                       status,
                       note_number,
                       velocity);
        }
    }
}

int main()
{
    auto hwnd = init_window();
    if (hwnd == nullptr)
    {
        return EXIT_FAILURE;
    }

    display_available_midi_in_devices();

    HMIDIIN in_device = nullptr;
    UINT device_id = 1;
    auto r            = midiInOpen(std::addressof(in_device),
                        device_id,
                        reinterpret_cast<DWORD_PTR>(midi_in_callback),
                        0,
                        CALLBACK_FUNCTION);

    if (r == MMSYSERR_NOERROR)
    {
        fmt::println("Opened sucessfully");
        midiInStart(in_device);
        fmt::println("Listening to MIDI input... Press Enter to quit.");
        getchar();
        midiInStop(in_device);
        midiInClose(in_device);
    }
    else
    {
        fmt::println("failed to open midi device");
    }
    //MSG msg;
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    return EXIT_SUCCESS;
}
