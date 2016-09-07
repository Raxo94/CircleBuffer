#include "CircBuffer.h"

CircBufferFixed::CircBufferFixed(LPCWSTR buffName, const size_t & buffSize, const bool & isProducer, const size_t & chunkSize)
{
	this->buffName = buffName;
	this->buffSize = buffSize;
	this->isProducer = isProducer;
	this->chunkSize = chunkSize;

}

CircBufferFixed::~CircBufferFixed()
{
}

void CircBufferFixed::createBuffer()
{

	HANDLE WINAPI CreateFile(
		_In_     LPCTSTR               lpFileName,
		_In_     DWORD                 dwDesiredAccess,
		_In_     DWORD                 dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		_In_     DWORD                 dwCreationDisposition,
		_In_     DWORD                 dwFlagsAndAttributes,
		_In_opt_ HANDLE                hTemplateFile
	);

}

bool CircBufferFixed::push(const void * msg, size_t length)
{
	return false;
}

bool CircBufferFixed::pop(char * msg, size_t & length)
{
	return false;
}
