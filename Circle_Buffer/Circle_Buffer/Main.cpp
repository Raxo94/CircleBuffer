
#include <iostream>
#include "CircBuffer.h"


void showUsage(); //called if program is being used incorrect
void gen_random(char * s, const int len);


int main(size_t argc, char* argv[])
{	
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	using namespace std;
	bool isProducer = false;  //program is a producer or consumer

	if (argc < 6) { showUsage(); }

				
	if (strcmp("producer", argv[1]) == 0)
	{
		SetConsoleTitle(L"Producer");
		isProducer = true;
	}
	else if (strcmp("consumer", argv[1]) == 0)
	{
		SetConsoleTitle(L"Consumer");
		isProducer = false;
	}
	else { showUsage(); }

	
	size_t delay = atoi(argv[2]);				  //delay in milliseconds
	size_t bufferSize = atoi(argv[3]) * 1 << 20; //input is in megabite so turn into bytes
	unsigned int numMessages= atoi(argv[4]);	//How many messages do you wish to send?
	size_t MsgSize = atoi(argv[5]);			   //And how big should the messages be
	size_t chunkSize = 256;					  //Padding. It is a thing
	

	SetConsoleTextAttribute(consoleHandle, 15);

	CircBufferFixed* CircleBuffer = new CircBufferFixed(L"Buffer", isProducer,bufferSize,chunkSize);
	
	if(isProducer)
	{
		bool result;
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			char* message = new char[MsgSize]();
			gen_random(message, MsgSize);

			while (CircleBuffer->push(message, MsgSize) == false)
			{
				Sleep(delay);
			}
				
		}
	}
	else
	{
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			char* message = new char[bufferSize/4]();
			while (CircleBuffer->pop(message, MsgSize) == false)
			{
				Sleep(delay);
			}
		}
	}
	
	delete CircleBuffer;
}




void showUsage() //called if program is being used incorrect
{
	std::cout << "Usage: program [producer|consumer] delay buffSize numMsg random|msgSize" << std::endl;
	getchar();
	exit(0);
}


void gen_random(char * s, const int len)
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

