#include "pch.h"
#include "ProxyID_Store.h"

ProxyID_Store::ProxyID_Store()
{

}

void ProxyID_Store::emplace_map(uint16_t proxyId, pendingContext context)
{
	CSingleLock lock(&this->cs, TRUE);
	map[proxyId] = context;
	return;
}

bool ProxyID_Store::find_map(uint16_t proxyId)
{
	CSingleLock lock(&this->cs, TRUE);
	if (this->map.find(proxyId) == this->map.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool ProxyID_Store::get_pendingContext(uint16_t proxyId, pendingContext& context)
{
	CSingleLock lock(&this->cs, TRUE);
	if (this->map.find(proxyId) == this->map.end())
	{
		return false;
	}
	context = this->map[proxyId];
	return true;
}

void ProxyID_Store::erase_map(uint16_t proxyId)
{
	CSingleLock lock(&this->cs, TRUE);
	this->map.erase(proxyId);
	return;
}

// =====================================================================
// =====================================================================

void  ProxyID_Store::emplace_un_mp(uint16_t proxyID, DNSLogItem item)
{
	CSingleLock lock(&this->cs_un_map, TRUE);
	this->un_map[proxyID] = item;
	return;
}

void ProxyID_Store::update_un_mp(uint16_t proxyID, ULONGLONG responseTick)
{
	CSingleLock lock(&this->cs_un_map, TRUE);
	ULONGLONG requestTick = this->un_map_pending[proxyID].requestTick;
	int ret = responseTick - requestTick;

	this->un_map[proxyID] = this->un_map_pending[proxyID];
	this->un_map[proxyID].responeseTime = ret;

	this->un_map_pending.erase(proxyID);
	return;
}
void ProxyID_Store::clear_un_mp()
{
	CSingleLock lock(&this->cs_un_map, TRUE);
	this->un_map.clear();
	return;
}

un_map_DNSLogItem ProxyID_Store::get_un_map()
{
	CSingleLock lock(&this->cs_un_map, TRUE);
	return std::move(this->un_map);
}

void ProxyID_Store::emplace_un_map_pending(uint16_t proxyID, DNSLogItem item)
{
	CSingleLock lock(&this->cs_un_map, TRUE);
	this->un_map_pending[proxyID] = item;
	return;
}











