#pragma once
#include <windows.h>
#include <windowsx.h>
#include<iostream>

using namespace std;

//��������� ������
void* getNewMemory(int size) {
	//�������� �� ��������� ���� ������ ��� ���� ��������� ������. ����������� p����p ����� ���� ������, ��� ���������.
	HLOCAL memory = LocalAlloc(GHND,			//���� ������������ ������������ ���� ������. ���������� ����� ������������� ����� ������ ����� ����������
												//���������� ������������� ������
					size + sizeof(HLOCAL));		//���-�� ������

	if (memory != NULL) 
	{
		//��������� ������ ��������� ������ � ���������� ��������� �� ������ ���� ����� ������ �������.
		void* pmem_tmp = LocalLock(memory);
		if (pmem_tmp != NULL)
		{
			// ������ ����������� � ������ ���������� ������
			pmem_tmp = memory;
			// ��������� ������ ������ �� ��������� ����� ����������� ����
			pmem_tmp = (int*)pmem_tmp + sizeof(HLOCAL);
			return pmem_tmp;
		}
		else
		{
			cout << "Error: error blocking" << endl;
			cout << GetLastError() << endl;
			return nullptr;
		}
	}
	else 
	{
		cout << "Error: NULL" << endl;
		cout << GetLastError() << endl;
		return nullptr;
	}
}

// ������� ����� � ���������� ���������
void* mAlloc(int size) 
{
	void* pMemtmp = getNewMemory(size);

	if (pMemtmp == nullptr) 
	{
		// ������� �������������� ������ � �� ���������� ���������
		UINT uMaxFreeMem = LocalCompact(size);
		cout << "Compressing..." << endl;
		pMemtmp = getNewMemory(size);
		if (pMemtmp == nullptr) 
		{
			cout << "Memory is full" << endl;
			return nullptr;
		}
		else 
		{
			return pMemtmp;
		}
	}
	else 
	{
		return pMemtmp;
	}
}

void Freemem(void* pmem) 
{
	// ��������� ����������� ������
	HLOCAL freeMem = (int*)pmem - sizeof(HLOCAL);

	// ������������� hmemLocal
	LocalUnlock(freeMem);

	// ���������� NULL ��� �������� ������������ ������
	if (LocalFree(freeMem) != NULL)
	{
		cout << "Free error!" << endl;
		cout << GetLastError() << endl;
	}
	else
	{
		cout << "Correct!" << endl;
		pmem = nullptr;
	}
}

void* Reallocmem(void* mem, int newSize) 
{
	// ��������� ����������� ������
	HLOCAL reallocMemory = (int*)mem - sizeof(HLOCAL);

	//������ ������� ���������� ������� ��������� ������. ������ ����� ������������� ��� �����������.
	reallocMemory = LocalReAlloc(reallocMemory, //���������� ������
									newSize,	//����� ������
									NULL);		//�����

	if (reallocMemory == NULL)
	{
		cout << "Error in realloc block" << endl;
		cout << GetLastError() << endl;
		return nullptr;
	}

	void* startMem = (int*)mem - sizeof(HLOCAL);
	startMem = LocalLock(reallocMemory);
	if (startMem == NULL) 
	{
		cout << "Error in locking block" << endl;
		cout << GetLastError() << endl;
	}
	startMem = reallocMemory;
	return (int*)startMem + sizeof(HLOCAL);
}
