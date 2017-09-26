#include <iostream>
#include "CircBuffer.h"
#include <time.h>
#include <windows.h>
#include "functions.cpp"
using namespace std;

//This CircularBuffer code was developed to work with Francisco Lopez Luro's test1414.py, and was written by Oskar Swing in his third year at Blekinge Institute of Technology.
//The CircularBuffer can be initiated by creating a producer and a consumer instance of the program. An easy way to do so is by using a '.bat' file.  
//The CircularBuffer can also be initiated by executing 'test1414.py' in the same folder as a built 'Circular_Buffer.exe' file.


//'test1414.py' is a test application that launches the circular buffer several times with different delay times and amount of clients. This is done in order to make sure the Circular buffer works as intended.
// The messages that are sent into the buffer are randomly generated strings. 'test1414.py' compares the strings that the producer sends and the strings the client recieves. If they are the same the test passes.  
// The first Circular buffer tests with 'test1414.py' should take about 60 secounds. The time does however vary depending on the computer. An issue with the sleep function has been observed on some computers causing the tests to take around 150 seconds.

// Main function takes five arguments: boolian isProducer, size_t bufferSize, sizeT delay, unsigned int numMessages, boolian isRandomSized. 
//These argument are used to augment the characteristics of the Circular buffer.
int main(size_t argc, char* argv[])
{
	
	if (argc < 6) { showUsage(); } // if too few arguments. Exit program.
	srand((unsigned)time(NULL));   // initate random numbers.


	bool isProducer = GetIsProducer(argv[1]);  // Is this program the producer or a consumer.
	size_t MsgSize = 0;					  //Size of messages.
	bool random = GetIsRandom(argv[5]);	  //is the message size random?
	if (!random) MsgSize = atoi(argv[5]); //if message size is not set to random. Get message size.


	DWORD delay = atoi(argv[2]);				  //delay between messages, in milliseconds.
	size_t bufferSize = atoi(argv[3]) * 1 << 20; //input is in megabite and is translated into bytes.
	unsigned int numMessages= atoi(argv[4]);	//How many messages do you wish to send?
	size_t chunkSize = 256;					  //Padding.
	size_t maxMessageSize = bufferSize / 4; 

	CircBuffer CircleBuffer (L"Buffer", isProducer, bufferSize, chunkSize); //Circle Buffer is initiated using the input values.

	char* message= new char[maxMessageSize](); //alocate memory to allow for messages of the maxMessageSize.
	size_t size; // Represent the size of the current message. Used when message size is set to random.
	
	if(isProducer)
	{
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			if (random)
			{
				size = randomSize(1, maxMessageSize);
				gen_randomString(message, size);

				while (CircleBuffer.push(message, size) == false) //if memory is full, wait and try again.
					Sleep(1);         
			}

			else // IF NOT RANDOM	
			{
				gen_randomString(message, MsgSize);

				while (CircleBuffer.push(message, MsgSize) == false) //if memory is full, wait and try again.
					Sleep(1);
			}	
		}
	}//END OF PRODUCER


	else //IF CLIENT
	{
		for (size_t i = 1; i <= numMessages; i++)
		{
			Sleep(delay);
			while (CircleBuffer.pop(message) == false) //if latest message is already read, wait and try again.
			{
				Sleep(1);
			}
		}		
	}//END OF CLIENT


	delete[] message;
	
}//ENDOFCODE







