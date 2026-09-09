#include "pch.h"
#include "CDriverLogCtrl.h"

CDriverLogCtrl::CDriverLogCtrl()
{

}

CDriverLogCtrl::~CDriverLogCtrl()
{

}

BEGIN_MESSAGE_MAP(CDriverLogCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CDriverLogCtrl::AddBlockItem(const DriverLogItem item)
{
	/*
		int year = item.requestTime.wYear;
		int month = item.requestTime.wMonth;
		int day = item.requestTime.wDay;
		int hour = item.requestTime.wHour;
		int minute = item.requestTime.wMinute;
		int second = item.requestTime.wSecond;

		CString s_RequestTime;
		s_RequestTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
			year, month, day, hour, minute, second);
*/
	int row = this->GetItemCount();

	this->InsertItem(row, item.s_BlockTime);

	//int responeseTime = item.responeseTime;
	//CString s_ResponeseTime;
	//s_ResponeseTime.Format(_T("%d ms"), responeseTime);
	this->SetItemText(row, 1, item.domain);

	//this->SetItemText(row, 2, item.domain);

	//this->SetItemText(row, 3, item.queryType);

	if (item.result == DriverLogResult::Allowed)
	{
		this->SetItemText(row, 2, _T("Allowed"));
	}
	else if (item.result == DriverLogResult::Blocked)
	{
		this->SetItemText(row, 2, _T("Blocked"));
	}
	return;
}

void CDriverLogCtrl::DeleteBlockItem(const CString& domain)
{
	return;
}

void CDriverLogCtrl::set_h_View(HWND set)
{
	return;
}

void CDriverLogCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	//this->ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_OWNERDRAWFIXED);
	this->ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	this->SetExtendedStyle(
		this->GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES |
		LVS_EX_DOUBLEBUFFER
	);

	this->InsertColumn(
		0,
		_T("BlockTime"),
		LVCFMT_CENTER, 150);

	this->InsertColumn(
		1,
		_T("Server Name Indication"),
		LVCFMT_CENTER, 200);

	this->InsertColumn(
		2,
		_T("Result"),
		LVCFMT_CENTER, 200);


	//RedrawWindow();
	return;
}

