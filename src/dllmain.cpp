#include "dllmain.hpp"
#include <cstring>
#include <thread>
#include "armaLogData.hpp"

int strncpy_safe(char* output, const char* src, int size) {
	int i;
	size--;
	for (i = 0; i < size && src[i] != '\0'; i++) {
		output[i] = src[i];
	}
	output[i] = '\0';
	return i;
}

RVExtensionCallback callbackPtr = nullptr;
void RVExtensionRegisterCallback(RVExtensionCallback callbackProc) {
	callbackPtr = callbackProc;
}

//--- Extension version information shown in .rpt file
void RVExtensionVersion(char* output, int outputSize) {
	//--- max outputSize is 32 bytes
	strncpy_safe(output, CURRENT_VERSION, outputSize);
}

void RVExtension(char* output, int outputSize, const char* function) {
	strncpy_safe(output,
				 "use RVExtensionArgs, available functions: manageSession",
				 outputSize);
}

//"extension" callExtension["function", ["arguments"...]]
int RVExtensionArgs(char* output, int outputSize, const char* function,
					const char** args, int argsCnt) {
	std::vector<String> arguments;
	for (int i = 0; i < argsCnt; i++)
		arguments.push_back(args[i]);

	/*DATA FUNCTIONS*/

	if (strcmp(function, "manageSession") == 0) {
		std::thread t1{arma::manageSession, arguments};
		t1.detach();

		strncpy_safe(output, "data being managed", outputSize);
		return 201;
	}

	/*---------------------*/
	strncpy_safe(output, "Available functions: manageSession", outputSize);
	return -1;
}

// "btc_serverLogger" callExtension ["manageSession",["tempMissionSP", "Altis", 45, 20, ["Server","Test",80,1200],["=BTC= Cpt.Ramius","12325425",56,3800],["=BTC= Col.Giallustio","12325425",45,3800],["=BTC= Cpl.Raven","12325425",50,3800]]];