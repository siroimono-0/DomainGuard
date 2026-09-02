#pragma once
#include "pch.h"
#include <vector>

class DomainBlockModel {

public:
	DomainBlockModel();
	void set_vecBlockDomain(vecBlockDomain set);
	void set_handle_View(HWND handle_View);
	vecBlockDomain get_vec_BlockDomain();

	bool isBlocked(CString domain);

	void update_bkDomain(const blockDomain bkDomain);
	void insert_bkDomain(const blockDomain bkDomain);
	void delete_bkDomain(const blockDomain bkDomain);

private:
	HWND handle_View = NULL;
	vecBlockDomain vec_BlockDomain;

	CCriticalSection cs;

	// std::vector<blockDomain>
};