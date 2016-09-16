#include "CircBuffer.h"
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <iostream>



CircBufferFixed::CircBufferFixed(LPCWSTR buffName, const bool & isProducer, const size_t & buffSize, const size_t & chunkSize)
{
	this->buffName = buffName;
	this->buffSize = buffSize;
	this->freeMemory = buffSize;
	this->isProducer = isProducer;
	this->chunkSize = chunkSize;
	this->ClientPosition = 0;
	this->MessageCount = 0;
	this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	createMaping();
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




bool CircBufferFixed::push(const void * message, size_t length)
{

	if (freeMemory > (length + sizeof(Header))) // IF THERE IS MEMORY
	{
		
		Header messageHeader;
		messageHeader.id = MessageCount;
		messageHeader.length = length;
		messageHeader.padding = 0;

		memcpy(&MapPointer[ClientPosition], &messageHeader, sizeof(Header)); //WRITE HEADER
		ClientPosition += sizeof(Header); //move header

		memcpy(&MapPointer[ClientPosition], message, length); //WRITE MESSAGE
		ClientPosition += length; //move header

		freeMemory -= (length + sizeof(Header));

		cout << "Message ID: " << messageHeader.id << endl;
		cout << "Message Length: " << messageHeader.length << endl;
		cout << "Header: " << ClientPosition << endl << endl;
		MessageCount += 1;

		memcpy(&ControlPointer[HEAD], &ClientPosition, sizeof(size_t));
		cout << "FreeMemory= " << freeMemory << endl << endl << endl;

	}
	else //IF THERE IS NO MEMORY;
	{
		cout << "OUT OF MEMORY" << endl;
		freeMemory = buffSize;
		ClientPosition = 0; //this is a test

	}
	
	delete[] message;
	return true;
}


bool CircBufferFixed::pop(char * message, size_t & length)
{
	Header messageHeader;
	size_t *head = ControlPointer;
	size_t *tail = head + 1; 
		
	
	while (*tail == *head)
	{
		this_thread::sleep_for(std::chrono::milliseconds(5));
		cout << "head = " << *head << endl << "Tail = " << *tail << endl << endl;
	}

	cout << "head = " << *head << endl << "Tail = " <<*tail <<endl << endl;

	memcpy(&messageHeader, &MapPointer[this->ClientPosition], sizeof(Header));
	ClientPosition += sizeof(Header);

	memcpy(message, &MapPointer[this->ClientPosition], messageHeader.length);
	ClientPosition += messageHeader.length;

	cout << "Message ID: " << messageHeader.id << endl;
	cout << "Message Length: " << messageHeader.length << endl;
	

	memcpy(&ControlPointer[TAIL], &this->ClientPosition, sizeof(size_t)); //update tail
	return true;

	//
}






