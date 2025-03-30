#include "dllmain.hpp"
#include "armaLogData.hpp"
#include <thread>
#include <cstring>  // Added for strcmp

constexpr auto CURRENT_VERSION = "1.1.0.0 JSON";

int strncpy_safe(char* output, const String src)
{
	return strncpy_safe(output, src.c_str(), static_cast<int>(src.length()) + 1);
}

int strncpy_safe(char* output, const char* src, int size)
{
	int i;
	size--;
	for (i = 0; i < size && src[i] != '\0'; i++)
	{
		output[i] = src[i];
	}
	output[i] = '\0';
	return i;
}

//--- Extension version information shown in .rpt file
void RVExtensionVersion(char* output, int outputSize) {
	//--- max outputSize is 32 bytes
	strncpy_safe(output, CURRENT_VERSION, outputSize); 
}

void RVExtension(char* output, int outputSize, const char* function) {
	(void)function;  // Explicitly indicate that this parameter is intentionally unused
	strncpy_safe(output, "use RVExtensionArgs, available functions: manageSession", outputSize);
}

// Implement the missing callback registration function
void RVExtensionRegisterCallback(int(*callbackProc)(char const*, char const*, char const*)) {
	// This is a stub implementation - the extension doesn't use callbacks currently
	(void)callbackProc; // Avoid unused parameter warning
}

//"extension" callExtension["function", ["arguments"...]]
int RVExtensionArgs(char* output, int outputSize, const char* function, const char** args, int argsCnt) {
	std::vector<String> arguments;
	for (int i = 0; i < argsCnt; i++)  // Changed from unsigned int to int to match argsCnt type
		arguments.push_back(args[i]);

	/*DATA FUNCTIONS*/

	if (strcmp(function, "manageSession") == 0) {
		std::thread t1{ arma::manageSession, arguments };
		t1.detach();

		strncpy_safe(output, "data saved", outputSize);
		return 201;
	}

	/*---------------------*/
	strncpy_safe(output, "Available functions: manageSession, manageNewSession", outputSize);
	return -1;
}

// "btc_serverLogger" callExtension ["manageSession",["tempMissionSP", "Altis", 45, 20, ["Server","Test",80,1200],["=BTC= Cpt.Ramius","12325425",56,3800],["=BTC= Col.Giallustio","12325425",45,3800],["=BTC= Cpl.Raven","12325425",50,3800]]];