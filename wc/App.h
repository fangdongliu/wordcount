#pragma once
#include"WordCounter.h"
#include<string>
enum Mode {
	ModeDefault,
	ModeChar,
	ModeLine,
	ModeWord,
	ModeExtra,
};
class App
{
public:
	WordCounter counter;
	Mode mode;
	char *fileName;
	void HandleCommand(int argc, char**argv);
	void CountFiles(std::string fileName);

};

