
#include "stdio.h"
#include <iostream>
#include "CircBuffer.h"

int main(size_t argc, char* argv[])
{	
	using namespace std;

	bool write = true;
	
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

	if (write = true)
	{
		cout << "ArgumentCount: " << argc << endl;
		wcout << "Name: " << buffName << endl;
		cout << "BufferSize: " << buffSize << endl;
		cout << "is the buffer a Producer?: " << isProducer << endl;
		cout << "Chunk size: " << chunkSize << endl;
	}
	
	

	CircBufferFixed* CircleBuffer = new CircBufferFixed(buffName,buffSize,isProducer,chunkSize);

	

	getchar();
}