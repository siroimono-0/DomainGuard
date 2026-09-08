#include "pch.h"
#include "AppContext.h"
#include "./Controller/Controller.h"
#include "./Service/SQLITE.h"
#include "./Service/ResultQ_SQLITE.h"
#include "DomainGuard2View.h"
#include "DomainGuard2Doc.h"
#include "./Model/DomainBlockModel.h"
#include "./Service/DNS/DNS.h"
#include "./Model/DNSLogItemModel.h"
#include "./Service/WFP/WFP.h"
#include "./Model/WFPModel.h"
#include "./Service/CalloutDriverClient/CalloutDriverClient.h"
#include "./Model/DriverLogModel.h"

AppContext::AppContext(CDomainGuard2Doc* p_Doc, CDomainGuard2View* p_View)
{
	this->p_Doc = p_Doc;
	this->p_View = p_View;
	this->assembly();
	this->start_Timer();

	return;
}

AppContext::~AppContext()
{
	return;
}

void AppContext::assembly()
{
	HWND handle_View = this->p_View->GetSafeHwnd();

	this->p_View->set_blockList_HWND();
	this->p_View->set_WFPListCtrl_HWND();

	this->p_Doc->set_handle_View(handle_View);
	this->p_DomainBlockModel = new DomainBlockModel();
	this->p_DomainBlockModel->set_handle_View(handle_View);
	this->p_Doc->set_DomainBlockModel(this->p_DomainBlockModel);

	this->p_DNSLogItemModel = new DNSLogItemModel(handle_View);
	this->p_Doc->set_p_DNSLogItemModel(this->p_DNSLogItemModel);

	this->p_WFPModel = new WFPModel(handle_View);
	this->p_Doc->set_p_WFPModel(this->p_WFPModel);

	this->p_DriverLogModel = new DriverLogModel();
	this->p_Doc->set_p_DriverLogModel(this->p_DriverLogModel);

	this->p_ResultQ_SQLITE = new ResultQ_SQLITE();
	this->p_Controller = new Controller();

	this->p_CalloutDriver = new CalloutDriverClient(this->p_DomainBlockModel);
	//this->p_SQLITE->set_handle_View(handle_View);
	this->p_DNS = new DNS(this->p_DomainBlockModel);
	this->p_WFP = new WFP(this->p_WFPModel);
	this->p_SQLITE = new SQLITE(this->p_ResultQ_SQLITE, handle_View);

	this->p_Controller->set_p_Doc(this->p_Doc);
	this->p_Controller->set_p_SQLITE(this->p_SQLITE);
	this->p_Controller->set_p_DNS(this->p_DNS);
	this->p_Controller->set_p_WFP(this->p_WFP); 
	this->p_Controller->set_p_CalloutDriver(this->p_CalloutDriver);

	this->p_View->set_p_Controller(this->p_Controller);
	return;
}

void AppContext::start_Timer()
{
	this->p_View->SetTimer(TIMER_3SEC, 3000, nullptr);
	this->p_View->SetTimer(TIMER_3SEC_DRIVERLOG, 3000, nullptr);
	return;
}
