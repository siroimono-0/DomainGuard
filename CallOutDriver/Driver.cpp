#include "Driver.h"
#include "SniPaser.h"
#pragma comment(lib, "Fwpkclnt.lib")

Q_Domain g_Q_Domain = {};
UINT32 g_calloutId_V4 = 0;
UINT32 g_calloutId_V6 = 0;

void init_Q_Domain()
{
	InitializeListHead(&g_Q_Domain.head);
	KeInitializeSpinLock(&g_Q_Domain.lock);
}

extern "C"
NTSTATUS DriverEntry(
	PDRIVER_OBJECT driverObject,
	PUNICODE_STRING registryPath)
{
	// registryPath 매개변수를 현재 사용하지 않는다는 뜻입니다.
	UNREFERENCED_PARAMETER(registryPath);

	init_Q_Domain();

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
			"IoCreateDevice __ ERR __ 0x%08X\n",
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
			"IoCreateSymbolicLink __ ERR __ 0x%08X\n",
			(ULONG)stat
			));
		IoDeleteDevice(deviceObject);
		return stat;
	}

	/*
	 장치 초기화가 모두 끝났다고 Windows에 표시합니다.
	 이 플래그를 제거해야 정상적으로 IRP를 받을 수 있습니다.
	*/
	deviceObject->Flags &=
		~DO_DEVICE_INITIALIZING;

	stat = RegisterSniCallout(deviceObject);
	if (!NT_SUCCESS(stat))
	{
		KdPrint((
			" RegisterSniCallout __ ERR __ 0x%08X\n",
			(ULONG)stat
			));
		IoDeleteDevice(deviceObject);
		return stat;
	}

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
		addDomain(irp, stack);
		status = STATUS_SUCCESS;
	}
	else if (ioctlCode == IOCTL_REMOVE_DOMAIN)
	{
		KdPrint(("DomainGuard: DispatchDeviceControl -  IOCTL_REMOVE_DOMAIN called\n"));
		removeDomain(irp, stack);
		status = STATUS_SUCCESS;
	}
	else if (ioctlCode == IOCTL_KDPRINT_DOMAIN)
	{
		KdPrint(("DomainGuard: DispatchDeviceControl -  IOCTL_KDPRINT_DOMAIN called\n"));
		KDPRINT_DOMAIN();
		status = STATUS_SUCCESS;
	}
	else
	{
		status = STATUS_INVALID_DEVICE_REQUEST;
	}


	return CompleteIrp(irp, status, 0);
}

void addDomain(PIRP p_irp, PIO_STACK_LOCATION p_stack)
{
	UNREFERENCED_PARAMETER(p_stack);
	KdPrint(("DomainGuard: addDomain\n"));
	DOMAIN_REQUEST* p_Domain_Request = (DOMAIN_REQUEST*)p_irp->AssociatedIrp.SystemBuffer;
	if (p_Domain_Request->block == true)
	{
		DOMAIN_REQUEST* p_DOMAIN_REQUEST =
			(DOMAIN_REQUEST*)ExAllocatePool2(POOL_FLAG_NON_PAGED,
				sizeof(DOMAIN_REQUEST),
				Q_Domain_Entry_Tag);

		if (p_DOMAIN_REQUEST == nullptr)
		{
			KdPrint(("DomainGuard: addDomain ___ ExAllocatePool2 ___ ERR\n"));
			return;
		}

		p_DOMAIN_REQUEST->block = p_Domain_Request->block;
		p_DOMAIN_REQUEST->len = p_Domain_Request->len;
		memcpy(p_DOMAIN_REQUEST->domain, p_Domain_Request->domain,
			p_DOMAIN_REQUEST->len);


		KIRQL oldIrql;
		KeAcquireSpinLock(&g_Q_Domain.lock, &oldIrql);
		InsertTailList(&g_Q_Domain.head, &p_DOMAIN_REQUEST->link);
		KeReleaseSpinLock(&g_Q_Domain.lock, oldIrql);
	}
}

void removeDomain(PIRP p_irp, PIO_STACK_LOCATION p_stack)
{
	UNREFERENCED_PARAMETER(p_stack);
	KdPrint(("DomainGuard: removeDomain\n"));
	DOMAIN_REQUEST* p_Domain_Request = (DOMAIN_REQUEST*)p_irp->AssociatedIrp.SystemBuffer;

	KIRQL oldIrql;
	KeAcquireSpinLock(&g_Q_Domain.lock, &oldIrql);
	PLIST_ENTRY link = g_Q_Domain.head.Flink;

	DOMAIN_REQUEST* remove_Entry = nullptr;

	while (link != &g_Q_Domain.head)
	{
		PLIST_ENTRY next = link->Flink;
		DOMAIN_REQUEST* tmp_Domain_Request =
			CONTAINING_RECORD(link, DOMAIN_REQUEST, link);

		if (p_Domain_Request->len != tmp_Domain_Request->len)
		{
			link = next;
			continue;
		}

		bool ret =
			RtlEqualMemory((const void*)p_Domain_Request->domain,
				(const void*)tmp_Domain_Request->domain,
				tmp_Domain_Request->len);

		if (ret == TRUE)
		{
			RemoveEntryList(link);
			remove_Entry = tmp_Domain_Request;
			break;
		}

		link = next;
	}
	KeReleaseSpinLock(&g_Q_Domain.lock, oldIrql);

	if (remove_Entry != nullptr)
	{
		ExFreePoolWithTag(remove_Entry, Q_Domain_Entry_Tag);
	}
	return;
}

void DriverUnload(
	PDRIVER_OBJECT driverObject
)
{
	//UNREFERENCED_PARAMETER(driverObject);

	// MFC에서 접근 가능하게 노출하는 이름
	UNICODE_STRING symbolicLinkName;
	RtlInitUnicodeString(&symbolicLinkName, L"\\DosDevices\\DomainGuard");

	NTSTATUS stat = STATUS_SUCCESS;

	if (g_calloutId_V4 != 0)
	{
		stat = FwpsCalloutUnregisterById0(g_calloutId_V4);
		KdPrint(("Domainguard: DriverUnload - FwpsCalloutUnregisterById0 ___ "
			"stat = 0x08X \n", stat));

		g_calloutId_V4 = 0;
	}

	if (g_calloutId_V6 != 0)
	{
		stat = FwpsCalloutUnregisterById0(g_calloutId_V6);
		KdPrint(("Domainguard: DriverUnload - FwpsCalloutUnregisterById0 ___ "
			"stat = 0x08X \n", stat));

		g_calloutId_V6 = 0;
	}

	IoDeleteSymbolicLink(&symbolicLinkName);

	if (driverObject->DeviceObject != nullptr)
	{
		IoDeleteDevice(driverObject->DeviceObject);
	}


	KdPrint(("DomainGuard: DriverUnload // Fact __ \n"));
	return;
}

void KDPRINT_DOMAIN()
{
	KIRQL oldIrql;
	KeAcquireSpinLock(&g_Q_Domain.lock, &oldIrql);
	KdPrint(("\nDomainGuard: KDPRINT_DOMAIN\n"));

	PLIST_ENTRY link = g_Q_Domain.head.Flink;

	while (link != &g_Q_Domain.head)
	{
		PLIST_ENTRY next = link->Flink;

		DOMAIN_REQUEST* tmp_Domain_Request =
			CONTAINING_RECORD(link, DOMAIN_REQUEST, link);

		KdPrint(("\n\n %s \n\n", tmp_Domain_Request->domain));

		link = next;
	}
	KdPrint(("\n------------------------------\n"));
	KeReleaseSpinLock(&g_Q_Domain.lock, oldIrql);

	return;
}

NTSTATUS RegisterSniCallout(PDEVICE_OBJECT deviceObject)
{
	NTSTATUS ret = STATUS_SUCCESS;
	FWPS_CALLOUT0 callout = {};
	callout.calloutKey = GUID_Callout_V4;
	callout.flags = 0;

	callout.classifyFn = SniClassifyFn;
	callout.notifyFn = SniNotifyFn;
	callout.flowDeleteFn = nullptr;

	ret =
		::FwpsCalloutRegister0(
			deviceObject,
			&callout,
			&g_calloutId_V4);

	if (!NT_SUCCESS(ret))
	{
		KdPrint(("ERR ___ DomainGuard: RegisterSniCallout - FwpsCalloutRegister0 __ V4 \n"));
		return ret;
	}

	KdPrint(("DomainGuard: RegisterSniCallout ___ ID=%lu \n", g_calloutId_V4));


	ret = STATUS_SUCCESS;
	callout = {};
	callout.calloutKey = GUID_Callout_V6;
	callout.flags = 0;

	callout.classifyFn = SniClassifyFn;
	callout.notifyFn = SniNotifyFn;
	callout.flowDeleteFn = nullptr;

	ret =
		::FwpsCalloutRegister0(
			deviceObject,
			&callout,
			&g_calloutId_V6);

	if (!NT_SUCCESS(ret))
	{
		KdPrint(("ERR ___ DomainGuard: RegisterSniCallout - FwpsCalloutRegister0 __ V4 \n"));
		return ret;
	}

	KdPrint(("DomainGuard: RegisterSniCallout ___ ID=%lu \n", g_calloutId_V6));

	return ret;
}

NTSTATUS NTAPI SniNotifyFn(FWPS_CALLOUT_NOTIFY_TYPE notifyType,
	const GUID* filterKey, FWPS_FILTER0* filter)
{
	UNREFERENCED_PARAMETER(filter);
	UNREFERENCED_PARAMETER(filterKey);
	//UNREFERENCED_PARAMETER(registryPath);
	NTSTATUS ret = STATUS_SUCCESS;

	if (notifyType == FWPS_CALLOUT_NOTIFY_ADD_FILTER)
	{
		KdPrint(("DomainGuard: SniNotifyFn ___ FWPS_CALLOUT_NOTIFY_ADD_FILTER \n"));
	}
	else if (notifyType == FWPS_CALLOUT_NOTIFY_DELETE_FILTER)
	{
		KdPrint(("DomainGuard: SniNotifyFn ___ FWPS_CALLOUT_NOTIFY_DELETE_FILTER \n"));
	}
	else
	{
		KdPrint(("DomainGuard: SniNotifyFn ___ ???\n"));
	}
	return ret;
}

/*
* WFP STREAM 레이어에 등록되는 Classify 콜백 함수입니다.
* MFC에서 FWPM_LAYER_STREAM_V4에 Callout Filter를 등록해 두었다면,
* 해당 필터 조건에 맞는 TCP 스트림 데이터가 들어올 때 WFP가 이 함수를 호출합니다.
*/
void NTAPI SniClassifyFn(const FWPS_INCOMING_VALUES0* inFixedValues,
	const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
	void* layerData,
	//const void* classiftyContext,
	const FWPS_FILTER0* filter,
	UINT64 flowContext,
	FWPS_CLASSIFY_OUT0* classifyOut)
{
	UNREFERENCED_PARAMETER(inMetaValues);
	UNREFERENCED_PARAMETER(filter);
	UNREFERENCED_PARAMETER(flowContext);

	/*
	classifyOut은 "이 패킷/스트림을 허용할 것인지 차단할 것인지"
	WFP에게 알려주는 결과 구조체입니다.
	NULL이라면 결과를 기록할 수 없으므로
	더 이상 처리하지 않고 종료합니다. */
	if (classifyOut == nullptr)
	{
		return;
	}

	/* FWPS_RIGHT_ACTION_WRITE가 있다는 것은
	* 현재 Callout이 classifyOut->actionType에
	* PERMIT / BLOCK 같은 판정 결과를 기록할 권한이 있다는 뜻입니다.
	* 이 권한이 없다면 classifyOut->actionType을 변경하면 안 됩니다. */
	if ((classifyOut->rights & FWPS_RIGHT_ACTION_WRITE) == 0)
	{
		KdPrint(("DomainGuard: SniClassifyFn - no ACTION_WRITE right\n"));
		return;
	}

	/* FWPM_LAYER_STREAM_V4에서 Classify 함수가 호출되는 경우
	* layerData를 통해 TCP Stream 데이터가 전달됩니다.
	* layerData == nullptr이면
	* 현재 처리할 스트림 데이터가 없다는 뜻이므로 종료합니다. */
	if (layerData == nullptr)
	{
		KdPrint(("DomainGuard: SniClassifyFn - layerData is NULL\n"));
		return;
	}

	FWPS_STREAM_CALLOUT_IO_PACKET0* streamPacket =
		(FWPS_STREAM_CALLOUT_IO_PACKET0*)layerData;

	/* streamData 안에 실제 TCP Stream 데이터가 들어 있습니다.
	* streamData가 NULL이라면 * 현재 읽을 수 있는 데이터가 없으므로
	* 스트림에 별도의 처리를 하지 않고 종료합니다. */
	if (streamPacket->streamData == nullptr)
	{
		KdPrint(("DomainGuard: SniClassifyFn - streamData is NULL\n"));

		// streamPacket->streamData에 요구하는 동작 설정
		streamPacket->streamAction = FWPS_STREAM_ACTION_NONE;

		// 추가 요구 데이터
		streamPacket->countBytesRequired = 0;

		// 이번 호출시 처리한 데이터
		streamPacket->countBytesEnforced = 0;
		return;
	}

	FWPS_STREAM_DATA0* streamData = streamPacket->streamData;

	// 현재 WFP가 전달해준 TCP Stream Data길이
	SIZE_T streamLength = streamPacket->streamData->dataLength;

	// 현재 Stream Data의 상태를 나타내는 Flag
	//UINT32 streamFlags = streamPacket->streamData->flags;

	// 어떤 WFP Layer에서 호출댓는지 확인을 위함 - DebugView
	UINT16 layerId = 0;

	if (inFixedValues != nullptr)
	{
		layerId = inFixedValues->layerId;
	}


	/* DebugView 확인용 로그입니다.
	* 브라우저에서 HTTPS 사이트에 접속했을 때
	* 이 메시지가 출력된다면
	* Filter
	* ↓
	* Callout
	* ↓
	* SniClassifyFn()
	* 연결까지 정상적으로 이루어진 것입니다.
	KdPrint((
		"DomainGuard: SniClassifyFn called "
		"layer=%u, bytes=%lu, flags=0x%08X\n",
		layerId, static_cast<ULONG>(streamLenght), streamFlags));
	*/

	// 기본 동작 -> 전체허용
	// streamPacket->streamData에 요구하는 동작 설정
	streamPacket->streamAction = FWPS_STREAM_ACTION_NONE;

	// 추가 요구 데이터
	streamPacket->countBytesRequired = 0;

	// 이번 호출시 처리한 데이터
	streamPacket->countBytesEnforced = streamLength;

	// 일단 무조건 허용
	classifyOut->actionType = FWP_ACTION_PERMIT;

	if (streamData->dataLength == 0)
	{
		return;
	}

	// SEND 방향만 검사
	// TLS ClientHello는 클라 -> 서버 방향임
	if ((streamData->flags & FWPS_STREAM_FLAG_SEND) == 0)
	{
		return;
	}

	size_t copyLen = streamData->dataLength;

	if (copyLen > TLS_MAX_RECORD_SIZE)
	{
		copyLen = TLS_MAX_RECORD_SIZE;
	}

	UINT8* buf = (UINT8*)ExAllocatePool2(
		POOL_FLAG_NON_PAGED,
		copyLen, 'inSD');

	size_t copyByteLen = 0;
	if (buf == nullptr)
	{
		return;
	}

	FwpsCopyStreamDataToBuffer0(
		streamData, buf, copyLen, &copyByteLen);

	char sni[256] = { 0 };
	size_t reqSize = 0;
	SNI_RET sniRet = SNI_Paser(buf, copyByteLen, sni, 256, &reqSize);

	ExFreePoolWithTag(buf, 'inSD');

	if (sniRet == SNI_NEED_MORE)
	{
		// 일단 넘김
		return;
	}

	if (sniRet != SNI_FOUND)
	{
		return;
	}

	bool blocked = isBlockedSni(sni);

	if (blocked == true)
	{
		streamPacket->streamAction = FWPS_STREAM_ACTION_DROP_CONNECTION;
		streamPacket->countBytesRequired = 0;
		streamPacket->countBytesEnforced = 0;

		classifyOut->actionType = FWP_ACTION_NONE;
		return;
	}

}

bool isBlockedSni(char* sni)
{
	PLIST_ENTRY link = g_Q_Domain.head.Flink;
	size_t sniSize = strlen(sni);

	KIRQL oldIrql;
	KeAcquireSpinLock(&g_Q_Domain.lock, &oldIrql);

	while (link != &g_Q_Domain.head)
	{
		PLIST_ENTRY next = link->Flink;

		DOMAIN_REQUEST* tmp_Domain_Request =
			CONTAINING_RECORD(link, DOMAIN_REQUEST, link);

		if (strcmp(sni, tmp_Domain_Request->domain) == 0 &&
			tmp_Domain_Request->block == true)
		{
			KdPrint(("\n\n BLOCK :: %s \n\n", tmp_Domain_Request->domain));
			KeReleaseSpinLock(&g_Q_Domain.lock, oldIrql);
			return true;
		}

		link = next;
	}
	KeReleaseSpinLock(&g_Q_Domain.lock, oldIrql);

	int start = -1;
	for (int i = 0; i < sniSize; i++)
	{
		if (sni[i] == '.')
		{
			start = i + 1;
			//sni += (i + 1);
			break;
		}
	}

	if (start == -1)
	{
		return false;
	}

	return isBlockedSni(sni + start);
}

































