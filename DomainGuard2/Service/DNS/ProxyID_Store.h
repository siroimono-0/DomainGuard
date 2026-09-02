#pragma once
#include <pch.h>
#include <map>
#include <unordered_map>

class ProxyID_Store
{
public:
	ProxyID_Store();
	void emplace_map(uint16_t proxyId, pendingContext context);
	bool find_map(uint16_t proxyId);
	bool get_pendingContext(uint16_t proxyId, pendingContext& context);
	void erase_map(uint16_t proxyId);

	void emplace_un_mp(uint16_t proxyID, DNSLogItem item);
	void update_un_mp(uint16_t  proxyID, ULONGLONG responseTick);
	void clear_un_mp();

	void emplace_un_map_pending(uint16_t proxyID, DNSLogItem item);

	un_map_DNSLogItem get_un_map();

private:
	std::map<uint16_t, pendingContext> map;
	CCriticalSection cs;

	std::unordered_map<uint16_t, DNSLogItem> un_map;
	CCriticalSection cs_un_map;

	std::unordered_map<uint16_t, DNSLogItem> un_map_pending;
};

