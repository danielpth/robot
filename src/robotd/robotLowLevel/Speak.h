#pragma once
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <string.h>
#include <errno.h>
#include <thread>
#include <string>
#include <string.h>

using namespace std;

class Speak
{
private:

public:

	Speak();
	int Talk(string sentence);
	int Talk(string sentence, int speed, int word_gap);

};

