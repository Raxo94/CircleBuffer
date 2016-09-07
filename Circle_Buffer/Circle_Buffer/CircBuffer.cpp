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

bool CircBufferFixed::push(const void * msg, size_t length)
{
	return false;
}

bool CircBufferFixed::pop(char * msg, size_t & length)
{
	return false;
}
