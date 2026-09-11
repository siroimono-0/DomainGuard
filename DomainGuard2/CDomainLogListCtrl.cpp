#include "pch.h"
#include "CDomainLogListCtrl.h"

CDomainLogListCtrl::CDomainLogListCtrl()
{

}

CDomainLogListCtrl::~CDomainLogListCtrl()
{

}

BEGIN_MESSAGE_MAP(CDomainLogListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CDomainLogListCtrl::AddBlockItem(const CString& domain, const bool block)
{
	return;
}

void CDomainLogListCtrl::DeleteBlockItem(const CString& domain)
{
	return;
}

void CDomainLogListCtrl::set_h_View(HWND set)
{
	return;
}

void CDomainLogListCtrl::PreSubclassWindow()
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
		_T("RequestTime"),
		LVCFMT_CENTER, 150);

	this->InsertColumn(
		1,
		_T("ResponeseTime"),
		LVCFMT_CENTER, 150);

	this->InsertColumn(
		2,
		_T("Domain"),
		LVCFMT_CENTER, 200);

	this->InsertColumn(
		3,
		_T("QueryType"),
		LVCFMT_CENTER, 100);

	this->InsertColumn(
		4,
		_T("Result"),
		LVCFMT_CENTER, 100);

	CRect client;
	GetClientRect(&client);

	int columnsWidth = 0;
	for (int i = 0; i < 5; i++)
	{
		if (i == 2)
		{
			continue;
		}
		columnsWidth += GetColumnWidth(i);
	}

	int domainWidth = client.Width() - columnsWidth;
	SetColumnWidth(2, domainWidth);
	//RedrawWindow();
	return;
}


void  CDomainLogListCtrl::update_DomainLogList(un_map_DNSLogItem un_map)
{
	for (auto it = un_map.begin(); it != un_map.end(); it++)
	{
		int year = it->second.requestTime.wYear;
		int month = it->second.requestTime.wMonth;
		int day = it->second.requestTime.wDay;
		int hour = it->second.requestTime.wHour;
		int minute = it->second.requestTime.wMinute;
		int second = it->second.requestTime.wSecond;

		CString s_RequestTime;
		s_RequestTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
			year, month, day, hour, minute, second);

		int row = this->GetItemCount();

		this->InsertItem(row, s_RequestTime);

		int responeseTime = it->second.responeseTime;
		CString s_ResponeseTime;
		s_ResponeseTime.Format(_T("%d ms"), responeseTime);
		this->SetItemText(row, 1, s_ResponeseTime);

		this->SetItemText(row, 2, it->second.domain);

		this->SetItemText(row, 3, it->second.queryType);

		if (it->second.result == DnsLogResult::Allowed)
		{
			this->SetItemText(row, 4, _T("Allowed"));
		}
		else if (it->second.result == DnsLogResult::Blocked)
		{
			this->SetItemText(row, 4, _T("Blocked"));
		}
	}
}
