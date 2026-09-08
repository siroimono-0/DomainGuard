#pragma once

enum SNI_RET
{
	SNI_FOUND = 0,
	SNI_NOT_FOUND = 0,
	SNI_NEED_MORE = 0
};

#define TLS_MAX_RECORD_SIZE (64*1024)
