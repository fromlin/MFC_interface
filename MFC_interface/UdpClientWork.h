//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: UdpClientWork.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _UDPCLIENTWORK_H__
#define _UDPCLIENTWORK_H__

//#define PC_INSIDE

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Eigen/Dense>

#pragma comment (lib,"ws2_32.lib")
#pragma warning(disable:4996) 

#define IP_KUKA			"172.31.1.147"
#define IP_PC			"172.31.1.179"	// "192.170.10.3"
#define CLIENT_PORT			30010
#define SERVER_PORT			30000

#define IP_PROCCES		 "127.0.0.1"
#define PROCESS_IN			7009
#define PROCESS_OUT			9007

#define BUFFER_SIZE			1024
#define DATA_NUM			13			//6,7

using namespace Eigen;

class CUdpClientWork : public CWorkBase{

public:
	CUdpClientWork(std::string name);
	virtual ~CUdpClientWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_omega;
	std::string _memname_kuka;
	std::string _memname_state;
	OmegaData_t _omega;
	KuKaData_t _kuka;
	TouchData_t _touch;
	CanData_t _can;

	WSADATA wsaData;
	SOCKET m_clientSock;
	SOCKADDR_IN m_ToServer;
	SOCKADDR_IN m_FromServer;

	bool _isOpen;
	int  udp_state;
	int  omega_state;

	char _recvBuf[BUFFER_SIZE];
	char _sendBuf[BUFFER_SIZE];
	double _recvPacket[DATA_NUM];

	double kt[7] = { 0.0, 1.2, 0.9, 1.2, 0.9, 1.0, 0.0 };
	double tmp[7] = { 0.0, 2.2525, 0.0751, -0.8967, 0.0337, 0.0038, 0.0 };
	double cosj[7], sinj[7];
	Matrix<double, 7, 1> _torq;
	Matrix<double, 12, 1>  _pc;
	Matrix<double, 7, 12>  _Ob;
};

#endif