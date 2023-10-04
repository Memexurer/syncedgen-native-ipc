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

const int AES_INIT = 0x14532e8a0;

void do_aes_decrypt(char* input_key, char* input_data, int input_len, char* output_data, int* output_len) {
	char* aes_key = new char[488];

	aes_init_ptr aes_init = reinterpret_cast<aes_init_ptr>(AES_INIT); //todo sigsearch ;-; - or just dont update the game
	aes_init(aes_key, input_key);

	aes_decrpyt_ptr aes_decrypt = reinterpret_cast<aes_decrpyt_ptr>(0x14532db10);

	//input len - 1 + output
	int res = aes_decrypt(input_data, input_len, aes_key, output_data, output_len);

	//todo - input nierówny wchuj XDDD jakeis trashbyte i w ogole
	// ale gdzies po srodku jest decrypted wiec g
	char buffer[128];
	sprintf(buffer, "%s nigga\n", output_data);

	OutputDebugStringA(buffer);

	delete[] buffer;
}



int main() {
	HANDLE pipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\fallingpipe"), PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
		PIPE_WAIT, 1, 512, 512, 0, NULL);

	char buffer[512];
	char* response;

	while (true) {
		memset(buffer, 0, 512);

		ConnectNamedPipe(pipe, NULL);

		// Get message
		DWORD bytesRead;
		ReadFile(pipe, buffer, 512, &bytesRead, NULL);

		int response_len;

		if (buffer[0] == 0x00) {
			paes_input input = (paes_input)(buffer+1);

			char* response2 = new char[input->input_len];
			int response_len2 = input->input_len;
			do_aes_decrypt(input->key, input->input, input->input_len, response2, &response_len2);

			response = new char[128];
			response_len = 128;

			sprintf(response, "siema: %s\n", response2);
		}
		else {
			response = new char[31];
			response_len = 31;
			strcpy(response, "idk men u sent something weird");
		}

		// Send response 
		WriteFile(pipe, response, response_len,
			&bytesRead, NULL);

		DisconnectNamedPipe(pipe);

		memset(response, 0, response_len);
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