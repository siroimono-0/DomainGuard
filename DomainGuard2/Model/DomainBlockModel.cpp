#include "pch.h"
#include "DomainBlockModel.h"
#include "DomainGuard2View.h"
//#include ""

DomainBlockModel::DomainBlockModel()
{
	return;
}

void  DomainBlockModel::set_handle_View(HWND handle_View)
{
	this->handle_View = handle_View;
	return;
}

void DomainBlockModel::set_vecBlockDomain(vecBlockDomain set)
{
	CSingleLock lock(&this->cs, TRUE);
	this->vec_BlockDomain = set;
	return;
}

vecBlockDomain DomainBlockModel::get_vec_BlockDomain()
{
	CSingleLock lock(&this->cs, TRUE);
	return this->vec_BlockDomain;
}

bool DomainBlockModel::isBlocked(CString domain)
{
	CSingleLock lock(&this->cs, TRUE);

	for (auto& v : vec_BlockDomain)
	{
		if (v.domain == domain && v.block == true)
		{
			return true;
		}
	}

	int start = domain.Find(_T('.'));

	if (start == -1)
	{
		return false;
	}
	CString nextDomain = domain.Mid(start + 1);

	//CString domain_new = 

	return this->isBlocked(nextDomain);

}

void DomainBlockModel::update_bkDomain(const blockDomain bkDomain)
{
	for (auto& v : this->vec_BlockDomain)
	{
		if (v.domain == bkDomain.domain)
		{
			v.block = bkDomain.block;
			break;
		}
	}
	return;
}

void DomainBlockModel::delete_bkDomain(const blockDomain bkDomain)
{

	auto it = std::find_if(this->vec_BlockDomain.begin(), this->vec_BlockDomain.end(), [bkDomain]
	(blockDomain a) {
			return a.domain == bkDomain.domain;
		});

	this->vec_BlockDomain.erase(it);
	//this->vec_BlockDomain.erase()
	return;
}

void DomainBlockModel::insert_bkDomain(const blockDomain bkDomain)
{
	this->vec_BlockDomain.push_back(bkDomain);
	return;
}
