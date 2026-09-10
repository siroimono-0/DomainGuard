#include "pch.h"
#include "CWFPListCtrl.h"

BEGIN_MESSAGE_MAP(CWFPListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


CWFPListCtrl::CWFPListCtrl()
{
	return;
}

CWFPListCtrl::~CWFPListCtrl()
{
	return;
}

void  CWFPListCtrl::set_h_View(HWND set)
{
	this->h_View = set;
}

void CWFPListCtrl::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	//this->ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_OWNERDRAWFIXED);
	this->ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_OWNERDRAWFIXED);
	this->SetExtendedStyle(
		this->GetExtendedStyle() |
		LVS_EX_FULLROWSELECT |
		LVS_EX_GRIDLINES |
		LVS_EX_DOUBLEBUFFER
	);
	this->InsertColumn(
		0,
		_T("FilePath"),
		LVCFMT_CENTER, 250);
	this->InsertColumn(
		1,
		_T("Direction"),
		LVCFMT_CENTER, 100);
	this->InsertColumn(
		2,
		_T("Protocol"),
		LVCFMT_CENTER, 100);
	this->InsertColumn(
		3,
		_T("IP"),
		LVCFMT_CENTER, 100);
	this->InsertColumn(
		4,
		_T("Port"),
		LVCFMT_CENTER, 100);
	this->InsertColumn(
		5,
		_T("Delete"),
		LVCFMT_CENTER, 100);

	CRect client;
	GetClientRect(&client);

	int columnsWidth = 0;

	for (int i = 1; i < 6; i++)
	{
		columnsWidth += GetColumnWidth(i);
	}

	int filePathWidth = client.Width() - columnsWidth;
	SetColumnWidth(0, filePathWidth);
	//RedrawWindow();
	return;
}

void CWFPListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDraw)
{
	CDC dc;
	dc.Attach(lpDraw->hDC);
	int row = (int)lpDraw->itemID;

	if (row < 0)
	{
		dc.Detach();
		return;
	}

	bool selected = (lpDraw->itemState & ODS_SELECTED) != 0;
	CRect rect(lpDraw->rcItem);

	COLORREF backCol;
	if (selected == true)
	{
		backCol = RGB(0, 120, 215);
	}
	else
	{
		backCol = RGB(255, 255, 255);
	}

	COLORREF textCol;
	if (selected == true)
	{
		textCol = RGB(255, 255, 255);
	}
	else
	{
		textCol = RGB(0, 0, 0);
	}

	dc.FillSolidRect(rect, backCol);
	dc.SetTextColor(textCol);
	dc.SetBkMode(TRANSPARENT);


	CRect domainRect = rect;
	domainRect.right = domainRect.left + GetColumnWidth(0);
	int col_left = domainRect.right;
	domainRect.DeflateRect(8, 0);

	dc.DrawText(GetItemText(row, 0), domainRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

	CRect directionRect = rect;
	directionRect.left = col_left;
	directionRect.right = directionRect.left + GetColumnWidth(1);
	col_left += directionRect.Width();
	directionRect.DeflateRect(8, 0);

	dc.DrawText(GetItemText(row, 1), directionRect, DT_CENTER);

	CRect protocolRect = rect;
	protocolRect.left = col_left;
	protocolRect.right = protocolRect.left + GetColumnWidth(2);
	col_left += protocolRect.Width();
	protocolRect.DeflateRect(8, 0);

	dc.DrawText(GetItemText(row, 2), protocolRect, DT_CENTER);

	CRect ipRect = rect;
	ipRect.left = col_left;
	ipRect.right = ipRect.left + GetColumnWidth(3);
	col_left += ipRect.Width();
	ipRect.DeflateRect(8, 0);

	dc.DrawText(GetItemText(row, 3), ipRect, DT_CENTER);

	CRect protRect = rect;
	protRect.left = col_left;
	protRect.right = protRect.left + GetColumnWidth(4);
	col_left += protRect.Width();
	protRect.DeflateRect(8, 0);

	dc.DrawText(GetItemText(row, 4), protRect, DT_CENTER);

	CRect deleteRect;
	GetSubItemRect(row, 5, LVIR_BOUNDS, deleteRect);
	CString s2 = GetItemText(row, 5);
	bool push = true;
	if (s2 == "1")
	{
		push = true;
	}
	else
	{
		push = false;
	}
	this->drawDeleteBtn(dc, deleteRect, push);

	dc.Detach();
}

void CWFPListCtrl::drawDeleteBtn(CDC& dc, const CRect& rect, bool push)
{
	int _size = 16;
	CRect btnRect = rect;
	if (push == true)
	{
		btnRect.DeflateRect(4, 2);
	}
	else
	{
		btnRect.DeflateRect(5, 3);
	}

	dc.DrawFrameControl(
		btnRect,
		DFC_BUTTON,
		DFCS_BUTTONPUSH
	);
	//btnRect.left = rect.left + 
}

void CWFPListCtrl::AddBlockItem(const wfpFileInfo info)
{
	const int row = GetItemCount();
	InsertItem(row, info.filePath);

	CString directionType;
	CString protocolType;

	if (info._DirectionType == DirectionType::both)
	{
		directionType = "both";
	}
	else if (info._DirectionType == DirectionType::in)
	{
		directionType = "in";
	}
	else if (info._DirectionType == DirectionType::out)
	{
		directionType = "out";
	}

	if (info._protocolType == protocolType::all)
	{
		protocolType = "all";
	}
	else if (info._protocolType == protocolType::TCP)
	{
		protocolType = "TCP";
	}
	else if (info._protocolType == protocolType::UDP)
	{
		protocolType = "UDP";
	}

	this->SetItemText(row, 1, directionType);
	this->SetItemText(row, 2, protocolType);
	this->SetItemText(row, 3, info.ip);
	this->SetItemText(row, 4, info.port);


}

void CWFPListCtrl::DeleteBlockItem(const CString& filePath)
{
	int cnt = GetItemCount();

	for (int i = 0; i < cnt; i++)
	{
		CString tmp = GetItemText(i, 0);

		if (tmp == filePath)
		{
			this->DeleteItem(i);
			return;
		}
	}

	return;
}

void CWFPListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{

	LVHITTESTINFO hitInfo{};
	hitInfo.pt = point;

	this->SubItemHitTest(&hitInfo);
	int row = hitInfo.iItem;
	int column = hitInfo.iSubItem;

	if (row < 0)
	{
		return;
	}

	// checkBox
	if (column == 1)
	{

	}
	else if (column == 5)
	{
		CRect deleteRect = this->get_DeleteRect(row);

		if (deleteRect.PtInRect(point))
		{
			this->SetItemText(row, 5, _T("0"));
			this->SetCapture();
			RedrawItems(row, row);
		}
		return;
	}
	CListCtrl::OnLButtonDown(nFlags, point);
}

CRect CWFPListCtrl::get_DeleteRect(int row)
{
	CRect rect;
	this->GetSubItemRect(
		row,
		5,
		LVIR_BOUNDS,
		rect
	);
	rect.DeflateRect(4, 2);
	return rect;
}

void CWFPListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (::GetCapture() == GetSafeHwnd())
	{
		::ReleaseCapture();
	}

	LVHITTESTINFO hitInfo{};
	hitInfo.pt = point;

	this->SubItemHitTest(&hitInfo);
	int row = hitInfo.iItem;
	int column = hitInfo.iSubItem;

	if (row < 0)
	{
		return;
	}

	if (column == 5)
	{
		this->SetItemText(row, 5, _T("1"));
		RedrawItems(row, row);

		CString filePath = this->GetItemText(row, 0);

		::SendMessage(this->h_View, WM_CLICKED_FROM_CWFPLISTCTRL,
			(WPARAM)clickedType::wfp_DeleteBtn, (LPARAM)filePath.GetString());
		return;
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

