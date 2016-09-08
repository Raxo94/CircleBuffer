
#include "stdio.h"
#include <iostream>
#include "CircBuffer.h"
#include <thread> 
#include <chrono>   

//The producer will print(for example) :
//	1 asdfqwe
//	2 lkjasoiueroisuer
//	The consumer HAS to print
//	1 asdfqwe
//	2 lkjasoiueroisuer


int main(size_t argc, char* argv[]) //remember to run as administrator 
{	
	using namespace std;

	bool printToConsole = true;
	bool done = false;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

#pragma region getName  
	/*size_t size = strlen(argv[1]) + 1;
	wchar_t* wtext = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, wtext, size, argv[1], size - 1);
	LPCWSTR buffName = wtext;*/
#pragma endregion 

	
	LPCWSTR buffName = TEXT ("CircleBuffer");

	bool isProducer = atoi(argv[1]);
	float delay = atoi(argv[2]);
	size_t buffSize = atoi(argv[3]);
	unsigned int numMessages= atoi(argv[4]);
	size_t MsgSize = atoi(argv[5]);
	size_t chunkSize = 256;
	

	if (printToConsole = true)
	{
		SetConsoleTextAttribute(hConsole, 11);
		cout << "ArgumentCount: " << argc << endl;
		wcout << "Name: " << buffName << endl;
		cout << "IsProducer? 1=Yes 0=No: " << isProducer << endl;
		cout << "delay:" << delay;
		cout << "BufferSize: " << buffSize << endl;
		cout << "Amount of messages: " << numMessages << endl;
		cout << "Chunk size: " << chunkSize << endl << endl;
		SetConsoleTextAttribute(hConsole, 15);
	}
	
	
	CircBufferFixed* CircleBuffer = new CircBufferFixed(buffName,buffSize,isProducer,chunkSize);

	
	
	
	if(isProducer == true)
	{
		if (CircleBuffer->createMapingProducer() && printToConsole == true) // CreateSharedMaping
		{
			cout << "Circle Buffer Created Sucessfully" << endl;
			while (done != true)
			{
				//this_thread::sleep_for(std::chrono::seconds(5));
				string message = "HELLO";
				size_t messageLength = message.length();
				CircleBuffer->push(&message, messageLength);
				getchar();
				done = true;

				/*int megabytes = 1;

				megabytes * 1 << 40;

				1024 * 10^2 = */
			}
		}
	}
	
	else
	{
		if (CircleBuffer->createMapingConsumer() && printToConsole == true)
		{
			cout << "Circle Buffer Created Sucessfully" << endl;
			while (done != true)
			{
				//this_thread::sleep_for(std::chrono::seconds(5));


				string message = "HELLO"; //Not Used Yet
				size_t messageLength = message.length(); //Not Used Yet
				CircleBuffer->read(&message,messageLength);
				getchar();
				done = true;
			}
		}
	}



	getchar();

	delete CircleBuffer;
}