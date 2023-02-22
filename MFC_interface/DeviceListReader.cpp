//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: DeviceListReader.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeviceListReader.h"
#include <windows.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Name	: CDeviceListReader::_CDeviceListReader
// Summury		: Private Class for CDeviceListReader
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CDeviceListReader::_CDeviceListReader {

// Define ////////////////////////////////////////////////////////
public:

protected:

private:


// Method ////////////////////////////////////////////////////////
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _CDeviceListReader
	// Input	: None
	// Output	: None
	// Summury	: Standard constructor
	////////////////////////////////////////////////////////////////////////////////////////////
	_CDeviceListReader() {

	}


	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: ~_CDeviceListReader
	// Input	: None
	// Output	: None
	// Summury	: Standard destructor
	////////////////////////////////////////////////////////////////////////////////////////////
	~_CDeviceListReader() {

	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: UpdateDeviceList
	// Input	: deviceName or Key value(const char)
	// Output	: Result(bool)
	// Summury	: Get device list from registry.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool UpdateDeviceList(const char* deviceName) {

		return _updateDeviceList(deviceName);
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: GetDeviceList
	// Input	: list storage(std::vector<std::string>&)
	// Output	: None
	// Summury	: Copy device list.
	////////////////////////////////////////////////////////////////////////////////////////////
	void GetDeviceList(std::vector<std::string> &list) {

		list = _list;
	}

protected:

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _updateDeviceList
	// Input	: deviceName or Key value(const char)
	// Output	: Result(bool)
	// Summury	: Get device list from registry.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool _updateDeviceList(const char* deviceName) {

		HKEY hKey;

		char keyStr[100];
		sprintf_s(keyStr, sizeof(keyStr), "HARDWARE\\DEVICEMAP\\%s", deviceName);

		RegOpenKey(HKEY_LOCAL_MACHINE, TEXT(keyStr), &hKey);


		TCHAR szData[20], szName[100];
		DWORD index = 0, dwSize = 100, dwSize2 = 20, dwType = REG_SZ;

		memset(szData, 0, sizeof(szData));
		memset(szName, 0, sizeof(szName));


		while (ERROR_SUCCESS == RegEnumValue(hKey, index, szName, &dwSize, NULL, NULL, NULL, NULL)){

			index++;

			RegQueryValueEx(hKey, szName, NULL, &dwType, (LPBYTE)szData, &dwSize2);

			std::string deviceName = szData;
			_list.push_back(deviceName);

			memset(szData, 0, sizeof(szData));
			memset(szName, 0, sizeof(szName));
			dwSize = 100;
			dwSize2 = 20;
		}

		RegCloseKey(hKey);

		return true;
	}

// Member ////////////////////////////////////////////////////////
public:

protected:

private:
	std::vector<std::string> _list;
};



CDeviceListReader::CDeviceListReader()
{
	_poDeviceListReader = new _CDeviceListReader;
}



CDeviceListReader::~CDeviceListReader()
{
	delete _poDeviceListReader;
}



bool CDeviceListReader::UpdateDeviceList(const char* deviceName) {

	return _poDeviceListReader->UpdateDeviceList(deviceName);
}



void CDeviceListReader::GetDeviceList(std::vector<std::string> &list) {

	return _poDeviceListReader->GetDeviceList(list);
}