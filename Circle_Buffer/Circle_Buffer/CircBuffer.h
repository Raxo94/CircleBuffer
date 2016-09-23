#include "stdio.h"
#include <windows.h>
#include <iostream>
#include <chrono> 
#include <thread> 
#include "Mutex.h"

using namespace std;

struct Header
{
	size_t id;
	size_t length;
	size_t padding;
	size_t ClientRemaining;
};

struct Control
{
	size_t head;
	size_t Tail;
	size_t clientCount;
};
class CircBufferFixed
{
	

	enum { HEAD, TAIL, CLIENTCOUNT };
	

private:
	Mutex mutex;
	LPCWSTR buffName;
	size_t buffSize;
	size_t MessageCount;
	size_t chunkSize;
	bool isProducer;
	HANDLE MapingFile, ControlFile;
	HANDLE consoleHandle; //used for consol text color
	size_t ClientPosition;
	size_t numberOfMessages;
	size_t freeMemory;
	char* MapPointer;
	size_t* ControlPointer;
	bool createMaping(); //returns true if success
	void UpdateControlBuffer();
	
public:
	
	CircBufferFixed(						   // Constructor
		LPCWSTR buffName,			       // unique name
		const bool& isProducer,			   // is this buffer going to be used as producer
		const size_t& buffSize,			   // size of the whole filemap
		const size_t& chunkSize);		   // round up messages to multiple of this.
		

    
	~CircBufferFixed(); // Destructor

	size_t CalculateFreeMemory();

    // try to send a message through the buffer,
    // if returns true, then it succeeded, otherwise the message has not been sent.
    // it should return false if the buffer does not have enough space.
	bool push(const void* msg, size_t length);


    // try to read a message from the buffer, and the implementation puts the content
    // in the memory. The memory is expected to be allocated by the program that calls
    // this function.
    // The variable length will be changed to reflect the size of the msg just read.
	bool pop(char* message, size_t& length);
};

