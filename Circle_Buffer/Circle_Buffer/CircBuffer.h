#include "stdio.h"
#include <windows.h>
#include <iostream>
#include <chrono> 
#include <thread> 
#include "Mutex.h"


using namespace std;

//this Class handles the
class CircBufferFixed
{
	enum { HEAD, TAIL, CLIENTCOUNT };
	struct Header // Header for messages in circularBuffer. Allows clients to the the Circular Buffer to know the size of the next message. 
	{
		size_t id;
		size_t length;
		size_t padding;
		size_t ClientRemaining;
	};

	struct Control // Struct used in the control buffer to allow communication between multiple instances of the circular buffer. Such as clients to the producer and vice versa.
	{
		size_t head;
		size_t Tail;
		size_t clientCount;
	};

	
private:
	
	Mutex mutex;// Mutex is used to make sure the application does not edit or remove memory whilst that memory is being read.
	LPCWSTR buffName;
	size_t buffSize;
	size_t MessageCount; 
	size_t chunkSize;
	bool isProducer;
	HANDLE MapingFile, ControlFile; //used in function 'createMaping()' in order to create the shared memory for the circular buffer.
	size_t positionInBuffer;
	size_t numberOfMessages; //used for testing the circular buffer. After this amount of messages have been read/pushed the application exits. 
	char* MapPointer;
	size_t* ControlPointer;
	bool createMaping(); // Creates a shared memory for the Producers and consumers to communicate. returns true if successful.
	void UpdateControlBuffer();
	
public:
	
	CircBufferFixed(					   // Constructor
		LPCWSTR buffName,			       // Name of the buffer
		const bool& isProducer,			   // is this buffer going to be used as producer
		const size_t& buffSize,			   // size of the whole filemap/Circular buffer
		const size_t& chunkSize);		   // round up messages to multiple of the varaibles value.

	~CircBufferFixed(); // Destructor

	size_t CalculateFreeMemory(); //returns how much memory is left in the Circle buffer

	// Used by the producer version of the class.
    // Sends a message through the buffer.
    // Returns true upon success or false if the message has not been sent.
	bool push(const char* msg, size_t length);


	// Used by the client version of the class.
    // Read a message from the Circular buffer. Returns true if a message has been read.
	// The message is saved in the message parameter.
	bool pop(char* message);
};

