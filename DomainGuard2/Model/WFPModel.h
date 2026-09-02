#pragma once
#include <vector>

class WFPModel
{
public:
	
	WFPModel(HWND h_View);
	~WFPModel();
	void set_h_View(HWND set);

	void insert_Info(const wfpFileInfo info);
	void delete_info(const wfpFileInfo info);

	vecWfpFileInfo get_vec_Info();

	void selcetInit(vecWfpFileInfo vec_info);
private:
	HWND h_View = NULL;

	vecWfpFileInfo vec_Info;
	//std::vector<wfpFileInfo> vec_info;
};

