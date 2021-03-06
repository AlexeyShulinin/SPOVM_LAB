#pragma once
#include <iostream>
using namespace std;
#include <Windows.h>
#include <list>
#include <iterator>
#include <string>


struct CommonInfo
{
	string CurDir;				//���������� �������
	char *buffer;				//��������� �� ������ ������,��������� �� �����
	int sizeBuffer;				//������ ������
	DWORD ReadBytes;			//������������ �����
	DWORD WriteBytes;			//���������� 
	HANDLE ReadOffEvent;		//���������� ������� ������
	HANDLE WriteOffEvent;		//���������� ������� ������
} commonInfo;

struct ReaderInfo
{
	list<string> fileList;		//������ ������ ��� ������
	CommonInfo *info;
} readerInfo;

struct WriterInfo
{
	string outputFile;			//���� ��� ������
	CommonInfo *info;
} writerInfo;

bool getTxtFiles(string directory, list<string> *fileList);

extern "C" __declspec(dllexport) DWORD ThreadReader(LPVOID lpParam);
extern "C" __declspec(dllexport) DWORD ThreadWriter(LPVOID lpParam);
extern "C" __declspec(dllexport) HANDLE init(char *curDir, char *outputFile);