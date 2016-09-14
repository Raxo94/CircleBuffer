#include "CircBuffer.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>

CircBufferFixed::CircBufferFixed(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	this->buffName = buffName;
	this->buffSize = buffSize;
	this->isProducer = isProducer;
	this->chunkSize = chunkSize;

	this->MessageCount = 0;
	this->MapingFile = NULL;

	this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

CircBufferFixed::~CircBufferFixed()
{
	UnmapViewOfFile(bufferPointer);
	CloseHandle(MapingFile);

}

//bool CircBufferFixed::createMapingProducer() 
//{
//	
//	
//	TCHAR szName[] = TEXT("MyFileMappingObject");
//	
//
//
//	this->MapingFile = CreateFileMapping(
//		INVALID_HANDLE_VALUE,    // use paging file
//		NULL,                    // default security
//		PAGE_READWRITE,          // read/write access
//		0,                       // maximum object size (high-order DWORD)
//		buffSize,                // maximum object size (low-order DWORD)
//		szName);                 // name of mapping object
//
//	if (this->MapingFile == NULL)
//	{
//		_tprintf(TEXT("Could not create file mapping object (%d).\n"),GetLastError());
//		return false;
//	}
//
//	
//	this->bufferPointer = (char*)MapViewOfFile(this->MapingFile,   // handle to map object 
//		FILE_MAP_ALL_ACCESS,				// read/write permission
//		0,0,buffSize);
//
//	if (bufferPointer == NULL)
//	{
//		_tprintf(TEXT("Could not map view of file (%d).\n"),GetLastError());
//		CloseHandle(this->MapingFile);
//
//		return false;
//	}
//
//	
//	
//	return true;
//}
//
//bool CircBufferFixed::createMapingConsumer()
//{
//	
//	TCHAR szName[] = TEXT("MyFileMappingObject");
//	
//	this->MapingFile = OpenFileMapping(
//		FILE_MAP_ALL_ACCESS,   // read/write access
//		FALSE,                 // do not inherit the name
//		szName);               // name of mapping object
//
//	if (this->MapingFile == NULL)
//	{
//		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
//			GetLastError());
//		return false;
//	}
//
//	this->bufferPointer = (char*)MapViewOfFile(this->MapingFile, // handle to map object
//		FILE_MAP_ALL_ACCESS,  // read/write permission
//		0,
//		0,
//		buffSize);
//
//	if (bufferPointer == NULL)
//	{
//		_tprintf(TEXT("Could not map view of file (%d).\n"),
//			GetLastError());
//
//		CloseHandle(this->MapingFile);
//
//		return false;
//	}
//
//	return true;
//	}
//
bool CircBufferFixed::createMaping()
{
	TCHAR szName[] = TEXT("FileMappingObject");

	this->MapingFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		buffSize,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (this->MapingFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
		return false;
	}


	this->bufferPointer = (char*)MapViewOfFile(this->MapingFile,   // handle to map object 
		FILE_MAP_ALL_ACCESS,				// read/write permission
		0, 0, buffSize);

	if (bufferPointer == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(this->MapingFile);

		return false;
	}



	return true;
}




bool CircBufferFixed::push(const void * msg, size_t length)
{
	Header currentHeader;
	currentHeader.id = MessageCount;
	currentHeader.length = length;
	currentHeader.padding = 0;

	memcpy(this->bufferPointer, &currentHeader, sizeof(Header));
	bufferPointer += sizeof(Header);	



	memcpy(this->bufferPointer, msg, length);
	bufferPointer += length;

	cout << "Message ID: " << currentHeader.id << endl;
	cout << "Message Length: " << currentHeader.length << endl;

	getchar();
	return false;

}

bool CircBufferFixed::read(const void* msg, size_t length)
{
	Header readHeader;
	memcpy(&readHeader, bufferPointer, sizeof(Header));
	bufferPointer += sizeof(Header);


	char* readMessage = new char[readHeader.length];
	memcpy(readMessage, bufferPointer, readHeader.length);
	

	SetConsoleTextAttribute(consoleHandle, 15); //change consol text color
	cout << "Message ID: " << readHeader.id << endl;
	cout << "Message Length: " << readHeader.length << endl;
	SetConsoleTextAttribute(consoleHandle, 7); //change consol text color
	cout << readMessage << endl;
	getchar();
	return false;
}

bool CircBufferFixed::pop(char * msg, size_t & length)
{
	return false;
}
