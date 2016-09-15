#include <windows.h>
#include <iostream>
using namespace std;

struct Header
{
	size_t id;
	size_t length;
	size_t padding;
	// maybe number of consumers here?
};
struct Control
{
	size_t header;
	size_t Tail;
};

class CircBufferFixed
{
	
private:
	LPCWSTR buffName;

	size_t buffSize;
	size_t MessageCount;
	size_t chunkSize;

	bool isProducer;
	HANDLE MapingFile, ControlFile;
	HANDLE consoleHandle; //used for consol text color
	size_t head;

	
public:
	long freeMemory;
	char* MapPointer;
	size_t* ControlPointer;
	Control headTails();
	

	CircBufferFixed( // Constructor
            LPCWSTR buffName,          // unique name
            const size_t& buffSize,    // size of the whole filemap
            const bool& isProducer,    // is this buffer going to be used as producer
            const size_t& chunkSize);  // round up messages to multiple of this.

    
	~CircBufferFixed(); // Destructor

	bool createMaping(); //returns true if success
	bool read(const void* msg, size_t length);

    // try to send a message through the buffer,
    // if returns true, then it succeeded, otherwise the message has not been sent.
    // it should return false if the buffer does not have enough space.
	bool push(const void* msg, size_t length);


	
    // try to read a message from the buffer, and the implementation puts the content
    // in the memory. The memory is expected to be allocated by the program that calls
    // this function.
    // The variable length will be changed to reflect the size of the msg just read.
	bool pop(char* msg, size_t& length);
};

