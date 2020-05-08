#pragma once
#include <windows.h>
#include <windowsx.h>
#include<iostream>

using namespace std;

//выделение памяти
void* getNewMemory(int size) {
	//Выделяет из локальной кучи память под блок локальной памяти. Фактический pазмеp может быть больше, чем указанный.
	HLOCAL memory = LocalAlloc(GHND,			//флаг заказывается перемещаемый блок памяти. Логический адрес перемещаемого блока памяти может изменяться
												//происходит инициализация нулями
					size + sizeof(HLOCAL));		//кол-во байтов

	if (memory != NULL) 
	{
		//Блокирует объект локальной памяти и возвращает указатель на первый байт блока памяти объекта.
		void* pmem_tmp = LocalLock(memory);
		if (pmem_tmp != NULL)
		{
			// Запись дескриптора в начало выделенной памяти
			pmem_tmp = memory;
			// Установка начала памяти на следующий после дескриптора байт
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

// находит место и возвращает указатель
void* mAlloc(int size) 
{
	void* pMemtmp = getNewMemory(size);

	if (pMemtmp == nullptr) 
	{
		// попытка дефрагментации памяти и ее повторного выделения
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
	// получение дескриптора памяти
	HLOCAL freeMem = (int*)pmem - sizeof(HLOCAL);

	// разблокировка hmemLocal
	LocalUnlock(freeMem);

	// возвращает NULL при успешном освобождении памяти
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
	// получение дескриптора памяти
	HLOCAL reallocMemory = (int*)mem - sizeof(HLOCAL);

	//Измене размера указанного объекта локальной памяти. Размер может увеличиваться или уменьшаться.
	reallocMemory = LocalReAlloc(reallocMemory, //дескриптор памяти
									newSize,	//новый размер
									NULL);		//флаги

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
