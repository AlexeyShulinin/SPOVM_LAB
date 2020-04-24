#pragma once
#include <iostream>
using namespace std;
#include <Windows.h>
#include <list>
#include <iterator>
#include <string>


struct CommonInfo
{
	string CurDir;				//директория проекта
	char *buffer;				//указатель на буффер данных,считанных из файла
	int sizeBuffer;				//размер буфера
	DWORD ReadBytes;			//прочитаенные байты
	DWORD WriteBytes;			//записанные 
	HANDLE ReadOffEvent;		//дескриптор события чтения
	HANDLE WriteOffEvent;		//дескриптор события записи
} commonInfo;

struct ReaderInfo
{
	list<string> fileList;		//список файлов для чтения
	CommonInfo *info;
} readerInfo;

struct WriterInfo
{
	string outputFile;			//файл для записи
	CommonInfo *info;
} writerInfo;

bool getTxtFiles(string directory, list<string> *fileList);

extern "C" __declspec(dllexport) DWORD ThreadReader(LPVOID lpParam);
extern "C" __declspec(dllexport) DWORD ThreadWriter(LPVOID lpParam);
extern "C" __declspec(dllexport) HANDLE init(char *curDir, char *outputFile);