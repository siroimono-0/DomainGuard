
// DomainGuard2Doc.cpp: CDomainGuard2Doc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "DomainGuard2.h"
#endif

#include "DomainGuard2Doc.h"
#include <propkey.h>
#include "./Model/DomainBlockModel.h"
#include "./Model/DNSLogItemModel.h"
#include "./Model/WFPModel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDomainGuard2Doc

IMPLEMENT_DYNCREATE(CDomainGuard2Doc, CDocument)

BEGIN_MESSAGE_MAP(CDomainGuard2Doc, CDocument)
END_MESSAGE_MAP()


// CDomainGuard2Doc 생성/소멸

CDomainGuard2Doc::CDomainGuard2Doc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CDomainGuard2Doc::~CDomainGuard2Doc()
{
}

BOOL CDomainGuard2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CDomainGuard2Doc serialization

void CDomainGuard2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CDomainGuard2Doc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CDomainGuard2Doc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CDomainGuard2Doc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl* pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CDomainGuard2Doc 진단

#ifdef _DEBUG
void CDomainGuard2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDomainGuard2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CDomainGuard2Doc::set_handle_View(HWND handle)
{
	this->handle_View = handle;
	return;
}

void CDomainGuard2Doc::set_DomainBlockModel(DomainBlockModel* set)
{
	this->p_DomainBlockModel = set;
	return;
}

void CDomainGuard2Doc::set_BlocDomain_To_Model(vecBlockDomain set)
{
	this->p_DomainBlockModel->set_vecBlockDomain(set);
	this->UpdateAllViews(nullptr, HINT_UPDATE_VEC_BLOCK_DOMAIN);

	return;
}
vecBlockDomain CDomainGuard2Doc::get_vec_BlockDomain_To_Model()
{
	return std::move(this->p_DomainBlockModel->get_vec_BlockDomain());
}
// CDomainGuard2Doc 명령

void  CDomainGuard2Doc::update_To_Model(const blockDomain bkDomain)
{
	this->p_DomainBlockModel->update_bkDomain(bkDomain);
	return;
}

void CDomainGuard2Doc::insert_To_Model(const blockDomain bkDomain)
{
	this->p_DomainBlockModel->insert_bkDomain(bkDomain);
	this->UpdateAllViews(nullptr, HINT_INSERT_BLOCK_DOMAIN);
}

void CDomainGuard2Doc::delete_To_Model(const blockDomain bkDomain)
{
	this->p_DomainBlockModel->delete_bkDomain(bkDomain);

	Hint_UpdateAllViews hint;
	hint.domain = bkDomain.domain;
	this->UpdateAllViews(nullptr, HINT_DELETE_BLOCK_DOMAIN, (CObject*)&hint);
	// UpdateAllViews는 동기방식으로
	// -> OnUpdate -> 반환 
}

void CDomainGuard2Doc::set_p_DNSLogItemModel(DNSLogItemModel* set)
{
	this->p_DNSLogItemModel = set;
}

void CDomainGuard2Doc::update_To_DNSLogItemModel(un_map_DNSLogItem item)
{
	this->p_DNSLogItemModel->update_vec_DNSLogItem(std::move(item));
	this->UpdateAllViews(nullptr, HINT_UPDATE_DNSLOGITEMMODEL, nullptr);

	return;
}

un_map_DNSLogItem CDomainGuard2Doc::get_un_map_DNSLogItem()
{
	return std::move(this->p_DNSLogItemModel->get_un_map_DNSLogItem())
		;
}

void CDomainGuard2Doc::set_p_WFPModel(WFPModel* set)
{
	this->p_WFPModel = set;
}

void CDomainGuard2Doc::insert_To_WFPModel(const wfpFileInfo info)
{
	this->p_WFPModel->insert_Info(info);
	this->UpdateAllViews(nullptr, HINT_INSERT_WFPMODEL, nullptr);
	return;
}

vecWfpFileInfo CDomainGuard2Doc::get_vec_Info_To_WFPModel()
{
	return this->p_WFPModel->get_vec_Info();
}

void CDomainGuard2Doc::delete_To_WFPModel(const wfpFileInfo info)
{
	this->p_WFPModel->delete_info(info);

	Hint_UpdateAllViews hint;
	hint.filePath = info.filePath;
	this->UpdateAllViews(nullptr, HINT_DELETE_WFP_CTRL, (CObject*)&hint);
	// UpdateAllViews는 동기방식으로
	// -> OnUpdate -> 반환 
	return;
}

void CDomainGuard2Doc::set_SelectInit_To_WFPModel(vecWfpFileInfo vec_Info)
{
	this->p_WFPModel->selcetInit(vec_Info);
	this->UpdateAllViews(nullptr, HINT_SELECTINIT_WFPMODEL);
}

void CDomainGuard2Doc::set_p_DriverLogModel(DriverLogModel* p_DriverLogModel)
{
	this->p_DriverLogModel = p_DriverLogModel;
	return;
}

void CDomainGuard2Doc::insert_To_DriverLogModel(ARR_DRIVERLOG arr)
{

}








