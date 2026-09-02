#pragma once

#include <ntddk.h>

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












