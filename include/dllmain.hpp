#pragma once
#include <string>
using String = std::string;
constexpr auto CURRENT_VERSION = "1.1.0.0 JSON";
using RVExtensionCallback = int (*)(char const* name, char const* function,
									char const* data);

int strncpy_safe(char* output, const String src);
bool areEqual(const char* _Str1, const char* _Str2);

extern "C" {
void RVExtensionVersion(char* output, int outputSize);
void RVExtension(char* output, int outputSize, const char* function);
// void RVExtensionRegisterCallback(int(*callbackProc)(char const*, char const*, char const*));
int RVExtensionArgs(char* output, int outputSize, const char* function,
					const char** args, int argsCnt);
void RVExtensionRegisterCallback(RVExtensionCallback callbackProc);
}
