#include <iostream>
#include "MemoryManager.h"

using namespace std;

int FIRST_SIZE = 10;
int NEW_SIZE = 15;

int main() {
	int* memoryManager;
	memoryManager = (int*)mAlloc(FIRST_SIZE);
	cout << "Put ms sizeof: " << FIRST_SIZE << endl;
	for(int i = 0; i < FIRST_SIZE; i++)
		cin >> memoryManager[i];
	for (int i = 0; i < FIRST_SIZE; i++)
		cout << memoryManager[i] << ' ';

	cout << "\nPut realloc ms sizeof: " << NEW_SIZE << endl;

	memoryManager = (int*)Reallocmem(memoryManager, NEW_SIZE);
	for (int i = 0; i < NEW_SIZE; i++)
		cin >> memoryManager[i];
	for (int i = 0; i < NEW_SIZE; i++)
		cout << memoryManager[i] << ' ';
	Freemem(memoryManager);

	return 0;
}