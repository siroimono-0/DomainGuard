#include "pch.h"
#include "DriverLogModel.h"

void  DriverLogModel::add_DriverLogItem(ARR_DRIVERLOG arr)
{
	for (int i = 0; i < 32; i++)
	{
		if (arr.arrDriverLog[i].domain[0] != 0)
		{
			DriverLogItem item;
			ULARGE_INTEGER time = {};
			time.QuadPart = (ULONGLONG)arr.arrDriverLog[i].time;

			FILETIME utcTime = {};
			utcTime.dwLowDateTime = time.LowPart;
			utcTime.dwHighDateTime = time.HighPart;

			FILETIME localTime = {};
			SYSTEMTIME sysTime = {};

			int ret1 = ::FileTimeToLocalFileTime(&utcTime, &localTime);
			int ret2 = ::FileTimeToSystemTime(&localTime, &sysTime);

			CString cs = _T("");
			if (ret1 && ret2)
			{
				cs.Format(_T("%04u-%02u %02u:%02u:%02u"),
					sysTime.wYear, sysTime.wMonth, sysTime.wDay,
					sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			}

			item.s_BlockTime = cs;
			item.BlockTime = sysTime;
			item.result = DriverLogResult::Blocked;

			CString tmp;
			tmp.Format(_T("%S"), arr.arrDriverLog[i].domain);
			item.domain = tmp;


			vec_DriverLogItem.push_back(item);
			tmp_vec_DriverLogItem.push_back(item);
		}
	}
	//this->vec_DriverLogItem.push_back(item);
	return;
}

vecDriverLogItem DriverLogModel::get_tmp_vec_DriverLogItem()
{
	return std::move(this->tmp_vec_DriverLogItem);
}
