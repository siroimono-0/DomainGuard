#pragma once

#ifdef _KERNEL_MODE
#include <ntddk.h>
#else
#include <Windows.h>
#include <winioctl.h>
#include <fwpmu.h>
#pragma comment(lib, "Fwpuclnt.lib")
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

#define IOCTL_KDPRINT_DOMAIN                     \
    CTL_CODE(                                \
        FILE_DEVICE_UNKNOWN,                 \
        0x802,                               \
        METHOD_BUFFERED,                     \
        FILE_WRITE_DATA                      \
    )


#define DOMAIN_MAX_CHARS 256

constexpr ULONG Q_Domain_Entry_Tag = 'xxxx';

struct DOMAIN_REQUEST
{
    LIST_ENTRY link;
    WCHAR domain[DOMAIN_MAX_CHARS];
    int len;
    bool block;
};

typedef struct Q_Domain {
    LIST_ENTRY head;
    KSPIN_LOCK lock;
}Q_Domain, *PQ_Domain;

typedef struct Q_Domain_Entry {
    LIST_ENTRY link;
    DOMAIN_REQUEST domain_req;
}Q_Domain_Entry, *PQ_Domain_Entry;

// {B00AA7A5-264D-4093-936D-0334A7CC90A1}
static const GUID GUID_WFP =
{ 0xb00aa7a5, 0x264d, 0x4093, 
	{ 0x93, 0x6d, 0x3, 0x34, 0xa7, 0xcc, 0x90, 0xa1 } };

// {2A47013E-585F-42A7-BA91-C1C4A2A13E42}
static const GUID GUID_Callout_V4 = 
{ 0x2a47013e, 0x585f, 0x42a7,
    { 0xba, 0x91, 0xc1, 0xc4, 0xa2, 0xa1, 0x3e, 0x42 } };

// {4B7FDF71-89A2-4CC9-9995-E531FB03C042}
static const GUID GUID_Callout_V6 = 
{ 0x4b7fdf71, 0x89a2, 0x4cc9, { 0x99, 0x95, 0xe5, 0x31, 0xfb, 0x3, 0xc0, 0x42 } };



















