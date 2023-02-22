//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: SystemMemory.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _SYSTEMMEMORYBLOCK_H__
#define _SYSTEMMEMORYBLOCK_H__

#include <string>
#include "SharedMemory.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Name	: CSystemMemoryBlock
// Summury		: Work base class.
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CSystemMemory {

	// Define ////////////////////////////////////////////////////////
public:

protected:

private:
	class _CSystemMemory;

	// Method ////////////////////////////////////////////////////////
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: CSystemMemory
	// Input	: None
	// Output	: None
	// Summury	: Standard constructor
	////////////////////////////////////////////////////////////////////////////////////////////
	CSystemMemory();



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: ~CSystemMemory
	// Input	: None
	// Output	: None
	// Summury	: Standard destructor
	////////////////////////////////////////////////////////////////////////////////////////////
	~CSystemMemory();



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: CreateMemory
	// Input	: Memory block name(std::string), Memory size(size_t)
	// Output	: Result(bool)
	// Summury	: Create shared memory.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool CreateMemory(std::string name, size_t size);



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: DeleteMemory
	// Input	: Memory block name(std::string)
	// Output	: Result(bool)
	// Summury	: Delete shared memory.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool DeleteMemory(std::string name);



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: GetMemory
	// Input	: Memory block name(std::string)
	// Output	: Memory block pointer(CSyncSharedMemory*)
	// Summury	: Return memory block pointer.
	////////////////////////////////////////////////////////////////////////////////////////////
	CSyncSharedMemory* GetMemory(std::string name);

protected:

private:


	// Member ////////////////////////////////////////////////////////
public:

protected:

private:
	_CSystemMemory *_poSystemMemory;


};


extern CSystemMemory g_SystemMemory;

#define CREATE_SYSTEM_MEMORY(NAME, TYPE)	g_SystemMemory.CreateMemory( NAME , sizeof(TYPE))
#define DELETE_SYSTEM_MEMORY(NAME)			g_SystemMemory.DeleteMemory( NAME );
#define SET_SYSTEM_MEMORY(NAME, MEM)		g_SystemMemory.GetMemory( NAME )->SetSharedMemory( &MEM )
#define GET_SYSTEM_MEMORY(NAME, MEM)		g_SystemMemory.GetMemory( NAME )->GetSharedMemory( &MEM )

#endif