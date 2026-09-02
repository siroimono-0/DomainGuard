#pragma once

#ifdef _KERNEL_MODE
#include <ntddk.h>
#else
#include <Windows.h>
#include <winioctl.h>
#endif

#define IOCTL_ADD_DOMAIN                     \
    CTL_CODE(                                \
        FILE_DEVICE_UNKNOWN,                 \
        0x800,                               \
        METHOD_BUFFERED,                     \
        FILE_WRITE_DATA                      \
    )

#define IOCTL_REMOVE_DOMAIN                     \
    CTL_CODE(                                \
        FILE_DEVICE_UNKNOWN,                 \
        0x801,                               \
        METHOD_BUFFERED,                     \
        FILE_WRITE_DATA                      \
    )

#define DOMAIN_MAX_CHARS 256

struct DOMAIN_REQUEST
{
    WCHAR domain[DOMAIN_MAX_CHARS];
};
