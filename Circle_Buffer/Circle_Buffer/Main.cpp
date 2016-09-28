#define _CRTDBG_MAP_ALLOC
#include <iostream>
#include "CircBuffer.h"
#include <time.h>
#include <windows.h>
#include "functions.cpp"
using namespace std;

//The code works consistantly. though sadly it takes longer than it should.
//Test that can be completed in 60 secounds take roughly 150 secounds. 
//I dont know why. The code also may get a memory leak being the allocated message memory for the consumer.
int main(size_t argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // detect memory leaks
	
	if (argc < 6) { showUsage(); } // if too many arguments exit
	srand((unsigned)time(NULL));   // make sure random works



	bool isProducer = GetIsProducer(argv[1]);  //program is a producer or consumer
	size_t MsgSize = 0;					  //how big should the messages be
	bool random = GetIsRandom(argv[5]);	  //is the message size random
	if (!random) MsgSize = atoi(argv[5]); //if not set size to size_t


	DWORD delay = atoi(argv[2]);				  //delay in milliseconds
	size_t bufferSize = atoi(argv[3]) * 1 << 20; //input is in megabite so turn into bytes
	unsigned int numMessages= atoi(argv[4]);	//How many messages do you wish to send?
	size_t chunkSize = 256;					  //Padding. It is a thing
	size_t maxMessageSize = bufferSize / 4;

	CircBufferFixed CircleBuffer (L"Buffer", isProducer, bufferSize, chunkSize);

	char* message;
	size_t size;
	
	if(isProducer)
	{
	
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			if (random)
			{
				size = randomSize(1, maxMessageSize);
				message = new char[size]();
				gen_randomString(message, size);

				while (CircleBuffer.push(message, size) == false)
				{	
					Sleep(1);         
					delete[] message; //Delete and make a new message
					size = randomSize(1, maxMessageSize);
					message = new char[size]();
					gen_randomString(message, size);
				}
				
			}
			else // IF NOT RANDOM	
			{
				message = new char[MsgSize]();
				gen_randomString(message, MsgSize);

				while (CircleBuffer.push(message, MsgSize) == false)
					Sleep(1);
				delete[] message; 
			}
				
		}
	}

	else //IF CLIENT
	{
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			message = new char[maxMessageSize];
			while (CircleBuffer.pop(message, MsgSize) == false)
			{
				Sleep(1);
			}
			delete[] message;
			
		}
		
	}
	//getchar()
}







