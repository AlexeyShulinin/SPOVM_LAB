#include "dll.h"

extern "C" __declspec(dllexport) DWORD ThreadReader(LPVOID lpParam)
{
	list<string>::iterator iterFileList = readerInfo.fileList.begin();

	OVERLAPPED overlapped = { 0 };						//структура для асинхронного ввода/вывода
	overlapped.hEvent = readerInfo.info->ReadOffEvent;

	while (iterFileList != readerInfo.fileList.end())	//пока не будет достигнут конец списка
	{
		HANDLE fileHandle = CreateFileA((readerInfo.info->CurDir + *iterFileList).c_str(),	//имя файла 
										GENERIC_READ,										//доступ для чтения
										0,													//режим обмена
										NULL,												//дескриптор безопасности
										OPEN_EXISTING,										//как действовать(открывает файл)
										FILE_FLAG_OVERLAPPED,								//флаг асинхронного ввода/вывода
										NULL);												//дескриптор шаблона файла

		overlapped.Offset = 0;
		readerInfo.info->ReadBytes = 0;

		cout << "Reading file " << *iterFileList << " ..." << endl;

		DWORD fileSize = GetFileSize(fileHandle, NULL);							//запоминаем размер файла

		while (true)
		{
			WaitForSingleObject(readerInfo.info->WriteOffEvent, INFINITE);		//ожидания конца записи

			overlapped.Offset += readerInfo.info->ReadBytes;
			if (overlapped.Offset == fileSize) break;							//когда кол-во считанной информации будет равно кол-ву байтов в файле 
																				//начинает работать писатель										
			ResetEvent(readerInfo.info->WriteOffEvent);							//ставит событие чтение в несигнальное состояние

			ReadFile(fileHandle,							//дескриптор файла для чтения
				readerInfo.info->buffer,					//буфер данных(туда запишутся данные из файла)
				readerInfo.info->sizeBuffer,				//число байт для чтения
				&readerInfo.info->ReadBytes,				//число прочитанных байт
				&overlapped);								//асинхронный буфер
		}

		iterFileList++;
	}

	return 0;
}

extern "C" __declspec(dllexport) DWORD ThreadWriter(LPVOID lpParam)
{
	OVERLAPPED overlapped = { 0 };						//структура для асинхронноого ввода/вывода
	overlapped.hEvent = writerInfo.info->WriteOffEvent;	//заносим в структуру событие записи

	while (true)
	{
		WaitForSingleObject(writerInfo.info->ReadOffEvent, INFINITE);								//ожидаения потока чтения
		ResetEvent(writerInfo.info->ReadOffEvent);													//ставим событие чтения в сигнальное

		HANDLE fileHandle = CreateFileA((writerInfo.info->CurDir + writerInfo.outputFile).c_str(),	//имя файла
										GENERIC_WRITE,												//доступ для записи
										0,															//режим обмена
										NULL,														//дескриптор безопасности
										OPEN_ALWAYS,												//создаёт файл асинхронного вывода
										FILE_FLAG_OVERLAPPED,										//флаг асинхронного ввода/вывода
										NULL);														//дескриптор шаблона файла

		overlapped.Offset += writerInfo.info->WriteBytes;

		WriteFile(fileHandle, writerInfo.info->buffer, writerInfo.info->ReadBytes,					//записываем данные из прочитанного 
			&writerInfo.info->WriteBytes, &overlapped);												//файла в файл записи

		CloseHandle(fileHandle);			//закрываем дескриптор файла записи
	}

	return 0;
}

//метод получения из дериктории файлов с расщирение txt
bool getTxtFiles(string directory, list<string> *fileList)
{
	WIN32_FIND_DATA dataFile;
	HANDLE file;

	file = FindFirstFile((directory + "*.txt").c_str(), &dataFile);

	if (file != INVALID_HANDLE_VALUE)
	{
		do
		{
			fileList->push_back(dataFile.cFileName);
		} while (FindNextFile(file, &dataFile));
	}
	else return false;

	FindClose(file);


	return true;
}

extern "C" __declspec(dllexport) HANDLE init(char *curDir, char *outputFile)
{
	commonInfo.buffer = new char[10240];
	commonInfo.sizeBuffer = 10240;
	commonInfo.CurDir = (string)curDir;
	commonInfo.ReadOffEvent = CreateEvent(NULL, TRUE, FALSE, NULL);				//создаём событие чтения с ручным сбросом
	commonInfo.WriteOffEvent = CreateEvent(NULL, TRUE, FALSE, NULL);			//создаём событие записи
	commonInfo.ReadBytes = 0;
	commonInfo.WriteBytes = 0;

	writerInfo.outputFile = (string)outputFile;					//структура для файла записи
	writerInfo.info = &commonInfo;

	if (!getTxtFiles(commonInfo.CurDir, &readerInfo.fileList))	//заполнение списка названий файлов для чтения
		return 0;

	readerInfo.fileList.remove(writerInfo.outputFile);

	readerInfo.info = &commonInfo;								//структура для файла чтения

	remove((commonInfo.CurDir + writerInfo.outputFile).c_str());

	SetEvent(commonInfo.WriteOffEvent);							//устанавливаем дескриптор собития записи на сигнальное состояние
																//чтобы запись не начиналась раньше, чем будет прочитан файл чтения

	return commonInfo.WriteOffEvent;
}