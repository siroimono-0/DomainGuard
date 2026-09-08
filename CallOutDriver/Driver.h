#pragma once

// 반드시 ndis.h보다 먼저 정의
#ifndef NDIS_SUPPORT_NDIS6
#define NDIS_SUPPORT_NDIS6 1
#endif

#ifndef NDIS630
#define NDIS630 1
#endif

#include <ntddk.h>
#include <ndis.h>
#include <fwpsk.h>
#include <fwpmk.h>
#include "../SharedIoctl/SharedIoctl.h"

// ================================================
extern Q_Domain g_Q_Domain;
extern UINT32 g_calloutId_V4;
extern UINT32 g_calloutId_V6;
//extern UINT32 g_SniCalloutIdV4;
//extern UINT32 g_SniCalloutIdV6;
// ================================================

void KDPRINT_DOMAIN();
// C++ 컴파일 시 DriverEntry 이름이 변형되지 않게 함
extern "C"
NTSTATUS DriverEntry(
	PDRIVER_OBJECT driverObject,
	PUNICODE_STRING registryPath
);

// 드라이버가 메모리에서 내려갈 때 Windows가 호출하는 함수입니다.
// 여기에서 DriverEntry()가 만들었던 자원을 정리합니다.
void DriverUnload(
	PDRIVER_OBJECT driverObject
);

// MajorFunction은 함수 포인터 배열입니다.
// 사용자 모드 프로그램이 드라이버 디바이스를 CreateFile()로 열 때 호출됩니다.
NTSTATUS DispatchCreate(
	PDEVICE_OBJECT deviceObject,
	PIRP irp
);

// MFC 프로그램에서 DeviceIoControl()을 호출했을 때 실행되는 함수입니다.
NTSTATUS DispatchDeviceControl(
	PDEVICE_OBJECT deviceObject,
	PIRP irp
);

// CloseHandle()로 드라이버 핸들을 닫을 때 호출
NTSTATUS DispatchClose(
	PDEVICE_OBJECT deviceObject,
	PIRP irp
);

static NTSTATUS CompleteIrp(
	PIRP irp,
	NTSTATUS status,
	ULONG_PTR information
);

NTSTATUS DispatchUnsupported(
	PDEVICE_OBJECT deviceObject, PIRP irp
);

NTSTATUS RegisterSniCallout(PDEVICE_OBJECT deviceObject);

NTSTATUS NTAPI SniNotifyFn(FWPS_CALLOUT_NOTIFY_TYPE notifyType,
	const GUID* filterKey, FWPS_FILTER0* filter);

void NTAPI SniClassifyFn(const FWPS_INCOMING_VALUES0* inFixedValues,
	const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
	void* layerData,
	//const void* classiftyContext,
	const FWPS_FILTER0* filter,
	UINT64 flowContext,
	FWPS_CLASSIFY_OUT0* classifyOut);


void addDomain(PIRP p_irp, PIO_STACK_LOCATION p_stack);
void removeDomain(PIRP p_irp, PIO_STACK_LOCATION p_stack);

void init_Q_Domain();

bool isBlockedSni(char* sni);












