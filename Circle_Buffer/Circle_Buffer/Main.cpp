
#include "stdio.h"
#include <iostream>
#include "CircBuffer.h"
#include <thread> 
#include <chrono>   
#include "OExtraFunctions.hpp"

int main(size_t argc, char* argv[])
{	
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	using namespace std;
	bool isProducer = false;  //program is a producer or consumer

	if (argc < 6)	//handle arguments
	{
		showUsage();
	}

							 

	if (strcmp("producer", argv[1]) == 0)
	{
		SetConsoleTitle(L"Producer");
		isProducer = true;
	}
	else if (strcmp("consumer", argv[1]) == 0)
	{
		SetConsoleTitle(L"Producer");
		isProducer = false;
	}
	else { showUsage(); }

	
	size_t delay = atoi(argv[2]);				  //delay in milliseconds
	size_t bufferSize = atoi(argv[3]) * 1 << 20; //input is in megabite so turn into bytes
	unsigned int numMessages= atoi(argv[4]);	//How many messages do you wish to send?
	size_t MsgSize = atoi(argv[5]);			   //And how big should the messages be
	size_t chunkSize = 256;					  //Padding. It is a thing
	

	SetConsoleTextAttribute(consoleHandle, 11);
	cout << "IsProducer?: " << isProducer << endl;
	cout << "delay:" << delay << endl;
	cout << "BufferSize: " << bufferSize << endl;
	cout << "Amount of messages: " << numMessages << endl;
	cout << "Chunk size: " << chunkSize << endl << endl;
	SetConsoleTextAttribute(consoleHandle, 15);
	
	
	
	CircBufferFixed* CircleBuffer = new CircBufferFixed(L"Buffer",bufferSize,isProducer,chunkSize);

	CircleBuffer->createMaping();
	
	
	if(isProducer == true)
	{
		for (size_t i = 1; i <= numMessages; i++)
		{

			this_thread::sleep_for(std::chrono::milliseconds(delay)); 
			

			char* message= new char[MsgSize]();
			gen_random(message, MsgSize);

			size_t HeaderSize = sizeof(Header);
			if (CircleBuffer->freeMemory > (MsgSize + sizeof(Header)))
			{
				CircleBuffer->push(message, MsgSize);
				CircleBuffer->freeMemory -= (MsgSize + sizeof(Header));


		
				SetConsoleTextAttribute(consoleHandle, 14);
				cout << "Message " << i;
				cout << ":  " << message << endl << endl;
				SetConsoleTextAttribute(consoleHandle, 15);
				cout << "FreeMemory= " << CircleBuffer->freeMemory << endl << endl << endl;

			}

			else //if there is no memory;
			{
				cout << "OUT OF MEMORY" << endl;
				getchar(); //remove this and you get error
				CircleBuffer->push(message, MsgSize);
				CircleBuffer->freeMemory -= MsgSize + sizeof(Header);

				cout << "Message ID " << i;
				cout << ":  " << message << endl << endl;
				SetConsoleTextAttribute(consoleHandle, 15);
				cout << "FreeMemory= " << CircleBuffer->freeMemory << endl << endl << endl;

				getchar();
			}
			
		}
	
	}
	else if(isProducer == false)
	{
		
		Control Controller = CircleBuffer->headTails();

		cout << "head = " << Controller.header << endl << "Tail = " << Controller.Tail;
		getchar();
		Header* header = new Header();
		for (size_t i = 1; i <= numMessages; i++)
		{
			this_thread::sleep_for(std::chrono::milliseconds(delay));

			CircleBuffer->read(header, header->length);
			
		}
		delete header;
			
	}
		
	getchar();
	delete CircleBuffer;
}

