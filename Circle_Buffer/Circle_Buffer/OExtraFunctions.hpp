#pragma once
#include <iostream>
using namespace std;

inline void showUsage() //called if program is being used incorrect
{
	std::cout << "Usage: program [producer|consumer] delay buffSize numMsg random|msgSize" << std::endl;
	getchar();
	exit(0);
}

inline void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (auto i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	s[len] = 0;
}