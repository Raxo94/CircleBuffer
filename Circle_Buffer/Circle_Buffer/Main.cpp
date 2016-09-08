
#include "stdio.h"
#include <iostream>
#include "CircBuffer.h"

//The producer will print(for example) :
//	1 asdfqwe
//	2 lkjasoiueroisuer
//	The consumer HAS to print
//	1 asdfqwe
//	2 lkjasoiueroisuer

int main(size_t argc, char* argv[]) //remember to run as administrator 
{	
	using namespace std;

	bool write = true;
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

	if (write = true)
	{
		SetConsoleTextAttribute(hConsole, 11);
		cout << "ArgumentCount: " << argc << endl;
		wcout << "Name: " << buffName << endl;
		cout << "BufferSize: " << buffSize << endl;
		cout << "is the buffer a Producer?: " << isProducer << endl;
		cout << "Chunk size: " << chunkSize << endl << endl;
	}
	
	

	CircBufferFixed* CircleBuffer = new CircBufferFixed(buffName,buffSize,isProducer,chunkSize);


	if (CircleBuffer->createBuffer()&& write==true) // CreateSharedMaping
	{
		SetConsoleTextAttribute(hConsole, 15);
		cout << "Circle Buffer Created Sucessfully" << endl;
	}




	getchar();
}