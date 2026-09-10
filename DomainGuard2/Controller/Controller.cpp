#include "pch.h"
#include "Controller.h"
#include "../Service/SQLITE.h"
#include "../DomainGuard2Doc.h"
#include "../Service/DNS/DNS.h"
#include "../Service/WFP/WFP.h"
#include "../Service/CalloutDriverClient/CalloutDriverClient.h"
#include "../../SharedIoctl/SharedIoctl.h"

void  Controller::set_p_Doc(CDomainGuard2Doc* p_Doc)
{
	this->p_Doc = p_Doc;
	return;
}

void Controller::set_p_SQLITE(SQLITE* p_SQLITE)
{
	this->p_SQLITE = p_SQLITE;
	return;
}

void  Controller::set_p_DNS(DNS* p_DNS)
{
	this->p_DNS = p_DNS;
	return;
}

void Controller::insert_To_SQLITE(CString domain)
{
	sql_Q_Job job;
	job.domain = domain;
	job.block = true;
	job.type = sqlType::insert;

	this->p_SQLITE->push_WK_Q(job);
}

void Controller::finished_ResultQ()
{
	Q_Finished_Data data = this->p_SQLITE->tryPop_WK_Q();

	if (data.sql_Type == sqlType::select_Init)
	{
		this->p_Doc->set_BlocDomain_To_Model(std::move(data.vec_BlockDomain));
		this->p_CalloutDriver->init_Post();
	}
	else if (data.sql_Type == sqlType::select)
	{

	}
	else if (data.sql_Type == sqlType::insert)
	{
		this->p_Doc->insert_To_Model(data._blockDomain);
		this->p_CalloutDriver->insert_Domain(data._blockDomain);
	}
	else if (data.sql_Type == sqlType::update)
	{
		this->p_Doc->update_To_Model(data._blockDomain);
		this->p_CalloutDriver->update_Domain(data._blockDomain);
	}
	else if (data.sql_Type == sqlType::remove)
	{
		this->p_Doc->delete_To_Model(data._blockDomain);
		this->p_CalloutDriver->remove_Domain(data._blockDomain);
	}
	else if (data.sql_Type == sqlType::wfpInsert)
	{
		this->p_Doc->insert_To_WFPModel(data.info);
	}
	else if (data.sql_Type == sqlType::wfpDelete)
	{
		this->p_Doc->delete_To_WFPModel(data.info);
	}
	else if (data.sql_Type == sqlType::wfpSelect_Init)
	{
		for (auto& v : data.vec_Info)
		{
			this->p_WFP->addFile(v);
		}
		this->p_Doc->set_SelectInit_To_WFPModel(std::move(data.vec_Info));

	}
	else if (data.sql_Type == sqlType::WK_delete)
	{
		this->p_SQLITE->delete_WK();
		this->isDelete_p_SQLITE_WK = true;
		this->close();
	}

	return;
}

vecBlockDomain Controller::get_vec_BlockDomain_To_Doc()
{
	return std::move(this->p_Doc->get_vec_BlockDomain_To_Model());
}

void Controller::push_SQL_To_SQLITE(clickedType type, CString domain)
{
	sql_Q_Job job;

	if (type == clickedType::checkBox_on)
	{
		job.type = sqlType::update;
		job.block = true;
		job.domain = domain;
		this->p_SQLITE->push_WK_Q(job);
	}
	else if (type == clickedType::checkBox_off)
	{
		job.type = sqlType::update;
		job.block = false;
		job.domain = domain;
		this->p_SQLITE->push_WK_Q(job);
	}
	else if (type == clickedType::deleteBtn)
	{
		job.type = sqlType::remove;
		job.domain = domain;
		this->p_SQLITE->push_WK_Q(job);
	}
	else if (type == clickedType::wfp_DeleteBtn)
	{
		wfpFileInfo info;
		info.filePath = domain;
		job.type = sqlType::wfpDelete;
		job.wfpInfo = info;
		this->p_SQLITE->push_WK_Q(job);
	}
}

void Controller::update_DNSLogItem()
{
	auto un_map_DNSLogItem = this->p_DNS->get_un_map_DNSLogItem();
	this->p_Doc->update_To_DNSLogItemModel(std::move(un_map_DNSLogItem));
	return;
}

un_map_DNSLogItem Controller::get_un_map_DNSLogItem()
{
	return std::move(this->p_Doc->get_un_map_DNSLogItem());
}

void Controller::set_p_WFP(WFP* p_WFP)
{
	this->p_WFP = p_WFP;
	return;
}

void Controller::addFile_To_WFP(wfpFileInfo info)
{
	this->p_WFP->addFile(info);
	this->wfp_Insert_To_SQLITE(info);
	return;
}

void Controller::wfp_Insert_To_SQLITE(wfpFileInfo info)
{
	sql_Q_Job job;
	job.type = sqlType::wfpInsert;
	job.wfpInfo = info;
	this->p_SQLITE->push_WK_Q(job);
}

vecWfpFileInfo Controller::get_vec_Info_To_Doc()
{
	return this->p_Doc->get_vec_Info_To_WFPModel();
}

void Controller::deleteBlockFilter_To_WFP(const CString filePath)
{
	this->p_WFP->deleteBlockFilter(filePath);
	return;
}

void  Controller::set_p_CalloutDriver(CalloutDriverClient* p_CalloutDriver)
{
	this->p_CalloutDriver = p_CalloutDriver;
	return;
}

void Controller::test_Driver_Domain()
{
	this->p_CalloutDriver->test_Driver_Q_TEXT();
	return;
}

void Controller::get_DriverLog_To_CalloutDriverClient()
{
	ARR_DRIVERLOG arrDriverLog = this->p_CalloutDriver->get_DriverLog();
	this->p_Doc->insert_To_DriverLogModel(arrDriverLog);
}

vecDriverLogItem Controller::get_vecDriverLog_To_Doc()
{
	return this->p_Doc->get_vecDriverLog_To_DriverLogModel();
}

void Controller::delete_WK_DNS_OK()
{
	this->isDelete_p_DNS_WK = true;
	if (this->isDelete_p_DNS_WK == true &&
		this->isDelete_p_DNS_GQCS_WK == true)
	{
		this->delete_p_WK__p_WK_GQCS();
	}
	return;
}

void  Controller::delete_WK_GQCS_DNS_OK()
{
	this->isDelete_p_DNS_GQCS_WK = true;
	if (this->isDelete_p_DNS_WK == true &&
		this->isDelete_p_DNS_GQCS_WK == true)
	{
		this->delete_p_WK__p_WK_GQCS();
	}
	return;
}

void Controller::delete_p_WK__p_WK_GQCS()
{
	this->p_DNS->delete_p_WK__p_WK_GQCS();
	this->close();
	return;
}

void  Controller::set_h_MainFrame(HWND h_MainFrame)
{
	this->h_MainFrame = h_MainFrame;
	return;
}

void Controller::close()
{
	if (this->isDelete_p_SQLITE_WK == false)
	{
		this->close_SQLITE();
	}

	if (this->isDelete_p_DNS_WK == false &&
		this->isDelete_p_DNS_GQCS_WK == false)
	{
		this->close_DNS();
	}

	if (this->isDelete_p_SQLITE_WK == true &&
		this->isDelete_p_DNS_WK == true &&
		this->isDelete_p_DNS_GQCS_WK == true)
	{
		::PostMessage(this->h_MainFrame, WM_FINISHED_WK_DELETE, NULL, NULL);
	}

	return;
}

void Controller::close_SQLITE()
{
	sql_Q_Job job;
	job.type = sqlType::close;
	this->p_SQLITE->push_WK_Q(job);
	return;
}

void  Controller::close_DNS()
{
	this->p_DNS->close();
	return;
}



