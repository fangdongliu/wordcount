#include"App.h"
#include<unordered_set>
#include<iostream>
using namespace std;

int main(int argc,char** argv) {
	App app;
	app.HandleCommand(argc, argv);
	return 0;
}