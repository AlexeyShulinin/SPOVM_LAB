#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>

using namespace std;

//указатель на внешний тип
typedef HANDLE(_cdecl* LPFNDLLinit)(char *, char *);

int main()
{
	char directory[MAX_PATH];
	char outputFileName[11] = "output.txt";

	GetCurrentDirectory(MAX_PATH, directory);
	strcat(directory, "\\");

	HINSTANCE hLib = LoadLibrary("Lab5Dll.dll");		//загружаем dll
	if (!hLib)
	{
		cout << "To work we need Lab5Dll.dll!\n";
		exit(0);
	}

	LPFNDLLinit init = (LPFNDLLinit)GetProcAddress(hLib, TEXT("init"));
	HANDLE WriteOffEvent = init(directory, outputFileName);

	if (WriteOffEvent == 0)
	{
		cout << "Error of input information!\n";
		exit(0);
	}

	LPTHREAD_START_ROUTINE  reader = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadReader"));
	LPTHREAD_START_ROUTINE  writer = (LPTHREAD_START_ROUTINE)GetProcAddress(hLib, TEXT("ThreadWriter"));

	//создаём потоки чтения и записи
	HANDLE threadReader = CreateThread(NULL, 0, reader, NULL, 0, 0);
	HANDLE threadWriter = CreateThread(NULL, 0, writer, NULL, 0, 0);

	//ожидаем конца операции у обоих потоков
	WaitForSingleObject(threadReader, INFINITE);
	WaitForSingleObject(WriteOffEvent, INFINITE);

	CloseHandle(threadReader);						//очищаем дескрипторы потоков
	CloseHandle(threadWriter);

	cout << "Combining completed successfully!\n";
	FreeLibrary(hLib);								//убираем из памяти dll
	exit(0);

}