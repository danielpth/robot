#include "Speak.h"

Speak::Speak() {

}
int Speak::Talk(string sentence)
{
	return Talk(sentence, 150, 10);
}

int Speak::Talk(string sentence, int speed, int word_gap)
{
	int rc;
	string command;
	command += "espeak \"";
	command += sentence;
	//command += "\" --stdout -vbrazil | aplay -D 'default' &";
	command += "\" -p20 -s" + to_string(speed) + " -g" + to_string(word_gap) + " --stdout | aplay -D 'default' ";

	rc = system(command.c_str());
	if (rc != 0) {
		printf("%s system fail: rc=[%d] errno=[%d]\n", __FUNCTION__, rc, errno);
	}
	return rc;

}
