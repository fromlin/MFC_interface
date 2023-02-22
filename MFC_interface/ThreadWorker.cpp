//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: ThreadWorker.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThreadWorker.h"
#include "SharedMemory.h"
#include "Mmsystem.h"

#include <thread>
#include <chrono>


class CThreadedWorker::_CThreadedWorker {

	// Define ////////////////////////////////////////////////////////
public:

protected:

private:
	typedef struct _ThreadData {

		int runFlag;
		int isRunning;
		CWorkBase* poWork;
		double period;
	}_ThreadData_t;

	// Method ////////////////////////////////////////////////////////
public:
	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _CThreadedWorker
	// Input	: None
	// Output	: None
	// Summury	: Standard constructor
	////////////////////////////////////////////////////////////////////////////////////////////
	_CThreadedWorker() :
		_period(0.), _isRunning(false) {


		TIMECAPS tc;
		UINT     wTimerRes;
		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
		{
			// Error; application can't continue.
		}
		wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
		timeBeginPeriod(wTimerRes);
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: ~_CThreadedWorker
	// Input	: None
	// Output	: None
	// Summury	: Standard destructor
	////////////////////////////////////////////////////////////////////////////////////////////
	~_CThreadedWorker() {

		if (_isRunning)	StopWork();

		_deleteWork();
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: SetWork
	// Input	: work pointer(CWorkBase*)
	// Output	: Result(bool)
	// Summury	: Set Work address.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool SetWork(CWorkBase* poWork) {

		if (!poWork)	return false;
		if (_poWork)	delete _poWork;

		_poWork = poWork;
		//_poWork->_setWorker(this);
		return true;
	}


	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: SetWork
	// Input	: work pointer(CWorkBase*)
	// Output	: Result(bool)
	// Summury	: Set Work address.
	////////////////////////////////////////////////////////////////////////////////////////////
	CWorkBase* GetWork() {
		return _poWork;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: StartWork
	// Input	: None
	// Output	: Result(bool)
	// Summury	: Start Worker thread.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool StartWork() {

		_ThreadData_t threadData;
		_threadMemory.CreateSharedMemory(sizeof(_ThreadData_t));
		threadData.runFlag = true;
		threadData.isRunning = false;
		threadData.poWork = _poWork;
		threadData.period = _period;

		_threadMemory.SetSharedMemory(&threadData);

		_poThread = new std::thread(_ThreadWork, &_threadMemory);
		_poThread->detach();

		_isRunning = true;

		return true;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: StopWork
	// Input	: None
	// Output	: Result(bool)
	// Summury	: Stop Worker thread.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool StopWork() {

		_ThreadData_t threadData;
		_threadMemory.GetSharedMemory(&threadData);
		threadData.runFlag = false;


		for (int i = 0; i < 100; i++) {
			_threadMemory.SetSharedMemory(&threadData);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			if (!threadData.isRunning) {
				_isRunning = false;
				return true;
			}
		}

		return false;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: isRunning
	// Input	: None
	// Output	: Running state flag(bool)
	// Summury	: Return running flag.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool isRunning() {

		return _isRunning;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: SetPeriod
	// Input	: Period time(s)(double)
	// Output	: None
	// Summury	: Set period. t(s)
	////////////////////////////////////////////////////////////////////////////////////////////
	void SetPeriod(double t) {

		_period = t;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: GetPeriod
	// Input	: None
	// Output	: Period time(s)(double)
	// Summury	: Return period. t(s)
	////////////////////////////////////////////////////////////////////////////////////////////
	double GetPeriod() {

		return _period;
	}

protected:

private:
	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _ThreadWork(static)
	// Input	: Sync shared memory pointer(CSyncSharedMemory*)
	// Output	: None
	// Summury	: Worker thread funcion.
	////////////////////////////////////////////////////////////////////////////////////////////
	static void _ThreadWork(CSyncSharedMemory* poMemory) {

		_ThreadData_t data;
		CWorkBase *poWork;
		std::chrono::system_clock::time_point start;

		//Get values from shared memory.
		poMemory->GetSharedMemory(&data);
		poWork = data.poWork;
		std::chrono::duration<double> period = std::chrono::milliseconds(static_cast<int>(1000 * data.period));

		//Thread Loop.
		while (data.runFlag) {

			start = std::chrono::system_clock::now();

			poWork->_executeWork();

			std::chrono::duration<double> sec = start - std::chrono::system_clock::now() + period;
			std::this_thread::sleep_for(sec);

			//Update flag.
			poMemory->GetSharedMemory(&data);
		}

		data.isRunning = false;
		poMemory->SetSharedMemory(&data);
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _deleteWork
	// Input	: None
	// Output	: None
	// Summury	: Delete work.
	////////////////////////////////////////////////////////////////////////////////////////////
	void _deleteWork() {

		if (_poWork)	delete _poWork;
	}

	// Member ////////////////////////////////////////////////////////
public:

protected:

private:
	CSyncSharedMemory _threadMemory;
	CWorkBase *_poWork;
	std::thread *_poThread;
	double _period;
	bool _isRunning;
};



CThreadedWorker::CThreadedWorker()
{
	_poWorker = new _CThreadedWorker();
}



CThreadedWorker::~CThreadedWorker()
{
	delete _poWorker;
}



bool CThreadedWorker::SetWork(CWorkBase* poWork) {

	return _poWorker->SetWork(poWork);
}

CWorkBase* CThreadedWorker::GetWork() {
	return _poWorker->GetWork();
}


bool CThreadedWorker::StartWork() {

	return _poWorker->StartWork();
}



bool CThreadedWorker::StopWork() {

	return _poWorker->StopWork();
}



bool CThreadedWorker::isRunning() {

	return _poWorker->isRunning();
}


void CThreadedWorker::SetPeriod(double t) {

	_poWorker->SetPeriod(t);
}



double CThreadedWorker::GetPeriod() {

	return _poWorker->GetPeriod();
}