#pragma once
#include<stdio.h>

struct Count3 {
	int cnt0, cnt1, cnt2;
};
struct AllCount {
	int cnt[6];
};
enum CountMode {
	ModeCpp,
	
};
class WordCounter
{
	FILE *currentFile;
public:
	WordCounter();
	~WordCounter();
	int CountChar();
	AllCount CountAll();
	
	int CountWord();
	int CountLine();
	Count3 CountLineExtra();
	bool SetCurrentFile(const wchar_t*fileName);
private:
	CountMode mode;
	AllCount CountCpp();
};

