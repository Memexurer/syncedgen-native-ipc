// dllmain.cpp : Definiuje punkt wejścia dla aplikacji DLL.

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <Windows.h>
#include <cstring>

void print(const char* format, ...) {
	va_list args;
	va_start(args, format);

	char buffer[256];
	vsnprintf(buffer, sizeof(buffer), format, args);

	OutputDebugStringA(buffer);
	printf(buffer);

	va_end(args);
}
#define printline() print("Line %d: %s\n", __LINE__, __FILE__)

using aes_init_ptr = void (*)(void*, void*);

// input, input len, encryption key, output, result output len
using aes_decrpyt_ptr = int (*)(void*, int, void*, void*, int*);

typedef struct aes_input {
	char key[32];
	int input_len;
	char input[];
} aes_input, *paes_input;

void do_aes_decrypt(char* input_key, char* input_data, int input_len, char* output_data, int* output_len) {
	char* aes_key = new char[488];

	aes_init_ptr aes_init = reinterpret_cast<aes_init_ptr>(0x14532e8a0); //todo sigsearch ;-; - or just dont update the game
	aes_init(aes_key, input_key);

	aes_decrpyt_ptr aes_decrypt = reinterpret_cast<aes_decrpyt_ptr>(0x14532db10);

	//input len - 1 + output
	int res = aes_decrypt(input_data, input_len, aes_key, output_data, output_len);

	delete[] aes_key;
}

const int BUFFER_SIZE = 65536;

int main() {
	HANDLE pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\fallingpipe"), PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
		PIPE_WAIT, 1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	char buffer[BUFFER_SIZE];
	char* response;

	while (true) {
		memset(buffer, 0, BUFFER_SIZE);

		ConnectNamedPipe(pipe, NULL);

		// Get message
		DWORD bytesRead;
		ReadFile(pipe, buffer, BUFFER_SIZE, &bytesRead, NULL);

		int response_len;

		printf("dupson first byte: %d", buffer[0]);
		if (buffer[0] == 0x00) {
			paes_input input = (paes_input)(buffer+1);

			char* response2 = new char[input->input_len];

			response_len = input->input_len;
			response = new char[response_len];
			memset(response, 0, response_len);

			do_aes_decrypt(input->key, input->input, input->input_len, response, &response_len);
		} else {
			response = new char[31];
			response_len = 31;
			strcpy(response, "idk men u sent something weird");
		}

		// Send response 
		WriteFile(pipe, response, response_len,
			&bytesRead, NULL);

		DisconnectNamedPipe(pipe);

		delete[] response;
	}

	CloseHandle(pipe);

	return 0;
}

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // handle to DLL module
	DWORD fdwReason,     // reason for calling function
	LPVOID lpvReserved)  // reserved
{

	OutputDebugStringA("nigga dllmain PART 2");
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		main();

		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:

		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:

		if (lpvReserved != nullptr)
		{
			break; // do not do cleanup if process termination scenario
		}

		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}