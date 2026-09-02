// Dia_WFP.cpp: 구현 파일
//

#include "pch.h"
#include "DomainGuard2.h"
#include "afxdialogex.h"
#include "Dia_WFP.h"


// Dia_WFP 대화 상자

IMPLEMENT_DYNAMIC(Dia_WFP, CDialogEx)

Dia_WFP::Dia_WFP(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WFP, pParent)
	, filePath(_T(""))
	, edit_Val_Text(_T(""))
	, edit_Port_Val_Text(_T(""))
{
	this->st_wfpFileInfo._DirectionType = DirectionType::both;
	this->st_wfpFileInfo._protocolType = protocolType::all;
	this->st_wfpFileInfo.ip = _T("ALL");
	this->st_wfpFileInfo.port = _T("ALL");
}

Dia_WFP::~Dia_WFP()
{
}

void Dia_WFP::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FILE_TEXT, filePath);
	DDX_Control(pDX, IDC_COMBO_DIRECTION, comboBox_Direction);
	DDX_Control(pDX, IDC_COMBO_PROTOCOL, comboBox_Protocol);
	DDX_Control(pDX, IDC_COMBO_IP, comboBox_IP);
	DDX_Control(pDX, IDC_EDIT_IP, edit_IP);
	DDX_Text(pDX, IDC_EDIT_IP, edit_Val_Text);
	DDX_Control(pDX, IDC_COMBO_PORT, comboBox_Port);
	DDX_Control(pDX, IDC_EDIT_PORT, edit_Port);
	DDX_Text(pDX, IDC_EDIT_PORT, edit_Port_Val_Text);
}


BEGIN_MESSAGE_MAP(Dia_WFP, CDialogEx)
	ON_BN_CLICKED(IDC_FIND_FILE, &Dia_WFP::OnClickedFindFile)
	ON_STN_CLICKED(IDC_STATIC_DIRECTION, &Dia_WFP::OnStnClickedStaticDirection)
	ON_CBN_SELCHANGE(IDC_COMBO_IP, &Dia_WFP::OnSelchangeComboIp)
	ON_CBN_SELCHANGE(IDC_COMBO_PORT, &Dia_WFP::OnSelchangeComboPort)
	ON_BN_CLICKED(IDOK, &Dia_WFP::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_DIRECTION, &Dia_WFP::OnSelchangeComboDirection)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCOL, &Dia_WFP::OnSelchangeComboProtocol)
END_MESSAGE_MAP()


// Dia_WFP 메시지 처리기

void Dia_WFP::OnClickedFindFile()
{
	CFileDialog dia(
		TRUE, _T("exe"), nullptr, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		_T("실행 파일 (*.exe)|*.exe||"), this
	);

	if (dia.DoModal() != IDOK)
	{
		return;
	}

	this->filePath = dia.GetPathName();
	this->st_wfpFileInfo.filePath = dia.GetPathName();

	this->UpdateData(FALSE);

	this->comboBox_Direction.EnableWindow(TRUE);
	this->comboBox_Protocol.EnableWindow(TRUE);
	this->comboBox_IP.EnableWindow(TRUE);
	this->comboBox_Port.EnableWindow(TRUE);
	return;
}

void Dia_WFP::OnStnClickedStaticDirection()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

BOOL Dia_WFP::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	this->comboBox_Direction.AddString(_T("양방향"));
	this->comboBox_Direction.AddString(_T("수신"));
	this->comboBox_Direction.AddString(_T("외부"));
	this->comboBox_Direction.SetCurSel(0);

	this->comboBox_Protocol.AddString(_T("전체"));
	this->comboBox_Protocol.AddString(_T("TCP"));
	this->comboBox_Protocol.AddString(_T("UDP"));
	this->comboBox_Protocol.SetCurSel(0);

	this->comboBox_IP.AddString(_T("전체"));
	this->comboBox_IP.AddString(_T("단일"));
	this->comboBox_IP.SetCurSel(0);
	this->edit_IP.SetReadOnly(TRUE);

	this->comboBox_Port.AddString(_T("전체"));
	this->comboBox_Port.AddString(_T("단일"));
	this->comboBox_Port.SetCurSel(0);
	this->edit_Port.SetReadOnly(TRUE);

	this->comboBox_Direction.EnableWindow(FALSE);
	this->comboBox_Protocol.EnableWindow(FALSE);
	this->comboBox_IP.EnableWindow(FALSE);
	this->comboBox_Port.EnableWindow(FALSE);

	return TRUE;
}

void Dia_WFP::OnSelchangeComboIp()
{
	int idx = this->comboBox_IP.GetCurSel();

	if (idx == 0)
	{
		this->edit_IP.SetReadOnly(TRUE);
		this->edit_Val_Text = _T("");
		this->UpdateData(FALSE);

		this->st_wfpFileInfo.ip = _T("ALL");
	}
	else
	{
		this->edit_IP.SetReadOnly(FALSE);
		this->st_wfpFileInfo.ip = _T("");
	}
}

void Dia_WFP::OnSelchangeComboPort()
{
	int idx = this->comboBox_Port.GetCurSel();

	if (idx == 0)
	{
		this->edit_Port.SetReadOnly(TRUE);
		this->edit_Port_Val_Text = _T("");
		this->UpdateData(FALSE);

		this->st_wfpFileInfo.port = _T("ALL");
	}
	else
	{
		this->edit_Port.SetReadOnly(FALSE);
		this->st_wfpFileInfo.port = _T("");
	}
}






void Dia_WFP::OnBnClickedOk()
{
	this->UpdateData(TRUE);

	if (this->st_wfpFileInfo.ip.IsEmpty())
	{
		this->st_wfpFileInfo.ip = this->edit_Val_Text;
	}

	if (this->st_wfpFileInfo.port.IsEmpty())
	{
		this->st_wfpFileInfo.port = this->edit_Port_Val_Text;
	}

	//this->st_wfpFileInfo._DirectionType = 
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void Dia_WFP::OnSelchangeComboDirection()
{
	int idx = this->comboBox_Direction.GetCurSel();

	if (idx == 0)
	{
		this->st_wfpFileInfo._DirectionType = DirectionType::both;
	}
	else if (idx == 1)
	{
		this->st_wfpFileInfo._DirectionType = DirectionType::in;
	}
	else if (idx == 2)
	{
		this->st_wfpFileInfo._DirectionType = DirectionType::out;
	}
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



void Dia_WFP::OnSelchangeComboProtocol()
{
	int idx = this->comboBox_Protocol.GetCurSel();
	if (idx == 0)
	{
		this->st_wfpFileInfo._protocolType = protocolType::all;
	}
	else if (idx == 1)
	{
		this->st_wfpFileInfo._protocolType = protocolType::TCP;
	}
	else if (idx == 2)
	{
		this->st_wfpFileInfo._protocolType = protocolType::UDP;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

wfpFileInfo Dia_WFP::get_st_wfpFileInfo()
{
	return this->st_wfpFileInfo;
}
