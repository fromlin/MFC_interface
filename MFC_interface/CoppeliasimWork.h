#pragma once

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"

#include "RemoteAPIClient.h"

class CCoppeliasimWork : public CWorkBase {

public:
	CCoppeliasimWork(std::string name);
	virtual ~CCoppeliasimWork();
	 
	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_data;

public:
	RemoteAPIClient client;

	double t = 0.0;
};
