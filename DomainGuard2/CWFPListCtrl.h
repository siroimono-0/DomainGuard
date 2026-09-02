#pragma once
#include "pch.h"

class CWFPListCtrl : public CListCtrl
{
public:
    CWFPListCtrl();
    ~CWFPListCtrl();
    void AddBlockItem(const wfpFileInfo info);
    void DeleteBlockItem(const CString& domain);
    
    //void drawCheckBox(CDC& dc, const CRect& rect, bool block);
    void drawDeleteBtn(CDC& dc, const CRect& rect, bool push);

    //CRect get_CheckBoxRect(int row);
    CRect get_DeleteRect(int row);

    void set_h_View(HWND set);

protected: // ¿Á¡§¿«
    virtual void PreSubclassWindow();
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDraw);

protected:

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

private:
    HWND h_View = NULL; 
};
