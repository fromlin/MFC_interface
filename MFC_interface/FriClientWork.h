//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: FriClientWork.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _FRICLIENTWORK_H__
#define _FRICLIENTWORK_H__

#ifdef  _M_IX86

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#include "LBRClient.h"
#include "friUdpConnection.h"
#include "friClientApplication.h"

#pragma comment (lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define IP_KUKA_FRI			"192.170.10.2"		// IP_PC_FRI : "192.170.10.3"
#define DEFAULT_PORT			30200


using namespace KUKA::FRI;

class CFriClientWork : public CWorkBase {

public:
	CFriClientWork(std::string name);
	virtual ~CFriClientWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_kuka;
	KuKaData_t _kuka;
	TouchData_t _touch;

	WSADATA wsaData;
	SOCKET m_clientSock;
	SOCKADDR_IN m_ToServer;
	SOCKADDR_IN m_FromServer;

	bool _isOpen;
};

#endif
#endif