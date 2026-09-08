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
#include "./Driver_pch.h"

UINT16 readUINT16_BE(const UINT8* data);
UINT32 readUINT32_BE(const UINT8* data);
bool moveByte(size_t end, size_t* pos, size_t mv);

SNI_RET SNI_Paser(
	const UINT8* data, size_t dataLen,
	char* sni, size_t sniLen, size_t* reqSize);


