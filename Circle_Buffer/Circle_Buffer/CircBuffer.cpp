#include "CircBuffer.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>


Control CircBufferFixed::headTails()
{
	Control ControlVariables;
	

	memcpy(&ControlVariables, ControlPointer, sizeof(Control));
	return ControlVariables;
}

CircBufferFixed::CircBufferFixed(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	this->buffName = buffName;
	this->freeMemory = this->buffSize = buffSize;
	this->isProducer = isProducer;
	this->chunkSize = chunkSize;
	this->head = 0;

	this->MessageCount = 0;
	this->MapingFile = NULL;
	this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

CircBufferFixed::~CircBufferFixed()
{
	UnmapViewOfFile(MapPointer);
	CloseHandle(MapingFile);

	UnmapViewOfFile(ControlPointer);
	CloseHandle(ControlFile);

}


bool CircBufferFixed::createMaping()
{
	//Message map
	this->MapingFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, buffSize, TEXT("FileMappingObject"));

	if (this->MapingFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"), GetLastError());
		return false;
	}


	this->MapPointer = (char*)MapViewOfFile(this->MapingFile, FILE_MAP_ALL_ACCESS, 0, 0, buffSize);

	if (MapPointer == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"), GetLastError());
		CloseHandle(this->MapingFile);
		return false;
	}

	//Controll Map;
	
	this->ControlFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Control), TEXT("ControlMappingObject"));

	if (this->ControlFile == NULL)
	{
		_tprintf(TEXT("Could not create file Control mapping object (%d).\n"), GetLastError());
		return false;
	}

	this->ControlPointer = (size_t*)MapViewOfFile(this->ControlFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Control));

	if (ControlPointer == NULL)
	{
		_tprintf(TEXT("Could not map view of Control file (%d).\n"), GetLastError());
		CloseHandle(this->MapingFile);
		return false;
	}
	size_t tail = 0;
	memcpy(&ControlPointer[1], &tail, sizeof(size_t)); //set the tail;
	return true;
}




bool CircBufferFixed::push(const void * msg, size_t length)
{
	Header currentHeader;
	currentHeader.id = MessageCount;
	currentHeader.length = length;
	currentHeader.padding = 0;

	memcpy(&MapPointer[head], &currentHeader, sizeof(Header));
	head += sizeof(Header);

	memcpy(&MapPointer[head], msg, length);
	head += length;
	
	
	cout << "Message ID: " << currentHeader.id << endl;
	cout << "Message Length: " << currentHeader.length << endl;
	cout << "Header: " << head<< endl << endl;
	MessageCount += 1;

	

	memcpy(ControlPointer,&head,sizeof(size_t));


	return false;

}

bool CircBufferFixed::read(const void* msg, size_t length)
{
	Header readHeader;
	memcpy(&readHeader, &MapPointer[head], sizeof(Header));
	head += sizeof(Header);

	
	char* readMessage = new char[readHeader.length];
	memcpy(readMessage, &MapPointer[head], readHeader.length);
	head += readHeader.length;

	SetConsoleTextAttribute(consoleHandle, 15); //change consol text color
	cout << "Message ID: " << readHeader.id << endl;
	cout << "Message Length: " << readHeader.length << endl;
	SetConsoleTextAttribute(consoleHandle, 7); //change consol text color
	cout << readMessage << endl;
	return false;
}

bool CircBufferFixed::pop(char * msg, size_t & length)
{
	return false;
}
