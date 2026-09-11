#include "pch.h"
#include "CalloutDriverClient.h"
#include "../../../SharedIoctl/SharedIoctl.h"
#include "../../Model/DomainBlockModel.h"
#include <winsvc.h>    // SCM 서비스 관련 함수
#include <string>      // std::wstring
#pragma comment(lib, "Advapi32.lib")

CalloutDriverClient::CalloutDriverClient(DomainBlockModel* p_DomainBlock_Model)
{
	DWORD errCode = 0;
	bool ret = this->calloutDriver_Load(&errCode);
	if (ret == false)
	{
		TRACE(_T("\n\n this->calloutDriver_Load(&errCode); ___ %d \n\n"), errCode);
	}

	this->p_DomainBlock_Model = p_DomainBlock_Model;
	this->open();
}

CalloutDriverClient::~CalloutDriverClient()
{
	this->close();
	DWORD errCode = 0;
	bool ret = this->calloutDriver_Unload(&errCode);
	if (ret == false)
	{
		TRACE(_T("\n\n this->calloutDriver_Unload(&errCode); ___ %d \n\n"), errCode);
	}
}

std::vector<string> CalloutDriverClient::paser(std::string s, char de)
{
	size_t start = 0;
	size_t eend = s.find(de);
	vector<string> vec;
	while (1)
	{
		auto tmp = s.substr(start, eend - start);
		vec.push_back(tmp);
		start = eend + 1;
		eend = s.find(de, start);

		if (eend == std::string::npos)
		{
			break;
		}
	}
	vec.push_back(s.substr(start));
	return vec;
}

bool CalloutDriverClient::calloutDriver_Load(DWORD* errCode)
{
	SC_HANDLE scm =
		::OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);

	if (scm == nullptr)
	{
		*errCode = ::GetLastError();
		return false;
	}

	WCHAR buf[256] = { 0 };
	auto tmp = ::GetCurrentDirectory(256, buf);
	CString s = buf;
	std::string s_tmp = CW2A(s.GetString(), CP_UTF8);
	char de = '\\';
	auto vec = this->paser(s_tmp, de);
	vec.pop_back();
	vec.push_back("x64");
	vec.push_back("Debug");
	vec.push_back("CallOutDriver.sys");

	std::string s_ret;
	for (auto& v : vec)
	{
		s_ret += (v + '\\');
	}
	s_ret.pop_back();

	CString ss_ret;
	ss_ret.Format(_T("%S"), s_ret.c_str());

	SC_HANDLE service =
		::CreateService(
			scm, this->serviceName.GetString(), this->serviceName.GetString(),
			SERVICE_START | SERVICE_STOP | DELETE, SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
			ss_ret.GetString(),
			nullptr, nullptr, nullptr, nullptr, nullptr);

	if (service == nullptr)
	{
		DWORD createErr = ::GetLastError();
		if (createErr == ERROR_SERVICE_EXISTS)
		{
			service =
				::OpenService(
					scm, ss_ret.GetString(), SERVICE_START | SERVICE_STOP | DELETE);

			if (service == nullptr)
			{
				DWORD openErr = ::GetLastError();
				::CloseServiceHandle(scm);
				*errCode = openErr;
				return false;
			}
		}
	}

	bool ret = ::StartService(service, 0, nullptr);
	if (!ret)
	{
		const DWORD startError = GetLastError();

		if (startError != ERROR_SERVICE_ALREADY_RUNNING)
		{
			DWORD openErr = ::GetLastError();
			::CloseServiceHandle(scm);
			*errCode = openErr;
			return false;
		}
	}

	::CloseServiceHandle(service);
	::CloseServiceHandle(scm);

	return true;
}

bool CalloutDriverClient::calloutDriver_Unload(DWORD* errCode)
{
	SC_HANDLE scm =
		::OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);

	if (scm == nullptr)
	{
		*errCode = ::GetLastError();
		return false;
	}

	SC_HANDLE service =
		::OpenService(scm, this->serviceName.GetString(),
			SERVICE_STOP | DELETE);

	if (service == nullptr)
	{
		DWORD openErr = ::GetLastError();
		::CloseServiceHandle(scm);

		if (openErr == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			return true;
		}
		else
		{
			*errCode = openErr;
			return false;
		}
	}

	SERVICE_STATUS stat = {};
	bool stopRet =
		::ControlService(service, SERVICE_CONTROL_STOP, &stat);

	if (!stopRet)
	{
		DWORD stopErr = ::GetLastError();
		if (stopErr != ERROR_SERVICE_NOT_ACTIVE)
		{
			::CloseServiceHandle(service);
			::CloseServiceHandle(scm);

			*errCode = stopErr;
			return false;
		}
	}

	bool deleteRet = ::DeleteService(service);

	if (!deleteRet)
	{
		const DWORD deleteError = GetLastError();

		if (deleteError != ERROR_SERVICE_MARKED_FOR_DELETE)
		{
			CloseServiceHandle(service);
			CloseServiceHandle(scm);

			if (errCode != nullptr)
			{
				*errCode = deleteError;
			}

			return false;
		}
	}
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















