#define _CRTDBG_MAP_ALLOC
#include <iostream>
#include "CircBuffer.h"
#include <time.h>
#include <windows.h>

void showUsage(); //called if program is being used incorrect
void gen_randomString(char * s, const int len);
size_t randomSize(size_t min, size_t max);

HANDLE ghMutex;

int main(size_t argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); // detect memory leaks

	ghMutex = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex

	if (ghMutex == NULL)
	{
		printf("CreateMutex error: %d\n", GetLastError());
		return 1;
	}


	if (argc < 6) { showUsage(); }
	using namespace std;
	srand((unsigned)time(NULL));

	bool isProducer = false;  //program is a producer or consumer
	bool random = false;	 //is the message size random
	size_t MsgSize = 0;     //how big should the messages be

	if (strcmp("producer", argv[1]) == 0)
	{	SetConsoleTitle(L"Producer"); isProducer = true;}
	else if (strcmp("consumer", argv[1]) == 0)
	{	SetConsoleTitle(L"Consumer");
		isProducer = false;}

	else { showUsage(); }

	if (strcmp("random", argv[5]) == 0)
	{	random = true; }

	else{MsgSize = atoi(argv[5]);}
	

	
	size_t delay = atoi(argv[2]);				  //delay in milliseconds
	size_t bufferSize = atoi(argv[3]) * 1 << 20; //input is in megabite so turn into bytes
	unsigned int numMessages= atoi(argv[4]);	//How many messages do you wish to send?
	size_t chunkSize = 256;					  //Padding. It is a thing
	size_t maxMessageSize = bufferSize / 4;

	CircBufferFixed* CircleBuffer = new CircBufferFixed(L"Buffer", isProducer,bufferSize,chunkSize);
	
	if(isProducer)
	{
		char* message;
		for (size_t i = 1; i <= numMessages; i++)
		{

			Sleep(delay);
			if (random)
			{
				size_t size = randomSize(1, maxMessageSize);
				message = new char[size]();
				gen_randomString(message, size);
				while (CircleBuffer->push(message, size) == false)
				{
					Sleep(1);
					delete[] message; //Delete and make a new message
					size = randomSize(1, maxMessageSize);
					message = new char[size]();
					gen_randomString(message, size);
				}
				delete[] message; //Delete message
			}

			else // if not random
			{
				message = new char[MsgSize]();
				gen_randomString(message, MsgSize);

				while (CircleBuffer->push(message, MsgSize) == false)
				{
					Sleep(1);
					//No need to generate a new message;
				}
				delete[] message; //delete message
			}
				
		}
	}
	else
	{
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			char* message = new char[maxMessageSize];
			while (CircleBuffer->pop(message, MsgSize) == false)
			{
				Sleep(1);
			}
			delete[] message; //delete message
		}
	}
	//getchar()
	delete CircleBuffer;
}







void showUsage() //called if program is being used incorrect
{
	std::cout << "Usage: program [producer|consumer] delay buffSize numMsg random|msgSize" << std::endl;
	//getchar();
	exit(0);
}


void gen_randomString(char * s, const int len)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (auto i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	s[len - 1] = 0;
}

size_t randomSize(size_t min, size_t max)
{
	if (max - min != 0)
	{
		size_t result = rand() % ((max +1) - min);
		result += min;
		return result;
	}
	else return 1;

}
