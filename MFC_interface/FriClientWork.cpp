//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: FriClientwork.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef  _M_IX86

#include "FriClientWork.h"

MyLBRClient lbr(0.0, 0.0, 0.0);
TFproviClient trafoClient;
UdpConnection connection;
ClientApplication app(connection, lbr, trafoClient);
//ClientApplication app(connection, lbr);

CFriClientWork::CFriClientWork(std::string name)
    :CWorkBase(name)
{
    memset(&_kuka, 0, sizeof(KuKaData_t));

    _memname_kuka = name + "_KuKa";
    CREATE_SYSTEM_MEMORY(_memname_kuka, KuKaData_t);

    _isOpen = false;
}

CFriClientWork::~CFriClientWork() {

    DELETE_SYSTEM_MEMORY(_memname_kuka);

    ClosePort();
}

bool CFriClientWork::OpenPort() {

    if (!app.connect(DEFAULT_PORT, IP_KUKA_FRI)) {
        return false;
    }

    _isOpen = true;
    return _isOpen;
}

void CFriClientWork::ClosePort() {

    app.disconnect();
    memset(&_kuka, 0, sizeof(KuKaData_t));
    SET_SYSTEM_MEMORY(_memname_kuka, _kuka);
}

void CFriClientWork::_execute() {
    
    app.step();

    if (lbr.robotState().getSessionState() == IDLE) {
        return;
    }
}
#endif