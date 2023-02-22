//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: UdpClientWork.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UdpClientWork.h"

CUdpClientWork::CUdpClientWork(std::string name)
    :CWorkBase(name)
{
    memset(&_omega, 0, sizeof(OmegaData_t));
    memset(&_kuka, 0, sizeof(KuKaData_t));

    _memname_omega = name + "_Omega";
    _memname_kuka = name + "_KuKa";
    _memname_state = name + "_State";
    CREATE_SYSTEM_MEMORY(_memname_omega, OmegaData_t);
    CREATE_SYSTEM_MEMORY(_memname_kuka, KuKaData_t);
    CREATE_SYSTEM_MEMORY(_memname_state, int);

    _pc << -0.2006, -0.1993, 0.2043, -0.0060, -0.0460, 0.0804, 0.0944, 0.0161, 0.0014, -0.0441, -0.0010, -0.0210;

    _isOpen = false;
}


CUdpClientWork::~CUdpClientWork() {

    DELETE_SYSTEM_MEMORY(_memname_omega);
    DELETE_SYSTEM_MEMORY(_memname_kuka);
    DELETE_SYSTEM_MEMORY(_memname_state);

    ClosePort();
}

bool CUdpClientWork::OpenPort() {

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) == SOCKET_ERROR) {
        std::cout << "WinSock initialize error !! " << std::endl;
        WSACleanup();
        exit(0);
    }
    m_clientSock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&m_ToServer, 0, sizeof(m_ToServer));
    memset(&m_FromServer, 0, sizeof(m_FromServer));

#ifdef PC_INSIDE
    m_ToServer.sin_family = AF_INET;
    m_ToServer.sin_addr.s_addr = inet_addr(IP_PROCCES);
    m_ToServer.sin_port = htons(PROCESS_OUT);

    m_FromServer.sin_family = AF_INET;
    m_FromServer.sin_addr.s_addr = inet_addr(IP_PROCCES);
    m_FromServer.sin_port = htons(PROCESS_IN);

    if (bind(m_clientSock, (struct sockaddr*)&m_FromServer, sizeof(m_FromServer)) == SOCKET_ERROR) {
        std::cerr << "Can't bind! " << std::endl;
        closesocket(m_clientSock);
        WSACleanup();
        exit(0);
    }
    else {
        _isOpen = true;
        sprintf_s(_sendBuf, "pose,%.04f,%.04f,%.04f", 140.8, 0.0, 0.0);
        sendto(m_clientSock, (char*)_sendBuf, BUFFER_SIZE, 0, (struct sockaddr*)&m_ToServer, sizeof(m_ToServer));
        return 1;
	}
#else
    m_ToServer.sin_family = AF_INET;
    m_ToServer.sin_addr.s_addr = inet_addr(IP_KUKA);
    m_ToServer.sin_port = htons(CLIENT_PORT);

    m_FromServer.sin_family = AF_INET;
    m_FromServer.sin_addr.s_addr = htonl(INADDR_ANY);
    m_FromServer.sin_port = htons(SERVER_PORT);

    if (bind(m_clientSock, (struct sockaddr*)&m_FromServer, sizeof(m_FromServer)) == SOCKET_ERROR) {
        std::cerr << "Can't bind! " << std::endl;
        closesocket(m_clientSock);
        WSACleanup();
        exit(0);
    }
    else {
        _isOpen = true;
        sprintf_s(_sendBuf, "%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%d,", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0);
        sendto(m_clientSock, (char*)_sendBuf, BUFFER_SIZE, 0, (struct sockaddr*)&m_ToServer, sizeof(m_ToServer));
        return 1;
    }
#endif
}


void CUdpClientWork::ClosePort() {

    _isOpen = udp_state = false;
    memset(&_kuka, 0, sizeof(KuKaData_t));
    SET_SYSTEM_MEMORY(_memname_kuka, _kuka);
    SET_SYSTEM_MEMORY(_memname_state, udp_state);
    closesocket(m_clientSock);
    WSACleanup();
}

void CUdpClientWork::_execute() {

    GET_SYSTEM_MEMORY("OmegaWork_State", omega_state);
    GET_SYSTEM_MEMORY("TouchWork_Data", _touch); 
    GET_SYSTEM_MEMORY("CanWork_Data", _can);
    //GET_SYSTEM_MEMORY("UdpClientWork_Omega", _omega);

    static int recvSize = 0, sendSize = 0;
    static int checkSize;

    if (_isOpen) {
#ifdef PC_INSIDE
        sprintf_s(_sendBuf, "pose,%.04f,%.04f,%.04f", _touch.pos[2]+140.8, _touch.pos[0], _touch.pos[1]);
		sendSize = sendto(m_clientSock, (char*)_sendBuf, BUFFER_SIZE, 0, (struct sockaddr*)&m_ToServer, sizeof(m_ToServer));
		if (sendSize != BUFFER_SIZE) {
			std::cout << "sendto() error!" << std::endl;
			return;
		}

        int size = sizeof(m_FromServer);
        recvSize = recvfrom(m_clientSock, (char*)_recvBuf, BUFFER_SIZE, 0, (sockaddr*)&m_FromServer, &size);
        if (recvSize < 0) {
            std::cout << "recvfrom() error!" << "\r";
            return;
        }
        else {
            char* temp = strtok(_recvBuf, ",");
            if (!strcmp(temp,"j")) {
				while (temp != NULL) {
					_recvPacket[checkSize++] = atof(temp);
					temp = strtok(NULL, ",");
				}
            }
            _kuka.pos[0] = _recvPacket[1];
            _kuka.pos[1] = _recvPacket[2];
            _kuka.pos[2] = _recvPacket[3];
            checkSize = 0;
        }
#else
        //if (omega_state) {
            //sprintf_s(_sendBuf, "%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%d,", _omega.pos[0], _omega.pos[1], _omega.pos[2], _omega.ori[0], _omega.ori[1], _omega.ori[2], (int)_omega.grp);
            //sprintf_s(_sendBuf, "%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%d,", _can.force[0], _can.force[1], _can.force[2], _touch.ori[0], _touch.ori[1], _touch.ori[2], _touch.state);
            
        //if (_touch.state == 1) {
            sprintf_s(_sendBuf, "%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%d,", _touch.pos[2], _touch.pos[0], _touch.pos[1], _touch.ori[0], _touch.ori[1], _touch.ori[2], _touch.state);
            sendSize = sendto(m_clientSock, (char*)_sendBuf, BUFFER_SIZE, 0, (struct sockaddr*)&m_ToServer, sizeof(m_ToServer));
            if (sendSize != BUFFER_SIZE) {
                std::cout << "sendto() error!" << std::endl;
                return;
            }
        //}


            int size = sizeof(m_FromServer);
            recvSize = recvfrom(m_clientSock, (char*)_recvBuf, BUFFER_SIZE, 0, (sockaddr*)&m_FromServer, &size);
            if (recvSize < 0) {
                std::cout << "recvfrom() error!" << "\r";
                return;
            }
            else {
                char* temp = strtok(_recvBuf, ",");
                while (temp != NULL) {
                    _recvPacket[checkSize++] = atof(temp);
                    temp = strtok(NULL, ",");
                }

                //_kuka.pos[0] = _recvPacket[0];              //_kuka.force[0] = _recvPacket[0];
                //_kuka.pos[1] = _recvPacket[1];              //_kuka.force[1] = _recvPacket[1];
                //_kuka.pos[2] = _recvPacket[2];              //_kuka.force[2] = _recvPacket[2];
                //_kuka.ori[0] = _recvPacket[3];              //_kuka.torq[0]  = _recvPacket[3];
                //_kuka.ori[1] = _recvPacket[4];              //_kuka.torq[1]  = _recvPacket[4];
                //_kuka.ori[2] = _recvPacket[5];              //_kuka.torq[2]  = _recvPacket[5];
                
                _kuka.pos[0] = _recvPacket[0];
                _kuka.pos[1] = _recvPacket[1];
                _kuka.pos[2] = _recvPacket[2];
                _kuka.ori[0] = _recvPacket[3];
                _kuka.ori[1] = _recvPacket[4];
                _kuka.ori[2] = _recvPacket[5];
                _kuka.jpos[0] = _recvPacket[6];
                _kuka.jpos[1] = _recvPacket[7];
                _kuka.jpos[2] = _recvPacket[8];
                _kuka.jpos[3] = _recvPacket[9];
                _kuka.jpos[4] = _recvPacket[10];
                _kuka.jpos[5] = _recvPacket[11];
                _kuka.jpos[6] = _recvPacket[12];
                

                for (int i = 0; i < 7; i++) {
                    cosj[i] = cos(_kuka.jpos[i]);
                    sinj[i] = sin(_kuka.jpos[i]);
                }

                _Ob << 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    -GA * cosj[1], GA* sinj[1], -GA * cosj[1] * cosj[2], GA* cosj[1] * sinj[2], -GA * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]), -GA * (cosj[3] * sinj[1] - cosj[1] * cosj[2] * sinj[3]), -GA * (cosj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) - cosj[1] * sinj[2] * sinj[4]), GA* (sinj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) + cosj[1] * cosj[4] * sinj[2]), -GA * (cosj[5] * (cosj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) - cosj[1] * sinj[2] * sinj[4]) - sinj[5] * (cosj[3] * sinj[1] - cosj[1] * cosj[2] * sinj[3])), GA* (sinj[5] * (cosj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) - cosj[1] * sinj[2] * sinj[4]) + cosj[5] * (cosj[3] * sinj[1] - cosj[1] * cosj[2] * sinj[3])), GA* (sinj[6] * (sinj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) + cosj[1] * cosj[4] * sinj[2]) - cosj[6] * (cosj[5] * (cosj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) - cosj[1] * sinj[2] * sinj[4]) - sinj[5] * (cosj[3] * sinj[1] - cosj[1] * cosj[2] * sinj[3]))), GA* (cosj[6] * (sinj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) + cosj[1] * cosj[4] * sinj[2]) + sinj[6] * (cosj[5] * (cosj[4] * (sinj[1] * sinj[3] + cosj[1] * cosj[2] * cosj[3]) - cosj[1] * sinj[2] * sinj[4]) - sinj[5] * (cosj[3] * sinj[1] - cosj[1] * cosj[2] * sinj[3]))),
                    0, 0, GA* sinj[1] * sinj[2], GA* cosj[2] * sinj[1], GA* cosj[3] * sinj[1] * sinj[2], -GA * sinj[1] * sinj[2] * sinj[3], GA* (cosj[2] * sinj[1] * sinj[4] + cosj[3] * cosj[4] * sinj[1] * sinj[2]), GA* (cosj[2] * cosj[4] * sinj[1] - cosj[3] * sinj[1] * sinj[2] * sinj[4]), GA* (cosj[5] * (cosj[2] * sinj[1] * sinj[4] + cosj[3] * cosj[4] * sinj[1] * sinj[2]) + sinj[1] * sinj[2] * sinj[3] * sinj[5]), -GA * (sinj[5] * (cosj[2] * sinj[1] * sinj[4] + cosj[3] * cosj[4] * sinj[1] * sinj[2]) - cosj[5] * sinj[1] * sinj[2] * sinj[3]), GA* (sinj[6] * (cosj[2] * cosj[4] * sinj[1] - cosj[3] * sinj[1] * sinj[2] * sinj[4]) + cosj[6] * (cosj[5] * (cosj[2] * sinj[1] * sinj[4] + cosj[3] * cosj[4] * sinj[1] * sinj[2]) + sinj[1] * sinj[2] * sinj[3] * sinj[5])), GA* (cosj[6] * (cosj[2] * cosj[4] * sinj[1] - cosj[3] * sinj[1] * sinj[2] * sinj[4]) - sinj[6] * (cosj[5] * (cosj[2] * sinj[1] * sinj[4] + cosj[3] * cosj[4] * sinj[1] * sinj[2]) + sinj[1] * sinj[2] * sinj[3] * sinj[5])),
                    0, 0, 0, 0, GA* (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3]), -GA * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]), GA* cosj[4] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3]), -GA * sinj[4] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3]), GA* (sinj[5] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + cosj[4] * cosj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])), GA* (cosj[5] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])), GA* (cosj[6] * (sinj[5] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + cosj[4] * cosj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])) - sinj[4] * sinj[6] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])), -GA * (sinj[6] * (sinj[5] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + cosj[4] * cosj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])) + cosj[6] * sinj[4] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])),
                    0, 0, 0, 0, 0, 0, -GA * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2]), -GA * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]), -GA * cosj[5] * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2]), GA* sinj[5] * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2]), -GA * (sinj[6] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) + cosj[5] * cosj[6] * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2])), -GA * (cosj[6] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) - cosj[5] * sinj[6] * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2])),
                    0, 0, 0, 0, 0, 0, 0, 0, -GA * (sinj[5] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) + cosj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])), -GA * (cosj[5] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) - sinj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])), -GA * cosj[6] * (sinj[5] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) + cosj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])), GA* sinj[6] * (sinj[5] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) + cosj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])),
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -GA * (cosj[6] * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2]) + sinj[6] * (cosj[5] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) - sinj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3]))), GA* (sinj[6] * (sinj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) - cosj[4] * sinj[1] * sinj[2]) - cosj[6] * (cosj[5] * (cosj[4] * (cosj[1] * sinj[3] - cosj[2] * cosj[3] * sinj[1]) + sinj[1] * sinj[2] * sinj[4]) - sinj[5] * (cosj[1] * cosj[3] + cosj[2] * sinj[1] * sinj[3])));
                _torq = _Ob * _pc;

                for (int i = 0; i < 7; i++) {
                    _kuka.jtorq[i] = _torq[i];
                    _kuka.jtorq[i] *= kt[i];
                }

                udp_state = true;
                SET_SYSTEM_MEMORY(_memname_state, udp_state);

                checkSize = 0;
            }
        //}
#endif
			SET_SYSTEM_MEMORY(_memname_kuka, _kuka);
    }
}