#include "App.h"
#include<windows.h>
#include<Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
using namespace std;

bool IsCmdEqual(const char *a, const char *b, int n) {
	for (int i = 0; i < n; i++) {
		if (a[i] != b[i])return false;
	}
	return a[n] == ' '||a[n]==0;
}

void App::HandleCommand(int argc, char**argv) {
	if (argc == 2) {

		if (IsCmdEqual(argv[1],"-x",2)) {
			string path(argv[0]);

			int pos = path.find_last_of('\\');
			if (pos != path.npos) {
				path = path.substr(0, pos + 1) + "wcGUI.exe";
			}
			else {
				pos = path.find_last_of('/');
				if (pos != path.npos) {
					path = path.substr(0, pos+1)+"wcGUI.exe";
				}
				else {
					path = "wcGUI.exe";
				}
			}
			PROCESS_INFORMATION pro_info; 
			STARTUPINFO sti; 
			ZeroMemory(&pro_info, sizeof(PROCESS_INFORMATION));
			ZeroMemory(&sti, sizeof(STARTUPINFO));
			CreateProcess(path.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sti, &pro_info);

		}
		else if (IsCmdEqual(argv[1], "/?",2)) {
			printf("wc [-x] [-s] [-w] [-l] [-c] [-a] [filename]  \n\
				\n\
				-x         显示用户界面\n\
				-s         递归查找所有匹配项。\n\
				-w         统计单词数\n\
				-l         统计行数\n\
				-a         统计注释行数、空行数、代码行数\n\
				-c         统计字符数\n");
		}
		else {
			printf("wrong input\n");
		}
	}
	else if (argc == 3) {
		if (counter.SetCurrentFile(argv[2])) {
			auto c = counter.CountAll();
			fileName = argv[2];
			if (argv[1][0] == '-' && (argv[1][2] == ' ' || argv[1][2] == 0)) {
				switch (argv[1][1]) {
				case 'w':
					mode = Mode::ModeWord;
					break;
				case 'l':
					mode = Mode::ModeLine;
					break;
				case 'c':
					mode = Mode::ModeChar;
					break;
				case 'a':
					mode = Mode::ModeExtra;
					break;
				default:
					mode = Mode::ModeDefault;
					printf("wrong input\n");
					return;
				}
				CountFiles("./");
			}
			else {
				printf("wrong input\n");
			}
		}
	}
	else if (argc == 4) {
		if(IsCmdEqual(argv[1], "-s", 2)) {
			fileName = argv[3];
			if (argv[2][0] == '-' && (argv[2][2] == ' ' || argv[2][2] == 0)) {
				switch (argv[2][1]) {
				case 'w':
					mode = Mode::ModeWord;
					break;
				case 'l':
					mode = Mode::ModeLine;
					break;
				case 'c':
					mode = Mode::ModeChar;
					break;
				case 'a':
					mode = Mode::ModeExtra;
					break;
				default:
					mode = Mode::ModeDefault;
					printf("wrong input\n");
					return;
				}
				CountFiles("./");
			}
			else {
				printf("wrong input\n");
			}
		}
		else {
			printf("wrong input\n");
		}
	}
	else {
		printf("wrong input\n");
	}
}

void App::CountFiles(string path) {
	SetCurrentDirectoryA(path.c_str());
	WIN32_FIND_DATA data;
	char strpath[1000];
	GetCurrentDirectoryA(1000, strpath);
	auto first = FindFirstFileA("*", &data);
	if (first) {
		do {
			bool isFolder = false;
			if (data.cFileName[0] == '.') {
				isFolder = true;
			}
			else {
				isFolder = data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
				if (isFolder) {
					CountFiles(string(strpath)+'\\' + data.cFileName);
				}
				else if (PathMatchSpecA(data.cFileName, fileName)){
					SetCurrentDirectoryA(strpath);
					if (counter.SetCurrentFile(data.cFileName)) {
						auto c = counter.CountAll();
						printf("file:%s\\%s:\n", strpath, data.cFileName);
						switch (mode) {
						case Mode::ModeWord:
							printf("wordcount: %d\n", c.cnt[1]);
							break;
						case Mode::ModeLine:
							printf("linecount: %d\n", c.cnt[2]);
							break;
						case Mode::ModeChar:
							printf("charcount: %d\n", c.cnt[0]);
							break;
						case Mode::ModeExtra:
							printf("note lines: %d\nspace lines: %d\ncode lines: %d\n", c.cnt[5], c.cnt[3], c.cnt[4]);
							break;
						}
					}
				}
			}
		} while (FindNextFile(first, &data));
		FindClose(first);
	}
}