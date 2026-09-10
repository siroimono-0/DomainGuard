
// DomainGuard2View.cpp: CDomainGuard2View 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "DomainGuard2.h"
#endif

#include "DomainGuard2Doc.h"
#include "DomainGuard2View.h"
#include "./Controller/Controller.h"
#include "Dia_INSERT.h"
#include "CBlockListCtrl.h"
#include "CDomainLogListCtrl.h"
#include "Dia_WFP.h"
#include "CWFPListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDomainGuard2View

IMPLEMENT_DYNCREATE(CDomainGuard2View, CFormView)

BEGIN_MESSAGE_MAP(CDomainGuard2View, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_COMMAND(ID_DOMAIN_INSERT, &CDomainGuard2View::OnDomainInsert)

	ON_MESSAGE(WM_QFINISHED_FROM_WK_SQLITE,
		&CDomainGuard2View::On_SELECT_INIT_FROM_WK_SQLITE)

	ON_MESSAGE(WM_CLICKED_FROM_CBLOCKLISTCTRL,
		&CDomainGuard2View::On_Clicked_FROM_CBlockListCtrl)

	ON_MESSAGE(WM_CLICKED_FROM_CWFPLISTCTRL,
		&CDomainGuard2View::On_Clicked_FROM_CWFPListCtrl)

	ON_MESSAGE(WM_DELETE_WK_DNS_FROM_WK_DNS,
		&CDomainGuard2View::On_WM_DELETE_WK_DNS_FROM_WK_DNS)

	ON_MESSAGE(WM_DELETE_WK_GQCS_DNS_FROM_WK_GQCS_DNS,
		&CDomainGuard2View::On_WM_DELETE_WK_GQCS_DNS_FROM_WK_GQCS_DNS)

	//ON_MESSAGE(WM_DELETE_FROM_CBLOCKLISTCTRL,
		//&CDomainGuard2View::On_SELECT_INIT_FROM_WK_SQLITE)
	ON_WM_TIMER()
	ON_COMMAND(ID_WFP_ADD, &CDomainGuard2View::OnWfpAdd)
	ON_COMMAND(ID_TEST_TEST, &CDomainGuard2View::OnTestTest)
END_MESSAGE_MAP()

// CDomainGuard2View 생성/소멸

CDomainGuard2View::CDomainGuard2View() noexcept
	: CFormView(IDD_DOMAINGUARD2_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CDomainGuard2View::~CDomainGuard2View()
{
}

void CDomainGuard2View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DOMAIN_BLOCK, blockList);
	//  DDX_Control(pDX, IDC_LIST_DOMAIN_LOG, domainLog_List);
	DDX_Control(pDX, IDC_LIST_DOMAIN_LOG, domainLogListCtrl);
	DDX_Control(pDX, IDC_LIST_WFP, WFPListCtrl);
	DDX_Control(pDX, IDC_LIST_DriverLog, driverLogCtrl);
}

BOOL CDomainGuard2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CDomainGuard2View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// CDomainGuard2View 인쇄

BOOL CDomainGuard2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CDomainGuard2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CDomainGuard2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CDomainGuard2View::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}


// CDomainGuard2View 진단

#ifdef _DEBUG
void CDomainGuard2View::AssertValid() const
{
	CFormView::AssertValid();
}

void CDomainGuard2View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDomainGuard2Doc* CDomainGuard2View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDomainGuard2Doc)));
	return (CDomainGuard2Doc*)m_pDocument;
}
#endif //_DEBUG


// CDomainGuard2View 메시지 처리기

void CDomainGuard2View::OnDomainInsert()
{
	Dia_INSERT dia;
	dia.DoModal();
	if (dia.editText == "")
	{
		return;
	}
	this->p_Controller->insert_To_SQLITE(dia.editText);
}

void CDomainGuard2View::set_p_Controller(Controller* set)
{
	this->p_Controller = set;
}

LRESULT CDomainGuard2View::On_SELECT_INIT_FROM_WK_SQLITE(WPARAM wParam, LPARAM lParam)
{
	this->p_Controller->finished_ResultQ();
	// TODO: 메시지 처리 코드 작성
	return 0;
}
void CDomainGuard2View::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (lHint == HINT_UPDATE_VEC_BLOCK_DOMAIN)
	{
		vecBlockDomain data = this->p_Controller->get_vec_BlockDomain_To_Doc();
		for (auto& v : data)
		{
			this->blockList.AddBlockItem(v.domain, v.block);
		}
	}
	else if (lHint == HINT_INSERT_BLOCK_DOMAIN)
	{
		vecBlockDomain data = this->p_Controller->get_vec_BlockDomain_To_Doc();
		this->blockList.AddBlockItem(data.back().domain, data.back().block);
	}
	else if (lHint == HINT_DELETE_BLOCK_DOMAIN)
	{
		Hint_UpdateAllViews* p_hint = (Hint_UpdateAllViews*)pHint;
		this->blockList.DeleteBlockItem(p_hint->domain);
	}
	else if (lHint == HINT_UPDATE_DNSLOGITEMMODEL)
	{
		un_map_DNSLogItem data = this->p_Controller->get_un_map_DNSLogItem();
		this->domainLogListCtrl.update_DomainLogList(data);
	}
	else if (lHint == HINT_INSERT_WFPMODEL)
	{
		vecWfpFileInfo data = this->p_Controller->get_vec_Info_To_Doc();
		this->WFPListCtrl.AddBlockItem(data.back());
	}
	else if (lHint == HINT_DELETE_WFP_CTRL)
	{
		Hint_UpdateAllViews* p_hint = (Hint_UpdateAllViews*)pHint;
		this->WFPListCtrl.DeleteBlockItem(p_hint->filePath);
	}
	else if (lHint == HINT_SELECTINIT_WFPMODEL)
	{
		vecWfpFileInfo vec = this->p_Controller->get_vec_Info_To_Doc();
		for (auto& v : vec)
		{
			this->WFPListCtrl.AddBlockItem(v);
		}
	}
	else if (lHint == HINT_ADD_DRIVERLOGMODEL)
	{
		vecDriverLogItem vec = this->p_Controller->get_vecDriverLog_To_Doc();
		for (auto& v : vec)
		{
			this->driverLogCtrl.AddBlockItem(v);
		}
	}

	//this->WFPListCtrl.AddBlockItem(_T("123"), true);
	return;
}

LRESULT CDomainGuard2View::On_Clicked_FROM_CBlockListCtrl(WPARAM wParam, LPARAM lParam)
{
	clickedType cnvType = (clickedType)wParam;
	LPCWSTR filePath = (LPCWSTR)lParam;
	CString s_filePath = filePath;

	this->p_Controller->push_SQL_To_SQLITE(cnvType, s_filePath);
	return 0;
}

LRESULT CDomainGuard2View::On_Clicked_FROM_CWFPListCtrl(WPARAM wParam, LPARAM lParam)
{
	clickedType cnvType = (clickedType)wParam;
	LPCWSTR filePath = (LPCWSTR)lParam;
	CString s_filePath = filePath;

	this->p_Controller->push_SQL_To_SQLITE(cnvType, s_filePath);
	this->p_Controller->deleteBlockFilter_To_WFP(s_filePath);
	return 0;
}

LRESULT CDomainGuard2View::On_WM_DELETE_WK_DNS_FROM_WK_DNS(WPARAM wParam, LPARAM lParam)
{
	this->p_Controller->delete_WK_DNS_OK();
	return 0;
}

LRESULT CDomainGuard2View::On_WM_DELETE_WK_GQCS_DNS_FROM_WK_GQCS_DNS(WPARAM wParam, LPARAM lParam)
{
	this->p_Controller->delete_WK_GQCS_DNS_OK();
	return 0;
}

void CDomainGuard2View::set_blockList_HWND()
{
	this->blockList.set_h_View(this->GetSafeHwnd());
	return;
}

void CDomainGuard2View::set_WFPListCtrl_HWND()
{
	this->WFPListCtrl.set_h_View(this->GetSafeHwnd());
}

void CDomainGuard2View::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_3SEC)
	{
		this->p_Controller->update_DNSLogItem();
	}
	else if (nIDEvent == TIMER_3SEC_DRIVERLOG)
	{
		this->p_Controller->get_DriverLog_To_CalloutDriverClient();
	}

	CFormView::OnTimer(nIDEvent);
}

void CDomainGuard2View::OnWfpAdd()
{
	Dia_WFP dia;
	if (dia.DoModal() != IDOK)
	{
		return;
	}

	wfpFileInfo info;
	info = dia.get_st_wfpFileInfo();

	this->p_Controller->addFile_To_WFP(std::move(info));
	return;
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CDomainGuard2View::OnTestTest()
{
	this->p_Controller->test_Driver_Domain();
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}









