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

bool CircBufferFixed::createMapingProducer() 
{
	
	
	TCHAR szName[] = TEXT("MyFileMappingObject");
	


	this->MapingFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		buffSize,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (this->MapingFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),GetLastError());
		return false;
	}

	
	this->bufferPointer = (char*)MapViewOfFile(this->MapingFile,   // handle to map object 
		FILE_MAP_ALL_ACCESS,				// read/write permission
		0,0,buffSize);

	if (bufferPointer == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),GetLastError());
		CloseHandle(this->MapingFile);

		return false;
	}

	
	
	return true;
}

bool CircBufferFixed::createMapingConsumer()
{
	
	TCHAR szName[] = TEXT("MyFileMappingObject");
	
	this->MapingFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (this->MapingFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return false;
	}

	this->bufferPointer = (char*)MapViewOfFile(this->MapingFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		buffSize);

	if (bufferPointer == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(this->MapingFile);

		return false;
	}

	return true;
	}




bool CircBufferFixed::push(const void * msg, size_t length)
{
	//char writeMessage[] = "This is the first message. And also the most important one.\nRemember and never forget!";

#pragma region push Header

	Header currentHeader;
	currentHeader.id = MessageCount;
	currentHeader.length = sizeof(length);
	currentHeader.padding = 0;

	memcpy(this->bufferPointer, &currentHeader, sizeof(Header));
	bufferPointer += sizeof(Header);	
#pragma endregion

#pragma region push message

	memcpy(this->bufferPointer, msg, length);
	bufferPointer += length;
#pragma endregion

	getchar();
	return false;

}

bool CircBufferFixed::read(const void* msg, size_t length)
{
	 //change color of text

#pragma region READ MESSAGE
	Header readHeader;

	memcpy(&readHeader, bufferPointer, sizeof(Header));
	bufferPointer += sizeof(Header);

	
	
#pragma endregion

#pragma region READ MESSAGE
	char* readMessage = new char[readHeader.length];
	memcpy(readMessage, bufferPointer, readHeader.length);
	
#pragma endregion

	SetConsoleTextAttribute(consoleHandle, 15); //change consol text color
	cout << "Message ID: " << readHeader.id << endl;
	SetConsoleTextAttribute(consoleHandle, 7); //change consol text color
	cout << readMessage << endl;
	getchar();
	return false;
}

bool CircBufferFixed::pop(char * msg, size_t & length)
{
	return false;
}
