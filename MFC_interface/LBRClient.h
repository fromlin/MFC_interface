//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: LBRClient.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _KUKA_FRI_LBRCLIENT_H
#define _KUKA_FRI_LBRCLIENT_H

#ifdef  _M_IX86

#define _USE_MATH_DEFINES
#include "../DataType.h"
#include "../SystemMemory.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include "friLBRClient.h"
#include "friTransformationClient.h"

#define DH_D1            0.36
#define DH_D3            0.42
#define DH_D5            0.4
#define DH_D7            0.126
#define DH_ALPHA       -M_PI*0.5

#define USER_STIFFNESS   0.25      // [N/mm]
#define USER_DAMPING     0.0       // [Ns/mm]
#define USER_DIST_LIMIT  4.0       // [mm]

using namespace Eigen;

class MyLBRClient : public KUKA::FRI::LBRClient {

public:
    MyLBRClient(double forceX, double forceY, double forceZ);
    ~MyLBRClient();

    virtual void onStateChange(KUKA::FRI::ESessionState oldState, KUKA::FRI::ESessionState newState);
    virtual void monitor();
    virtual void waitForCommand();
    virtual void command();
    Matrix4d transformMat(double theta, double d, double a, double alpha);

    //Matrix4d forwardKinematics(double* j);

private:
    static const int CART_VECTOR_DIM = 6;   //!< number of elements in a Cartesian vector

    double _forceX;
    double _forceY;
    double _forceZ;
    double _wrench[CART_VECTOR_DIM];
    double _etorques[KUKA::FRI::LBRState::NUMBER_OF_JOINTS];
    double _mtorques[KUKA::FRI::LBRState::NUMBER_OF_JOINTS];
    double _torques[KUKA::FRI::LBRState::NUMBER_OF_JOINTS];
    double _joint[KUKA::FRI::LBRState::NUMBER_OF_JOINTS];
    
    KuKaData_t _kuka;
    TouchData_t _touch;

    const double* _erecv;
    const double* _mrecv;
    Vector3d _dist;
    Vector3d _fvec;

    //Matrix4d _T;
    //Matrix3d _R;
};



class TFproviClient : public KUKA::FRI::TransformationClient
{
private:
    int udp_state;
    double _tf[3][4];
    double _pos[3];

    Vector3d _dist;
    Vector3d _fvec;
    KuKaData_t _kuka;
    TouchData_t _touch;

    //std::ofstream fout;

public:

    TFproviClient();
    ~TFproviClient();

    virtual void provide();
};


#endif
#endif
