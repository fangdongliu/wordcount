#pragma once
#include<stdio.h>

struct AllCount {
	int cnt[6];
};
enum CountMode {
	ModeCpp,
	ModePhp,
	ModePython
};
class WordCounter
{
	FILE *currentFile;
public:
	WordCounter();
	~WordCounter();
	AllCount CountAll();
	
	bool SetCurrentFile(const wchar_t*fileName);
private:
	CountMode mode;
	AllCount CountCpp();
	AllCount CountPhp();
	AllCount CountPython();
};

