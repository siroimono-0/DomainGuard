
// DomainGuard2Doc.h: CDomainGuard2Doc 클래스의 인터페이스
//

#include "../SharedIoctl/SharedIoctl.h"

#pragma once
class DomainBlockModel;
class DNSLogItemModel;
class WFPModel;
class DriverLogModel;

class CDomainGuard2Doc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CDomainGuard2Doc() noexcept;
	DECLARE_DYNCREATE(CDomainGuard2Doc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CDomainGuard2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	void set_handle_View(HWND handle);
	void set_DomainBlockModel(DomainBlockModel* set);

	void set_BlocDomain_To_Model(vecBlockDomain set);
	vecBlockDomain get_vec_BlockDomain_To_Model();

	void update_To_Model(const blockDomain bkDomain);

	void insert_To_Model(const blockDomain bkDomain);

	void delete_To_Model(const blockDomain bkDomain);

	// =================================================
	// =================================================

	void set_p_DNSLogItemModel (DNSLogItemModel* set);
	void update_To_DNSLogItemModel(un_map_DNSLogItem item);
	un_map_DNSLogItem get_un_map_DNSLogItem();

	// =================================================
	// =================================================

	void set_p_WFPModel(WFPModel* set);
	void insert_To_WFPModel(const wfpFileInfo info);
	vecWfpFileInfo get_vec_Info_To_WFPModel();

	void delete_To_WFPModel(const wfpFileInfo info);

	void set_SelectInit_To_WFPModel(vecWfpFileInfo vec_Info);

	// =================================================
	// =================================================

	void set_p_DriverLogModel(DriverLogModel* p_DriverLogModel);
	void insert_To_DriverLogModel(ARR_DRIVERLOG arr);

private:
	HWND handle_View = NULL;
	DomainBlockModel* p_DomainBlockModel = nullptr;
	DNSLogItemModel* p_DNSLogItemModel = nullptr;
	WFPModel* p_WFPModel = nullptr;
	DriverLogModel* p_DriverLogModel = nullptr;

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
