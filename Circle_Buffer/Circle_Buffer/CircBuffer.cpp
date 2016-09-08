#include "CircBuffer.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

CircBufferFixed::CircBufferFixed(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	this->buffName = buffName;
	this->buffSize = buffSize;
	this->isProducer = isProducer;
	this->chunkSize = chunkSize;

}

CircBufferFixed::~CircBufferFixed()
{
	UnmapViewOfFile(pBuf);
	CloseHandle(MapingFile);

}

bool CircBufferFixed::createMapingProducer() 
{
	#define BUF_SIZE buffSize
	
	TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
	TCHAR szMsg[] = TEXT("Message from first process.");
	

	HANDLE MapingFileHandler;
	LPCTSTR pBuf;


	MapingFileHandler = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (MapingFileHandler == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),GetLastError());
		return false;
	}

	
	pBuf = (LPTSTR)MapViewOfFile(MapingFileHandler,   // handle to map object 
		FILE_MAP_ALL_ACCESS,				// read/write permission
		0,0,BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),GetLastError());
		CloseHandle(MapingFileHandler);

		return false;
	}

	this->MapingFile = MapingFileHandler;
	this->pBuf = pBuf;
	
	return true;
}

bool CircBufferFixed::createMapingConsumer()
{
	
	#define BUF_SIZE buffSize
	TCHAR szName[] = TEXT("Global\\MyFileMappingObject");
	HANDLE MapingFileHandler;
	LPCTSTR pBuf;

	MapingFileHandler = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (MapingFileHandler == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return false;
	}

	pBuf = (LPTSTR)MapViewOfFile(MapingFileHandler, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(MapingFileHandler);

		return false;
	}

	this->MapingFile = MapingFileHandler;
	this->pBuf = pBuf;
	//UnmapViewOfFile(pBuf);

	//CloseHandle(hMapFile);

	return true;
	}




bool CircBufferFixed::push(const void * msg, size_t length)
{
	TCHAR szMsg[] = TEXT("Message from first process.");
	CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR))); //Copies a block of memory from one location to another.
	getchar();

	return false;
}

bool CircBufferFixed::read(const void* msg, size_t length)
{
	MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);
	return false;
}

bool CircBufferFixed::pop(char * msg, size_t & length)
{
	return false;
}
