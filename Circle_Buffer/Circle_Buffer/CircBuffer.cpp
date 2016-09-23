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
	//*ControlPointer = buffSize - 2000;
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
	return true;
}

void CircBufferFixed::UpdateControlBuffer()
{
	size_t tail = 0;
	memcpy(&ControlPointer[TAIL], &tail, sizeof(size_t)); //set the tail;

	if (isProducer == false)
	{
		size_t clientCount;
		memcpy(&clientCount, &ControlPointer[CLIENTCOUNT], sizeof(size_t)); //getClientCount;
		clientCount += 1;
		memcpy(&ControlPointer[CLIENTCOUNT], &clientCount, sizeof(size_t)); //set the clientCount;
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
	Header messageHeader;
	messageHeader.padding = chunkSize - ((ClientPosition + sizeof(Header) + length) % chunkSize); //pading my way.
	messageHeader.id = MessageCount;
	messageHeader.length = length;
	messageHeader.ClientRemaining = ControlPointer[CLIENTCOUNT];
	

	if (CalculateFreeMemory() >= (sizeof(Header) + length + messageHeader.padding)) // if there is enough free memory to make a messag MIGHT COUSE ERROR 
	{
		if (ClientPosition == buffSize) // if we happened to fill the entire buffer;
		{ 
			ClientPosition = 0;
		}

		memcpy(&MapPointer[ClientPosition], &messageHeader, sizeof(Header)); //WRITE HEADER Because of padding there should be enough space... Apparantly
		ClientPosition += sizeof(Header); //move header

		if (buffSize - ClientPosition >= (length + messageHeader.padding))
		{
			memcpy(&MapPointer[ClientPosition], message, length); //WRITE MESSAGE
			ClientPosition += length; //move header
		}
		else //if no then just jump to start
		{
			ClientPosition = 0;
			memcpy(&MapPointer[ClientPosition], message, length); //WRITE MESSAGE
			ClientPosition += length; //move header

			
		}

		//ClientPosition = (ClientPosition + messageHeader.padding) % //buffSize; istället för att kolla if satser-

		ClientPosition += messageHeader.padding;
		memcpy(&ControlPointer[HEAD], &ClientPosition, sizeof(size_t));
		cout << MessageCount << " ";
		cout << (char*)message <<endl;
	}


	else
	{
		//cout << "out of memory";
		return false;
	}



	MessageCount += 1;
	return true;
}


bool CircBufferFixed::pop(char * message, size_t & length)
{
	mutex.lock();
	if (this->ClientPosition == ControlPointer[HEAD]) // if client reached the head
	{
		//cout << "head = " << ControlPointer[HEAD] << endl << "Tail = " << ControlPointer[TAIL] << endl << endl;
		mutex.unlock();
		return false;
	}
	if (ClientPosition == buffSize) // if we happened to fill the entire buffer;
	{
		//cout << "ENTIRE BUFFER FILLED MOVING HEADER TO START" << endl;
		ClientPosition = 0;
		//getchar();
	}

	//hej oskar /staffan / HEJ steffe
	
	
	
	Header messageHeader;
	
	memcpy(&messageHeader, &MapPointer[this->ClientPosition], sizeof(Header)); //read message header
	messageHeader.ClientRemaining -= 1;
	memcpy(&MapPointer[this->ClientPosition], &messageHeader, sizeof(Header)); //set updated message header

	ClientPosition += sizeof(Header); //move pointer forward;


	if (buffSize - ClientPosition >= (messageHeader.length + messageHeader.padding)) // here
	{
		memcpy(message, &MapPointer[this->ClientPosition], messageHeader.length);
		ClientPosition += messageHeader.length;
	}
	else 
	{
		ClientPosition = 0;
		memcpy(message, &MapPointer[this->ClientPosition], messageHeader.length);
		ClientPosition += messageHeader.length;
	}

	ClientPosition += messageHeader.padding;


	if (messageHeader.ClientRemaining == 0)
	{
		memcpy(&ControlPointer[TAIL], &this->ClientPosition, sizeof(size_t)); //update tail
	}
	
	cout <<  messageHeader.id <<" ";
	cout << (char*)message << endl;
	mutex.unlock();
	return true;
	
}






