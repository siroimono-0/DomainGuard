#pragma once

#include <Windows.h>
#include "../SharedIoctl/SharedIoctl.h"

class DomainBlockModel;

class CalloutDriverClient
{
public:
	CalloutDriverClient(DomainBlockModel* p_DomainBlock_Model);
	~CalloutDriverClient();
	void open();
	void close();
	void add_Domain();
	void init_Post();
	void insert_Domain(blockDomain _blockDomain);
	void update_Domain(blockDomain _blockDomain);
	void remove_Domain(blockDomain _blockDomain);

	void test_Driver_Q_TEXT(); 
	ARR_DRIVERLOG get_DriverLog();

private:
	HANDLE h_CalloutDriver = INVALID_HANDLE_VALUE;

	DomainBlockModel* p_DomainBlock_Model;
	//vecBlockDomain vec_BlockDomain;
};

