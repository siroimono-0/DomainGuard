#pragma once
#include "afxdialogex.h"


// Dia_INSERT 대화 상자

class Dia_INSERT : public CDialogEx
{
	DECLARE_DYNAMIC(Dia_INSERT)

public:
	Dia_INSERT(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~Dia_INSERT();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INSERT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString editText;
	afx_msg void OnClickedButtonOk();
};
