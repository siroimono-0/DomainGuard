// Dia_INSERT.cpp: 구현 파일
//

#include "pch.h"
#include "DomainGuard2.h"
#include "afxdialogex.h"
#include "Dia_INSERT.h"


// Dia_INSERT 대화 상자

IMPLEMENT_DYNAMIC(Dia_INSERT, CDialogEx)

Dia_INSERT::Dia_INSERT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INSERT, pParent)
	, editText(_T(""))
{

}

Dia_INSERT::~Dia_INSERT()
{
}

void Dia_INSERT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DOMAIN, editText);
}


BEGIN_MESSAGE_MAP(Dia_INSERT, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OK, &Dia_INSERT::OnClickedButtonOk)
END_MESSAGE_MAP()


// Dia_INSERT 메시지 처리기

void Dia_INSERT::OnClickedButtonOk()
{
	UpdateData(TRUE);
	OnOK();
}
