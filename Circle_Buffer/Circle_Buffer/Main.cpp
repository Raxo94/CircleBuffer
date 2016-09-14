
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
	bool printToConsole = true;
	bool done = false;

	if (argc < 6)										     //handle arguments
	{
		showUsage();
	}

	bool isProducer = false;							 //program is a producer or consumer
	if (strcmp("producer", argv[1]) == 0)
		isProducer = true;
	else if (strcmp("consumer", argv[1]) == 0)
		isProducer = false;
	else { showUsage(); }

												   
	size_t delay = atoi(argv[2]);				  //delay in milliseconds
	size_t bufferSize = atoi(argv[3]) * 1 << 20; //input is in megabite so turn into bytes
	unsigned int numMessages= atoi(argv[4]);	//How many messages do you wish to send?
	size_t MsgSize = atoi(argv[5]);			   //And how big should the messages be
	size_t chunkSize = 256;					  //Padding. It is a thing
	

	if (printToConsole = true)
	{
		SetConsoleTextAttribute(consoleHandle, 11);
		cout << "IsProducer?: " << isProducer << endl;
		cout << "delay:" << delay << endl;
		cout << "BufferSize: " << bufferSize << endl;
		cout << "Amount of messages: " << numMessages << endl;
		cout << "Chunk size: " << chunkSize << endl << endl;
		SetConsoleTextAttribute(consoleHandle, 15);
	}
	
	
	CircBufferFixed* CircleBuffer = new CircBufferFixed(L"Buffer",bufferSize,isProducer,chunkSize);
	CircleBuffer->createMaping();
	
	
	
	if(isProducer == true)
	{
		
		SetConsoleTextAttribute(consoleHandle, 10);
		cout << "Circle Buffer Created Successfully" << endl << endl;
		SetConsoleTextAttribute(consoleHandle, 14);

		for (size_t i = 1; i <= /*numMessages*/10000; i++)
		{

			if (delay != 0) 
			{ this_thread::sleep_for(std::chrono::milliseconds(delay)); }
			
			char* message= new char[MsgSize]();
			gen_random(message, MsgSize);

			if (CircleBuffer->freeMemory > MsgSize)
			{
				CircleBuffer->freeMemory -= MsgSize;

				CircleBuffer->push(message, MsgSize);
				if (printToConsole)
					cout << "Message " << i << ":  " << message << endl << endl << "FreeMemory= " << CircleBuffer->freeMemory << endl << endl <<endl;
			}
			
			
		}
	
	}
	
	else
	{
		SetConsoleTextAttribute(consoleHandle, 10);
		cout << "Circle Buffer Created Successfully" << endl << endl;
		while (done != true)
		{
			if (delay != 0) 
			{ this_thread::sleep_for(std::chrono::milliseconds(delay)); }
				
			//maybe read header first
			Header* header= new Header();
			CircleBuffer->read(header,header->length);
			
			done = true;
		}
		
	}



	getchar();

	delete CircleBuffer;
}

