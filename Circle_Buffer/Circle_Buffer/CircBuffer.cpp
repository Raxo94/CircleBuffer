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
	this->positionInBuffer = 0;
	this->MessageCount = 0;
	
	
	createMaping();
	UpdateControlBuffer();
}

CircBufferFixed::~CircBufferFixed()
{
	if (isProducer=false)
		ControlPointer[CLIENTCOUNT] -= 1;

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
		ControlPointer[HEAD] = 0;
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

	else if (ControlPointer[HEAD] < ControlPointer[TAIL])
	{
		return freeMemory = ControlPointer[TAIL] - ControlPointer[HEAD];
	}
}




bool CircBufferFixed::push(const char * message, size_t length)
{

	size_t padding = chunkSize - ((positionInBuffer + sizeof(Header) + length) % chunkSize); // Padding makes sure that the message is dividable by the chunkSize.
	

	if (CalculateFreeMemory() > (sizeof(Header) + length + padding)) // Send message if there is enough space in the buffer to do so.
	{
		if (positionInBuffer == buffSize) // If producer has reached the end of the shared memory, go back to the begining. 
			positionInBuffer = 0;

		MessageCount += 1;

		Header messageHeader; // Prepare the message header that will be used by clients to read the message.
		messageHeader.id = MessageCount;
		messageHeader.length = length;
		messageHeader.ClientRemaining = ControlPointer[CLIENTCOUNT];
		messageHeader.padding = padding;

		memcpy(&MapPointer[positionInBuffer], &messageHeader, sizeof(Header)); //Write the message header into the circular buffer. 
		positionInBuffer += sizeof(Header); // Update the producers position in the circular buffer.

		if (buffSize - positionInBuffer >= length + padding) //The message can be sent without spliting it. 
		{
			memcpy(&MapPointer[positionInBuffer], message, length);
			positionInBuffer += length;
			positionInBuffer += messageHeader.padding;
		}
		
		else //Split the message
		{
			size_t Rest = buffSize - positionInBuffer;
			memcpy(&MapPointer[positionInBuffer], message, Rest); //Write the first part of the message.

			positionInBuffer = 0;
			size_t Remaining = length - Rest;
			memcpy(&MapPointer[positionInBuffer], message + Rest, Remaining); //Write the rest of the message.
			positionInBuffer = Remaining; //update the producers position in the circular buffer.
		}
		
		ControlPointer[HEAD] = positionInBuffer; //communicate to clients were the producer is located in the circular buffer.
		
		
		cout << MessageCount << " "; 
		cout << (char*)message << endl;
		
		return true;
	}


	else //NOT ENOUGH MEMORY
		return false;
}


bool CircBufferFixed::pop(char * message)
{
	if (this->positionInBuffer == ControlPointer[HEAD]) // If the consumer has read all available messages, return false.
		return false;

	mutex.lock(); // Make sure no other clients read this memory at the same time.

	if (positionInBuffer == buffSize) 
		positionInBuffer = 0; //if client has reached the end of the shared memory, go back to the begining.  

	Header* messageHeader= (Header*)&MapPointer[this->positionInBuffer]; // Read the message header.
	positionInBuffer += sizeof(Header); //update this clients position in the circular buffer.


	if (buffSize - positionInBuffer >= (messageHeader->length + messageHeader->padding)) // If the message is not split.
	{
		memcpy(message, &MapPointer[this->positionInBuffer], messageHeader->length);
		positionInBuffer += messageHeader->length;
		positionInBuffer += messageHeader->padding;
	}

	else // If the message is split.
	{
		size_t part1 = buffSize - positionInBuffer; // Calculate how much of the message is still in the end of the buffer memory. 
		memcpy(message,&MapPointer[positionInBuffer], part1); // Read the first half of the message.

		positionInBuffer = 0;
		size_t part2 = messageHeader->length - part1; // Calculate how much of the message is in the begining of the bufffer memory.
		memcpy(message + part1,&MapPointer[positionInBuffer], part2); // Read the rest of the message.
		positionInBuffer = part2; //change this
	}


	messageHeader->ClientRemaining -= 1; //Notify the message that it has been read by this client. 

	if (messageHeader->ClientRemaining == 0) // If all clients have read this message, allow the producer to overwrite it.   
		ControlPointer[TAIL] = positionInBuffer;

	mutex.unlock(); //Allow other clients to read this part of the buffer memory.

	
	cout << messageHeader->id << " ";
	cout << (char*)message << endl;
	
	
	return true;
	
}






