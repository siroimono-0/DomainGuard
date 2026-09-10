#include "pch.h"
#include "CBlockListCtrl.h"

CBlockListCtrl::CBlockListCtrl()
{
}

CBlockListCtrl::~CBlockListCtrl()
{
}

BEGIN_MESSAGE_MAP(CBlockListCtrl, CListCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void  CBlockListCtrl::set_h_View(HWND set)
{
	this->h_View = set;
}

void CBlockListCtrl::PreSubclassWindow()
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
		_T("Domain"),
		LVCFMT_CENTER, 250);
	this->InsertColumn(
		1,
		_T("Block"),
		LVCFMT_CENTER, 100);
	this->InsertColumn(
		2,
		_T("Delete"),
		LVCFMT_CENTER, 100);

	CRect client;
	GetClientRect(&client);
	//RedrawWindow();

	int domainWidth = client.Width() - GetColumnWidth(1) - GetColumnWidth(2);
	this->SetColumnWidth(0, domainWidth);
	return;
}

void CBlockListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDraw)
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
	domainRect.DeflateRect(8, 0);

	dc.DrawText(GetItemText(row, 0), domainRect, DT_CENTER);

	CRect checkRect;
	GetSubItemRect(row, 1, LVIR_BOUNDS, checkRect);
	CString s = GetItemText(row, 1);
	bool checkBlock = true;
	if (s == "1")
	{
		checkBlock = true;
	}
	else
	{
		checkBlock = false;
	}
	this->drawCheckBox(dc, checkRect, checkBlock);

	CRect deleteRect;
	GetSubItemRect(row, 2, LVIR_BOUNDS, deleteRect);
	CString s2 = GetItemText(row, 2);
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

void CBlockListCtrl::drawDeleteBtn(CDC& dc, const CRect& rect, bool push)
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

void CBlockListCtrl::drawCheckBox(CDC& dc, const CRect& rect, bool block)
{
	int _size = 16;

	CRect checkRect;
	checkRect.left = rect.left + (rect.Width() - _size) / 2;
	checkRect.top = rect.top + (rect.Height() - _size) / 2;
	checkRect.right = checkRect.left + _size;
	checkRect.bottom = checkRect.top + _size;

	UINT state = DFCS_BUTTONCHECK;
	if (block)
	{
		state |= DFCS_CHECKED;
	}

	dc.DrawFrameControl(
		checkRect,
		DFC_BUTTON,
		state
	);
}

void CBlockListCtrl::AddBlockItem(const CString& domain, const bool block)
{
	const int row = GetItemCount();
	InsertItem(row, domain);
	CString s;
	if (block == true)
	{
		s = "1";
	}
	else if (block == false)
	{
		s = "0";
	}

	this->SetItemText(row, 1, s);
	this->SetItemText(row, 2, _T("1"));

}

void CBlockListCtrl::DeleteBlockItem(const CString& domain)
{
	int cnt = GetItemCount();

	for (int i = 0; i < cnt; i++)
	{
		CString tmp = GetItemText(i, 0);

		if (tmp == domain)
		{
			this->DeleteItem(i);
			return;
		}
	}

	return;
}

void CBlockListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
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
		CRect checkRect = this->get_CheckBoxRect(row);

		if (checkRect.PtInRect(point))
		{
			CString tmp = this->GetItemText(row, 1);
			CString domain = this->GetItemText(row, 0);
			bool block = true;

			if (tmp == "1")
			{
				block = true;
			}
			else
			{
				block = false;
			}

			bool newBlock = !block;

			if (newBlock)
			{
				::SendMessage(this->h_View, WM_CLICKED_FROM_CBLOCKLISTCTRL,
					(WPARAM)clickedType::checkBox_on, (LPARAM)domain.GetString());
			}
			else
			{
				::SendMessage(this->h_View, WM_CLICKED_FROM_CBLOCKLISTCTRL,
					(WPARAM)clickedType::checkBox_off, (LPARAM)domain.GetString());
			}
			this->SetItemText(row, 1, newBlock ? _T("1") : _T("0"));
			this->RedrawItems(row, row);
		}
		return;
	}
	else if (column == 2)
	{
		CRect deleteRect = this->get_DeleteRect(row);

		if (deleteRect.PtInRect(point))
		{
			this->SetItemText(row, 2, _T("0"));
			this->SetCapture();
			RedrawItems(row, row);
		}
		return;
	}
	CListCtrl::OnLButtonDown(nFlags, point);
}

CRect CBlockListCtrl::get_CheckBoxRect(int row)
{
	CRect rect;
	this->GetSubItemRect(
		row,
		1,
		LVIR_BOUNDS,
		rect
	);

	int _size = 16;

	CRect checkRect;
	checkRect.left = rect.left + (rect.Width() - _size) / 2;
	checkRect.top = rect.top + (rect.Height() - _size) / 2;
	checkRect.right = checkRect.left + _size;
	checkRect.bottom = checkRect.top + _size;

	return checkRect;
}

CRect CBlockListCtrl::get_DeleteRect(int row)
{
	CRect rect;
	this->GetSubItemRect(
		row,
		2,
		LVIR_BOUNDS,
		rect
	);
	rect.DeflateRect(4, 2);
	return rect;
}

void CBlockListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
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

	if (column == 2)
	{
		this->SetItemText(row, 2, _T("1"));
		RedrawItems(row, row);

		CString domain = this->GetItemText(row, 0);

		::SendMessage(this->h_View, WM_CLICKED_FROM_CBLOCKLISTCTRL,
			(WPARAM)clickedType::deleteBtn, (LPARAM)domain.GetString());
		return;
	}

	CListCtrl::OnLButtonUp(nFlags, point);
}

