
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
	size_t size = strlen(argv[1]) + 1;
	wchar_t* wtext = new wchar_t[size];
	size_t outSize;
	mbstowcs_s(&outSize, wtext, size, argv[1], size - 1);
	
#pragma endregion 

	LPCWSTR buffName = wtext;
	size_t buffSize = atoi(argv[2]);
	bool isProducer = atoi(argv[3]);
	size_t chunkSize = atoi(argv[4]);
	//Unsigned float delay
	//unsigned int numMessages

	if (printToConsole = true)
	{
		SetConsoleTextAttribute(hConsole, 11);
		cout << "ArgumentCount: " << argc << endl;
		wcout << "Name: " << buffName << endl;
		cout << "BufferSize: " << buffSize << endl;
		cout << "is the buffer a Producer?: " << isProducer << endl;
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
				this_thread::sleep_for(std::chrono::seconds(1));
				
			}
		}
	}
	
	else
	{
		if (CircleBuffer->createMapingConsumer() && printToConsole == true)
		{
			;
		}
	}



	//getchar();
}