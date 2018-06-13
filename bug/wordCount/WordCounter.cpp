#include "WordCounter.h"
#include<cctype>
#include<windows.h>
WordCounter::WordCounter(){
	currentFile = nullptr;
}


WordCounter::~WordCounter()
{	
	if (currentFile) {
		fclose(currentFile);
		currentFile = nullptr;
	}
}

bool WordCounter::SetCurrentFile(const wchar_t* fileName) {
	if (currentFile) 
		fclose(currentFile);
	currentFile = 0;
	_wfopen_s(&currentFile, fileName, L"rb");
	for (int i = lstrlenW(fileName); i >= 0; i--) {
		if (fileName[i] == '.') {
			if (lstrcmpW(fileName + i + 1, L"cpp") == 0||
				lstrcmpW(fileName + i + 1, L"cxx") == 0 ||
				lstrcmpW(fileName + i + 1, L"c") == 0 ||
				lstrcmpW(fileName + i + 1, L"h") == 0 ||
				lstrcmpW(fileName + i + 1, L"hpp") == 0 ||
				lstrcmpW(fileName + i + 1, L"java") == 0 ||
				lstrcmpW(fileName + i + 1, L"js") == 0||
				lstrcmpW(fileName + i + 1, L"cs") == 0||
				lstrcmpW(fileName + i + 1, L"jsp") == 0
				) {
				mode = ModeCpp;
			}
			
		}
	}
	return currentFile;
}

int WordCounter::CountChar() {
	char buf[30];
	int len = 0;
	int SIZE = 20;
	int charCount = 0;
	do {
		len = fread(buf, 1, SIZE, currentFile);
		charCount += len;
	} while (len > 0);
	return charCount;
}

AllCount WordCounter::CountAll() {
	switch (mode) {

	default:
		return CountCpp();
	}
}

AllCount WordCounter::CountCpp() {
	char buf[30] = { 0 };
	int len = 0;
	int SIZE = 1;
	AllCount cnt = { 0 };
	int currentCharCount = 0;
	bool hasAlpha = false;
	char prevChar = 0;
	enum Mode {
		none,
		note,
		multiNote
	}mode = none;
	do {
		len = fread(buf, 1, SIZE, currentFile);
		if (len) {
			if (iswalnum(buf[0])) {
				hasAlpha = true;
			}
			else if(iswspace(buf[0])){
				if (hasAlpha) {
					cnt.cnt[1]++;
					hasAlpha = false;
				}
			}
			else {
				if (hasAlpha) {
					cnt.cnt[1] += 2;
					hasAlpha = false;
				}
				else
					cnt.cnt[1]++;
			}
			cnt.cnt[0]++;
			if (mode == multiNote) {
				if (prevChar == '*'&&buf[0] == '/') {
					mode = none;
				}
				else if (buf[0] == '\n') {
					if (currentCharCount > 1)
						cnt.cnt[3]++;
					else
						cnt.cnt[4]++;
					currentCharCount = 0;
					cnt.cnt[5]++;
					cnt.cnt[2]++;
				}
				else {
					prevChar = buf[0];
				}
			}
			else if (mode == note) {
				if (buf[0] == '\n') {
					cnt.cnt[2]++;
					mode = none;
					if (currentCharCount > 1)
						cnt.cnt[3]++;
					else
						cnt.cnt[4]++;
					currentCharCount = 0;
				}
			}
			else switch (buf[0]) {
			case '\n':
				if (currentCharCount > 1)
					cnt.cnt[3]++;
				else
					cnt.cnt[4]++;
				currentCharCount = 0;
				cnt.cnt[2]++;
				break;
			case '/':
				if (prevChar == '/') {
					mode = note;
					cnt.cnt[5]++;
					currentCharCount--;
				}
				else {
					currentCharCount++;
				}
				break;
			case '*':
				if (prevChar == '/') {
					mode = multiNote;
					cnt.cnt[5]++;
					currentCharCount--;
				}
				else {
					currentCharCount++;
				}

			case '\r':
				break;
			default:
				if (!iswspace(buf[0])) {
					currentCharCount++;
				}
				break;


			}

			prevChar = buf[0];
		}

	} while (len > 0);
	cnt.cnt[2]++;
	if (buf[0]) {
		if (currentCharCount > 1) {
			cnt.cnt[3]++;
			
		}
		else {
			cnt.cnt[4]++;
		}
	}

	return cnt;
}

Count3 WordCounter::CountLineExtra() {
	char buf[30];
	int len = 0;
	int SIZE = 20;
	int counts[3] = { 0 };
	int currentCharCount = 0;
	char prevChar=0;
	enum Mode {
		none,
		note,
		multiNote
	}mode = none;
	do {
		len = fread(buf, 1, SIZE, currentFile);
		for (int i = 0; i != len; i++) {
			if (mode == multiNote) {
				if (prevChar == '*'&&buf[i]=='/') {
					mode = none;
					counts[2]++;
				}
				else {
					prevChar = buf[i];
				}
			}
			else if(mode==note){
				if (buf[i] == '\n') {
					mode = none;
					counts[2]++;
				}
			}
			else {
				switch (buf[i]) {
				case '\n':
					if (currentCharCount>1) {
						counts[1]++;
						currentCharCount = 0;
					}
					else 
						counts[0]++;
					
					prevChar = 0;
					break;
				case '/':
					if (prevChar == '/') {
						mode = note;
						if (currentCharCount > 1) {
							counts[1]++;
						}
						else{
							counts[0]++;
						}
						currentCharCount = 0;
					}
					else {
						prevChar = '/';
					}
					break;
				case '*':
					if (prevChar == '/') {
						mode = multiNote;
						if (currentCharCount > 1) {
							counts[1]++;
						}
						else {
							counts[0]++;
						}
						currentCharCount = 0;
					}
					else {
						prevChar = '*';
					}
				case '\r':
					break;
				default:
					if (!iswspace(buf[i])) {
						currentCharCount++;
					}
					prevChar = 0;
					break;


				}
			}
		}
	} while (len > 0);
	switch (mode) {
	case multiNote:
	case note:
		counts[2]++;
		break;
	case none:
		if (currentCharCount > 1) {
			counts[1]++;
		}
		else {
			counts[0]++;
		}
		break;
	}
	Count3 c = { counts[0],counts[1],counts[2] };
	return c;
}

int WordCounter::CountWord() {
	int wordCount=0;
	bool hasWord=false;
	char buf[30];
	int len = 0;
	int SIZE = 20;
	do {
		len = fread(buf, 1, SIZE, currentFile);
		for (int i = 0; i != len; i++) {
			if (iswspace(buf[i])) {
				if (hasWord) {
					hasWord = false;
					wordCount++;
				}
			}
			else if (!hasWord)hasWord = true;
		}
	} while (len > 0);
	if (hasWord)wordCount++;
	return wordCount;
}
int WordCounter::CountLine() {
	char buf[30];
	int len = 0;
	int SIZE = 20;
	int lineCount=0;
	do {
		len = fread(buf, 1, SIZE, currentFile);
		for (int i = 0; i != len; i++) {
			if (buf[i] == '\n')
				lineCount++;
		}
	} while (len > 0);
	return lineCount;
}