#include "SniPaser.h"

UINT16 readUINT16_BE(const UINT8* data)
{
	UINT16 ret = 0;
	// 16비트로 만들고 아랫값 위로 비트연산 - 상위값 생성됨
	// 아랫값은 그냥 or 로 채움
	ret = ((UINT16)data[0]) << 8 | ((UINT16)data[1]);
	return ret;
}

UINT32 readUINT32_BE(const UINT8* data)
{
	UINT32 ret = 0;
	ret = ((UINT32)data[0] << 16) | ((UINT32)data[1] << 8) | ((UINT32)data[2]);
	return ret;
}

bool moveByte(size_t end, size_t* pos, size_t mv)
{
	if (*pos > end)
	{
		return false;
	}

	if (mv > end - *pos)
	{
		return false;
	}

	*pos += mv;
	return true;
}

SNI_RET SNI_Paser(
	const UINT8* data, size_t dataLen,
	char* sni, size_t sniLen, size_t* reqSize)
{
	// TLS Header Size는 5바이트
	if (dataLen < 5)
	{
		return SNI_NEED_MORE;
	}

	// 핸드쉐이크인지 확인
	if (data[0] != 0x16)
	{
		return SNI_NOT_FOUND;
	}

	// 빅엔디안 -> 리틀 / 핸드쉐이크 메시지 데이터 크기
	size_t recordLen = readUINT16_BE(data + 3);

	// TLS 값 초과
	if (recordLen > TLS_MAX_RECORD_SIZE - 5)
	{
		return SNI_NOT_FOUND;
	}

	// 레코드 길이 + 레코드헤더 길이
	size_t recordEnd = recordLen + 5;

	// 현재 확인 데이터 길이가 총길이보다 작으면 
	// 추가데이터 더 받아와야댐
	// 추가 요청 데이터 사이즈 입력 
	if (recordEnd > dataLen)
	{
		*reqSize = recordEnd;
		return SNI_NEED_MORE;
	}
	size_t pos = 5;

	// Handshake Header 사이즈는 4임
	// 즉 핸드쉐이크 헤더가 없다? 
	if (recordEnd - pos < 4)
	{
		return SNI_NOT_FOUND;
	}

	// ClientHello인지 확인
	if (data[pos] != 0x01)
	{
		return SNI_NOT_FOUND;
	}

	// 핸드쉐이크 읽었으니 이동
	pos += 1;

	UINT32 handshakeLen = readUINT32_BE(data + (pos));

	// 핸드쉐이크 길이 찾았으니 이동
	pos += 3;

	// 핸드쉐이크 잘렸으면 일단 NotFound 추후 수정필요
	if (handshakeLen > recordEnd - pos)
	{
		return SNI_NOT_FOUND;
	}

	size_t handshakeEnd = pos + handshakeLen;

	// ClientHello Body 중 Lagacy Version / Random 필드 건너뜀
	// 그 때 오버 대는지 확인
	if (!moveByte(handshakeEnd, &pos, 32 + 2))
	{
		return SNI_NOT_FOUND;
	}

	if (pos >= handshakeEnd)
	{
		return SNI_NOT_FOUND;
	}

	size_t sessionIdLen = data[pos];
	pos++;

	if (!moveByte(handshakeEnd, &pos, sessionIdLen))
	{
		return SNI_NOT_FOUND;
	}

	if (pos > handshakeEnd || handshakeEnd - pos < 2)
	{
		return SNI_NOT_FOUND;
	}

	size_t chipherSuitesLen = readUINT16_BE(data + pos);

	pos += 2;

	// CipherSuites 건너뜀
	if (!moveByte(handshakeEnd, &pos, chipherSuitesLen))
	{
		return SNI_NOT_FOUND;
	}

	if (pos >= handshakeEnd)
	{
		return SNI_NOT_FOUND;
	}

	size_t compressionLen = data[pos];
	pos++;

	// Compression Methods 건너뜀
	if (!moveByte(handshakeEnd, &pos, compressionLen))
	{
		return SNI_NOT_FOUND;
	}

	UINT16 extensionLen_ALL = readUINT16_BE(data + pos);
	pos += 2;

	if (extensionLen_ALL > handshakeEnd - pos)
	{
		return SNI_NOT_FOUND;
	}

	size_t extensionEnd_ALL = pos + extensionLen_ALL;

	while (pos < extensionEnd_ALL)
	{
		// extension Header -> 4byte
		if (extensionEnd_ALL - pos < 4)
		{
			return SNI_NOT_FOUND;
		}

		UINT16 extensionType = readUINT16_BE(data + pos);
		UINT16 extensionLen = readUINT16_BE(data + (pos + 2));
		pos += 4;

		if (extensionLen > extensionEnd_ALL - pos)
		{
			return SNI_NOT_FOUND;
		}

		size_t extensionEnd = pos + extensionLen;

		// 0x0000 -> SNI(server name) 확장임
		if (extensionType == 0x0000)
		{
			size_t sniPos = pos;

			if (extensionEnd - sniPos < 2)
			{
				return SNI_NOT_FOUND;
			}

			size_t serverNameListLen = readUINT16_BE(data + sniPos);
			sniPos += 2;

			if (serverNameListLen > extensionEnd - sniPos)
			{
				return SNI_NOT_FOUND;
			}

			size_t serverNameListEnd = sniPos + serverNameListLen;

			/*
			* Server Name 항목:
			*
			* Name Type   : 1바이트
			* Name Length : 2바이트
			* Name        : 실제 도메인
			*/
			while (sniPos < serverNameListEnd)
			{
				if (serverNameListEnd - sniPos < 3)
				{
					return SNI_NOT_FOUND;
				}

				UINT8 nameType = data[sniPos];
				sniPos++;
				size_t nameLen = readUINT16_BE(data + sniPos);
				sniPos += 2;

				if (nameLen > serverNameListEnd - sniPos)
				{
					return SNI_NOT_FOUND;
				}

				if (nameType == 0x00)
				{
					if (nameLen == 0 || nameLen >= sniLen)
					{
						return SNI_NOT_FOUND;
					}

					for (int i = 0; i < nameLen; i++)
					{
						char c = (char)data[sniPos + i];

						if (isupper(c))
						{
							c = (char)tolower(c);
						}

						sni[i] = c;
					}
					sni[nameLen] = '\0';
					return SNI_FOUND;
				}
				sniPos += nameLen;
			}
			return SNI_NOT_FOUND;
		}
		pos = extensionEnd;
	}
	return SNI_NOT_FOUND;
}























