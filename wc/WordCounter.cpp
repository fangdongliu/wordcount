#include "WordCounter.h"
#include<cctype>
#include<windows.h>
#include<unordered_map>
using namespace std;

namespace {
	unordered_map<string, CountMode>modeMap = {
		{ "cpp",ModeCpp },{ "cxx",ModeCpp },{ "c",ModeCpp },{ "h",ModeCpp },{ "hpp",ModeCpp },{ "java",ModeCpp },{ "js",ModeCpp },{ "jsp",ModeCpp },{ "cs",ModeCpp },
	{ "php",ModePhp },
	{ "py",ModePython }

	};
}

WordCounter::WordCounter() {
	currentFile = nullptr;
}


WordCounter::~WordCounter()
{
	if (currentFile) {
		fclose(currentFile);
		currentFile = nullptr;
	}
}

bool WordCounter::SetCurrentFile(const char* fileName) {
	if (currentFile)
		fclose(currentFile);
	currentFile = 0;
	if (fopen_s(&currentFile, fileName, "rb")){
		printf("cannot open the file %s\n",fileName);
		return false;
	}
	for (int i = strlen(fileName); i >= 0; i--) {
		if (fileName[i] == '.') {
			mode = modeMap[fileName + i + 1];
			return true;
		}
	}
	return true;
}



AllCount WordCounter::CountAll() {
	switch (mode) {
	case ModePhp:
		return CountPhp();
	case ModePython:
		return CountPython();
	case ModeCpp:
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
			else if (iswspace(buf[0])) {
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

AllCount WordCounter::CountPhp() {
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
			else if (iswspace(buf[0])) {
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
			case '#':
				mode = note;
				cnt.cnt[5]++;
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

AllCount WordCounter::CountPython() {
	char buf[30] = { 0 };
	int len = 0;
	int SIZE = 1;
	AllCount cnt = { 0 };
	int currentCharCount = 0;
	bool hasAlpha = false;
	char prevChar = 0;
	char prevprevChar = 0;
	enum Mode {
		none,
		note,
		multiNote,
		multiNoteDq
	}mode = none;
	do {
		len = fread(buf, 1, SIZE, currentFile);
		if (len) {
			if (iswalnum(buf[0])) {
				hasAlpha = true;
			}
			else if (iswspace(buf[0])) {
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
				if (prevChar == '\''&&prevChar == '\''&&buf[0] == '\'') {
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
			}
			else if (mode == multiNoteDq) {
				if (prevChar == '"'&&prevChar == '"'&&buf[0] == '"') {
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
			case '\'':
				if (prevChar == '\''&&prevChar == '\'') {
					mode = multiNote;
					cnt.cnt[5]++;
					currentCharCount -= 2;
				}
				else {
					currentCharCount++;
				}
				break;
			case '"':
				if (prevChar == '"'&&prevChar == '"') {
					mode = multiNote;
					cnt.cnt[5]++;
					currentCharCount -= 2;
				}
				else {
					currentCharCount++;
				}
				break;
			case '#':
				mode = note;
				cnt.cnt[5]++;
				break;

			case '\r':
				break;
			default:
				if (!iswspace(buf[0])) {
					currentCharCount++;
				}
				break;


			}
			prevprevChar = prevChar;
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