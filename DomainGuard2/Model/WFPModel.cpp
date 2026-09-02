#include "pch.h"
#include "WFPModel.h"

WFPModel::WFPModel(HWND h_View)
{
	this->h_View = h_View;
	return;
}

WFPModel::~WFPModel()
{
	return;
}

void WFPModel::set_h_View(HWND set)
{
	this->h_View = set;
}

void WFPModel::insert_Info(const wfpFileInfo info)
{
	this->vec_Info.push_back(info);
	return;
}

vecWfpFileInfo  WFPModel::get_vec_Info()
{
	return this->vec_Info;
}

void WFPModel::delete_info(const wfpFileInfo info)
{
	for (auto it = this->vec_Info.begin(); it != this->vec_Info.end(); it++)
	{
		if (it->filePath == info.filePath)
		{
			vec_Info.erase(it);
			return;
		}
	}
	return;
}

void  WFPModel::selcetInit(vecWfpFileInfo vec_info)
{
	this->vec_Info = vec_info;
	return;
}





