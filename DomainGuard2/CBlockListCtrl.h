#pragma once

class CBlockListCtrl : public CListCtrl
{
public:
    CBlockListCtrl();
    ~CBlockListCtrl();
    void AddBlockItem(const CString& domain, const bool block);
    void DeleteBlockItem(const CString& domain);
    
    void drawCheckBox(CDC& dc, const CRect& rect, bool block);
    void drawDeleteBtn(CDC& dc, const CRect& rect, bool push);

    CRect get_CheckBoxRect(int row);
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
