#pragma once
#include "afxdialogex.h"


// Dia_WFP 대화 상자

class Dia_WFP : public CDialogEx
{
	DECLARE_DYNAMIC(Dia_WFP)

public:
	Dia_WFP(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Dia_WFP();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WFP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog()  override;

public:

private:
	wfpFileInfo st_wfpFileInfo;

	DECLARE_MESSAGE_MAP()
public:
	wfpFileInfo get_st_wfpFileInfo();
	afx_msg void OnClickedFindFile();
	CString filePath;

	afx_msg void OnStnClickedStaticDirection();
	CComboBox comboBox_Direction;

	
	CComboBox comboBox_Protocol;
	CComboBox comboBox_IP;
	CEdit edit_IP;
	afx_msg void OnSelchangeComboIp();
	CString edit_Val_Text;
	CComboBox comboBox_Port;
	CEdit edit_Port;
	CString edit_Port_Val_Text;
	afx_msg void OnSelchangeComboPort();
	afx_msg void OnBnClickedOk();
	afx_msg void OnSelchangeComboDirection();
	afx_msg void OnSelchangeComboProtocol();
};
