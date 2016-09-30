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
	this->isProducer = isProducer;
	this->chunkSize = chunkSize;
	this->ClientPosition = 0;
	this->MessageCount = 0;
	this->consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	

	createMaping();
	UpdateControlBuffer();
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
	this->MapingFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)buffSize, TEXT("FileMappingObject"));

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

	return true; //DONE
}

void CircBufferFixed::UpdateControlBuffer()
{
	
	if (isProducer)
	{
		ControlPointer[HEAD] = 0;
	}
	else
	{
		ControlPointer[TAIL] = 0;
		ControlPointer[CLIENTCOUNT] += 1;
	}
}

size_t CircBufferFixed::CalculateFreeMemory()
{
	size_t freeMemory;
	if (ControlPointer[HEAD] >= ControlPointer[TAIL])
	{
		return freeMemory = (buffSize - ControlPointer[HEAD]) + ControlPointer[TAIL];
	}
	else
	{
		return freeMemory = ControlPointer[TAIL] - ControlPointer[HEAD];
	}
}




bool CircBufferFixed::push(const void * message, size_t length)
{

	size_t padding = chunkSize - ((ClientPosition + sizeof(Header) + length) % chunkSize); //padding my way through the darkness.
	
	if (CalculateFreeMemory() > (sizeof(Header) + length + padding)) // WRITE if enough free memory for message. Make sure not to reach the tail.
	{
		Header messageHeader;
		messageHeader.id = MessageCount;
		messageHeader.length = length;
		messageHeader.ClientRemaining = ControlPointer[CLIENTCOUNT];
		messageHeader.padding = padding;

		memcpy(&MapPointer[ClientPosition], &messageHeader, sizeof(Header)); //WRITE HEADER Because of padding there should be >= 256 bites left.


		if (buffSize - ClientPosition >= (length + padding)) //WRITE MESSAGE
		{
			ClientPosition += sizeof(Header);
			memcpy(&MapPointer[ClientPosition], message, length); 
		}


		else if (ControlPointer[TAIL] > length + padding) //RECYCLE BUFFER WRITE MESSAGE
		{
			ClientPosition += sizeof(Header);
			ClientPosition = 0;
			memcpy(&MapPointer[ClientPosition], message, length); 
		}
		else //NOT ENOUGH MEMORY
			return false;


		ClientPosition += length; 
		ClientPosition += messageHeader.padding;

		
		ControlPointer[HEAD] = ClientPosition;
		ClientPosition = ClientPosition % buffSize;  //filled the buffer. Pointer will be reset;
		
		cout << MessageCount << " ";
		//cout << (char*)message << endl;

		MessageCount += 1;
		return true;
	}


	else //NO MEMORY
		return false;
}


bool CircBufferFixed::pop(char * message, size_t & length)
{
	if (this->ClientPosition == ControlPointer[HEAD]) //CONSUMER HAS READ ALL MESSAGES
		return false;

	ClientPosition = ClientPosition % buffSize; //filled the buffer. Pointer will be reset;
	 
	mutex.lock();
	Header* messageHeader= (Header*)&MapPointer[this->ClientPosition]; //read header
	messageHeader->ClientRemaining -= 1;

	ClientPosition += sizeof(Header); //move pointer forward;

	if (buffSize - ClientPosition >= (messageHeader->length + messageHeader->padding)) 
		memcpy(message, &MapPointer[this->ClientPosition], messageHeader->length);

	else 
	{
		ClientPosition = 0;
		memcpy(message, &MapPointer[this->ClientPosition], messageHeader->length);
	}

	ClientPosition += messageHeader->length;
	ClientPosition += messageHeader->padding;

	if (messageHeader->ClientRemaining == 0)
		ControlPointer[TAIL] = ClientPosition;

	mutex.unlock(); //Allow other clients to read

	cout <<  messageHeader->id <<" ";
	//cout << (char*)message << endl;
	

	
	return true;
	
}






