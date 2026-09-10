#include "pch.h"
#include "CalloutDriverClient.h"
#include "../../../SharedIoctl/SharedIoctl.h"
#include "../../Model/DomainBlockModel.h"

CalloutDriverClient::CalloutDriverClient(DomainBlockModel* p_DomainBlock_Model)
{
	this->p_DomainBlock_Model = p_DomainBlock_Model;
	this->open();
}
CalloutDriverClient::~CalloutDriverClient()
{
	this->close();
}

void CalloutDriverClient::open()
{
	this->h_CalloutDriver =
		CreateFile(
			L"\\\\.\\DomainGuard",
			GENERIC_READ | GENERIC_WRITE,
			0,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);

	if (this->h_CalloutDriver == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("open _ CreateFile ___ ERR [%d]"), ::GetLastError());
	}
}

void CalloutDriverClient::close()
{
	auto vec_BlockDomain = this->p_DomainBlock_Model->get_vec_BlockDomain();

	for (auto& v : vec_BlockDomain)
	{
		DOMAIN_REQUEST domain_Driver = { 0 };
		domain_Driver.len = v.domain.GetLength();
		domain_Driver.block = v.block;
		//std::string tmp = CW2A(v.domain.GetString(), CP_UTF8);
		memcpy(domain_Driver.domain, v.domain.GetString(),
			domain_Driver.len );
		domain_Driver.domain[domain_Driver.len] = _T('\0');

		DWORD ret;

		bool ret2 =
			::DeviceIoControl(
				this->h_CalloutDriver, IOCTL_REMOVE_DOMAIN,
				(LPVOID)&domain_Driver, sizeof(domain_Driver),
				nullptr, 0,
				&ret, nullptr);

		if (ret2 == false)
		{
			TRACE(_T("close ___ ERR %d"), ::GetLastError());
		}

		// 도메인 삭제 처리가 끝난 뒤 추가
		if (::CloseHandle(h_CalloutDriver))
		{
			h_CalloutDriver = INVALID_HANDLE_VALUE;
		}
		else
		{
			TRACE(_T("CloseHandle ERR: %lu\n"), ::GetLastError());
		}
	}
}

void CalloutDriverClient::init_Post()
{
	auto vec_BlockDomain = this->p_DomainBlock_Model->get_vec_BlockDomain();

	for (auto& v : vec_BlockDomain)
	{
		if (v.block == false)
		{
			continue;
		}

		DOMAIN_REQUEST domain_Driver = { 0 };
		domain_Driver.len = v.domain.GetLength();
		domain_Driver.block = v.block;

		std::string tmp = CW2A(v.domain.GetString(), CP_UTF8);
		memcpy(domain_Driver.domain, tmp.c_str(),
			domain_Driver.len);

		domain_Driver.domain[domain_Driver.len] = _T('\0');

		DWORD ret;

		bool ret2 =
			::DeviceIoControl(
				this->h_CalloutDriver, IOCTL_ADD_DOMAIN,
				(LPVOID)&domain_Driver, sizeof(domain_Driver),
				nullptr, 0,
				&ret, nullptr);

		if (ret2 == false)
		{
			TRACE(_T("init_Post ___ ERR %d"), ::GetLastError());
		}
	}

	return;
}

void CalloutDriverClient::insert_Domain(blockDomain _blockDomain)
{
	if (_blockDomain.block == false)
	{
		return;
	}

	DOMAIN_REQUEST domain_Driver = { 0 };
	domain_Driver.len = _blockDomain.domain.GetLength();
	domain_Driver.block = _blockDomain.block;
	std::string tmp = CW2A(_blockDomain.domain.GetString(), CP_UTF8);

	memcpy(domain_Driver.domain, tmp.c_str(),
		domain_Driver.len);

	domain_Driver.domain[domain_Driver.len] = _T('\0');

	DWORD ret;

	bool ret2 =
		::DeviceIoControl(
			this->h_CalloutDriver, IOCTL_ADD_DOMAIN,
			(LPVOID)&domain_Driver, sizeof(domain_Driver),
			nullptr, 0,
			&ret, nullptr);

	if (ret2 == false)
	{
		TRACE(_T("insert_Domain ___ ERR %d"), ::GetLastError());
	}

	return;
}

void CalloutDriverClient::update_Domain(blockDomain _blockDomain)
{
	DOMAIN_REQUEST domain_Driver = { 0 };
	domain_Driver.len = _blockDomain.domain.GetLength();
	domain_Driver.block = _blockDomain.block;
	std::string tmp = CW2A(_blockDomain.domain.GetString(), CP_UTF8);

	memcpy(domain_Driver.domain, tmp.c_str(),
		domain_Driver.len);

	domain_Driver.domain[domain_Driver.len] = _T('\0');


	DWORD ret;

	if (_blockDomain.block == true)
	{
		bool ret2 =
			::DeviceIoControl(
				this->h_CalloutDriver, IOCTL_ADD_DOMAIN,
				(LPVOID)&domain_Driver, sizeof(domain_Driver),
				nullptr, 0,
				&ret, nullptr);

		if (ret2 == false)
		{
			TRACE(_T("update_Domain ___ true ___ ERR %d"), ::GetLastError());
		}
	}
	else if (_blockDomain.block == false)
	{
		bool ret2 =
			::DeviceIoControl(
				this->h_CalloutDriver, IOCTL_REMOVE_DOMAIN,
				(LPVOID)&domain_Driver, sizeof(domain_Driver),
				nullptr, 0,
				&ret, nullptr);

		if (ret2 == false)
		{
			TRACE(_T("update_Domain ___ false ___ ERR %d"), ::GetLastError());
		}
	}
}

void CalloutDriverClient::remove_Domain(blockDomain _blockDomain)
{
	DOMAIN_REQUEST domain_Driver = { 0 };
	domain_Driver.len = _blockDomain.domain.GetLength();
	domain_Driver.block = _blockDomain.block;
	std::string tmp = CW2A(_blockDomain.domain.GetString(), CP_UTF8);

	memcpy(domain_Driver.domain, tmp.c_str(),
		domain_Driver.len);

	domain_Driver.domain[domain_Driver.len] = _T('\0');


	DWORD ret;

	bool ret2 =
		::DeviceIoControl(
			this->h_CalloutDriver, IOCTL_REMOVE_DOMAIN,
			(LPVOID)&domain_Driver, sizeof(domain_Driver),
			nullptr, 0,
			&ret, nullptr);

	if (ret2 == false)
	{
		TRACE(_T("update_Domain ___ false ___ ERR %d"), ::GetLastError());
	}

	return;
}

void CalloutDriverClient::test_Driver_Q_TEXT()
{
	DWORD ret;
	bool ret2 =
		::DeviceIoControl(
			this->h_CalloutDriver, IOCTL_KDPRINT_DOMAIN,
			nullptr, 0,
			nullptr, 0,
			&ret, nullptr);
	if (ret2 == false)
	{
		TRACE(_T("test_Driver_Q_TEXT ___ ERR %d"), ::GetLastError());
	}
	return;
}

ARR_DRIVERLOG CalloutDriverClient::get_DriverLog()
{
	DWORD ret = 0;

	ARR_DRIVERLOG arr = { 0 };
	/*
	strcpy_s(arr.arrDriverLog[0].domain, "hello11111");
	arr.arrDriverLog[0].time = 0;

	strcpy_s(arr.arrDriverLog[1].domain, "hello22222");
	arr.arrDriverLog[1].time = 1;

	strcpy_s(arr.arrDriverLog[2].domain, "hello33333");
	arr.arrDriverLog[2].time = 2;*/

	bool ret2 =
		::DeviceIoControl(
			this->h_CalloutDriver, IOCTL_GET_ARR_DRIVERLOG,
			nullptr, 0,
			(LPVOID)&arr, sizeof(ARR_DRIVERLOG),
			&ret, nullptr);

	if (ret2 == false)
	{
		TRACE(_T("get_DriverLog ___ false ___ ERR %d"), ::GetLastError());
	}

	return arr;
}

/*
#define IOCTL_ADD_DOMAIN                     \
	CTL_CODE(                                \
		FILE_DEVICE_UNKNOWN,                 \
		0x800,                               \
		METHOD_BUFFERED,                     \
		FILE_WRITE_DATA                      \
	)
*/















