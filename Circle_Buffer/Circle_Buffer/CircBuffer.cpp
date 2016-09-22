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
	
	size_t tail = 0;
	memcpy(&ControlPointer[1], &tail, sizeof(size_t)); //set the tail;
	return true;
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
	
	if (CalculateFreeMemory() > (sizeof(Header) + length)) // if there is enough free memory to make a messag MIGHT COUSE ERROR 
	{
		if (ClientPosition == buffSize) // if we happened to fill the entire buffer;
		{ //om den paddar sig upp till buffsize sätter den till noll.
			//cout << "ENTIRE BUFFER FILLED MOVING HEADER TO START" << endl;
			getchar();
			ClientPosition = 0;
		}

		Header messageHeader;
		messageHeader.id = MessageCount;
		messageHeader.length = length;
		messageHeader.padding = chunkSize - ((ClientPosition + sizeof(Header) + length) % chunkSize); //pading my way.


		memcpy(&MapPointer[ClientPosition], &messageHeader, sizeof(Header)); //WRITE HEADER Because of padding there should be enough space... Apparantly
		ClientPosition += sizeof(Header); //move header

		if (buffSize - ClientPosition >= length)
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

		ClientPosition += messageHeader.padding;
		memcpy(&ControlPointer[HEAD], &ClientPosition, sizeof(size_t));
		cout << MessageCount << " ";
		cout << (char*)message << endl <<endl << "FREE: " << CalculateFreeMemory() <<endl << endl;
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

	if (ClientPosition == buffSize) // if we happened to fill the entire buffer;
	{
		//cout << "ENTIRE BUFFER FILLED MOVING HEADER TO START" << endl;
		ClientPosition = 0;
		//getchar();
	}

	
		
	if (this->ClientPosition == ControlPointer[HEAD]) // if client reached the head
	{
		//cout << "head = " << ControlPointer[HEAD] << endl << "Tail = " << ControlPointer[TAIL] << endl << endl;
		return false;
	}
	Header messageHeader;
	memcpy(&messageHeader, &MapPointer[this->ClientPosition], sizeof(Header));
	ClientPosition += sizeof(Header);


	if (buffSize - ClientPosition >= messageHeader.length)
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
	memcpy(&ControlPointer[TAIL], &this->ClientPosition, sizeof(size_t)); //update tail
	cout << "Message ID: " << messageHeader.id << endl;
	cout << message << endl << endl << endl;
	return true;
	
}






