
// DomainGuard2View.h: CDomainGuard2View 클래스의 인터페이스
//

#include "CBlockListCtrl.h"
#include "CDomainLogListCtrl.h"
#include "CWFPListCtrl.h"
#include "CDriverLogCtrl.h"

#pragma once
class Controller;
class CDomainGuard2Doc;

class CDomainGuard2View : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CDomainGuard2View() noexcept;
	DECLARE_DYNCREATE(CDomainGuard2View)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_DOMAINGUARD2_FORM };
#endif

// 특성입니다.
public:
	CDomainGuard2Doc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

// 구현입니다.
public:
	virtual ~CDomainGuard2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	void set_p_Controller(Controller* set);
	void set_blockList_HWND();
	void set_WFPListCtrl_HWND();
	LRESULT On_SELECT_INIT_FROM_WK_SQLITE(WPARAM wParam, LPARAM lParam);
	LRESULT On_Clicked_FROM_CBlockListCtrl(WPARAM wParam, LPARAM lParam);
	LRESULT On_Clicked_FROM_CWFPListCtrl(WPARAM wParam, LPARAM lParam);

private:
	Controller* p_Controller = nullptr;

public:
	afx_msg void OnDomainInsert();

	CBlockListCtrl blockList;
//	CListCtrl domainLog_List;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CDomainLogListCtrl domainLogListCtrl;
	afx_msg void OnWfpAdd();

	CWFPListCtrl WFPListCtrl;
	afx_msg void OnTestTest();
	CDriverLogCtrl driverLogCtrl;
};

#ifndef _DEBUG  // DomainGuard2View.cpp의 디버그 버전
inline CDomainGuard2Doc* CDomainGuard2View::GetDocument() const
   { return reinterpret_cast<CDomainGuard2Doc*>(m_pDocument); }
#endif

