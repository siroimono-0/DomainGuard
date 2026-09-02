#include "Driver.h"
#include "../SharedIoctl/SharedIoctl.h"

extern "C"
NTSTATUS DriverEntry(
	PDRIVER_OBJECT driverObject,
	PUNICODE_STRING registryPath)
{
	// registryPath 매개변수를 현재 사용하지 않는다는 뜻입니다.
	UNREFERENCED_PARAMETER(registryPath);

	// 미정의된 번호들 미정의 함수 등록
	for (ULONG i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driverObject->MajorFunction[i] = DispatchUnsupported;
	}

	driverObject->DriverUnload = DriverUnload;

	driverObject->MajorFunction[IRP_MJ_CREATE]
		= DispatchCreate;

	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]
		= DispatchDeviceControl;

	driverObject->MajorFunction[IRP_MJ_CLOSE]
		= DispatchClose;

	// 커널에서 사용할 장치 이름
	UNICODE_STRING deviceName;
	RtlInitUnicodeString(&deviceName, L"\\Device\\DomainGuard");

	// 생선된 커널 객체 핸들
	PDEVICE_OBJECT deviceObject = nullptr;

	NTSTATUS stat = IoCreateDevice(
		driverObject,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN,
		FALSE,
		&deviceObject
	);

	if (!NT_SUCCESS(stat))
	{
		KdPrint((
			"IoCreateDevice __ ERR __ 0x08X\n",
			(ULONG)stat
			));
		return stat;
	}

	// MFC에서 접근 가능하게 노출하는 이름
	UNICODE_STRING symbolicLinkName;
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\DomainGuard");

	stat = IoCreateSymbolicLink(
		&symbolicLinkName,
		&deviceName
	);

	if (!NT_SUCCESS(stat))
	{
		KdPrint((
			"IoCreateSymbolicLink __ ERR __ 0x08X\n",
			(ULONG)stat
			));
		return stat;
	}

	/*
	 장치 초기화가 모두 끝났다고 Windows에 표시합니다.
	 이 플래그를 제거해야 정상적으로 IRP를 받을 수 있습니다.
	*/
	deviceObject->Flags &=
		~DO_DEVICE_INITIALIZING;

	KdPrint((
		"DomainGuard: device created successfully\n"
		));


	return STATUS_SUCCESS;
}

static NTSTATUS CompleteIrp(
	PIRP irp,
	NTSTATUS status,
	ULONG_PTR information
)
{
	irp->IoStatus.Status = status;
	//사용자 프로그램에 반환할 데이터의 바이트 수를 저장합니다.
	irp->IoStatus.Information = information;

	// “이 IRP 요청 처리가 끝났습니다”라고 Windows에 알려주는 함수
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchUnsupported(
	PDEVICE_OBJECT deviceObject, PIRP irp
)
{
	UNREFERENCED_PARAMETER(deviceObject);
	return CompleteIrp(irp, STATUS_INVALID_DEVICE_REQUEST, 0);
}

NTSTATUS DispatchCreate(
	PDEVICE_OBJECT deviceObject,
	PIRP irp
)
{
	UNREFERENCED_PARAMETER(deviceObject);
	KdPrint(("DomainGuard: DispatchCreate called\n"));
	return CompleteIrp(irp, STATUS_SUCCESS, 0);
}

NTSTATUS DispatchClose(
	PDEVICE_OBJECT deviceObject,
	PIRP irp
)
{
	UNREFERENCED_PARAMETER(deviceObject);
	KdPrint(("DomainGuard: DispatchClose called\n"));
	return CompleteIrp(irp, STATUS_SUCCESS, 0);
}

NTSTATUS DispatchDeviceControl(
	PDEVICE_OBJECT deviceObject,
	PIRP irp
)
{
	UNREFERENCED_PARAMETER(deviceObject);

	NTSTATUS status =
		STATUS_INVALID_DEVICE_REQUEST;

	// IoGetCurrentIrpStackLocation()은
	// 현재 드라이버가 확인해야 할 요청 정보를 가져옵니다.
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	ULONG ioctlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	//ULONG inputLength = stack->Parameters.DeviceIoControl.InputBufferLength;

	if (ioctlCode == IOCTL_ADD_DOMAIN)
	{
		KdPrint(("DomainGuard: DispatchDeviceControl - IOCTL_ADD_DOMAIN called\n"));
		status = STATUS_SUCCESS;
	}
	else if (ioctlCode == IOCTL_REMOVE_DOMAIN)
	{
		KdPrint(("DomainGuard: DispatchDeviceControl -  IOCTL_REMOVE_DOMAIN called\n"));
		status = STATUS_SUCCESS;
	}
	else
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
	}


	return CompleteIrp(irp, status, 0);
}

void DriverUnload(
	PDRIVER_OBJECT driverObject
)
{
	UNREFERENCED_PARAMETER(driverObject);

	// MFC에서 접근 가능하게 노출하는 이름
	UNICODE_STRING symbolicLinkName;
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\DomainGuard");

	IoDeleteSymbolicLink(&symbolicLinkName);

	if (driverObject->DeviceObject != nullptr)
	{
		IoDeleteDevice(driverObject->DeviceObject);
	}


	KdPrint(("DomainGuard: DriverUnload // Fact __ \n"));
	return;
}









