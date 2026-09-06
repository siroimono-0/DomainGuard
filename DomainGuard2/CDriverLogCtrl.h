#pragma once

class CDriverLogCtrl : public CListCtrl
{

public:
    CDriverLogCtrl();
    ~CDriverLogCtrl();
    void AddBlockItem(const DriverLogItem item, const bool block);
    void DeleteBlockItem(const CString& domain);

    void set_h_View(HWND set);

    void update_DomainLogList(un_map_DNSLogItem un_map);

protected: // ¿Á¡§¿«
    virtual void PreSubclassWindow();
    //virtual void DrawItem(LPDRAWITEMSTRUCT lpDraw);

protected:

    DECLARE_MESSAGE_MAP()
public:

private:
    HWND h_View = NULL; 
};

