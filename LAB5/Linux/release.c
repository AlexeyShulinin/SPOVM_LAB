#include "header.h"


const int MAX = 3;


char *files[3] = { "./files/1.txt", "./files/2.txt", "./files/3.txt"};

//функция чтения, выполняется в потоке чтения
void *threadReader(void *arg)
{
	struct Info *info = (struct Info *)arg;								//запоминаем указатель для изменения информации
	void (*lib_function)(int, char *);
	*(void **) (&lib_function) = dlsym(info->library, "readFromFile");	//для работы с функцией readFromFile динамической библиотеки получаем адрес функции в памяти

	for(int i = 0; i < MAX; i++)
	{
		while(info->flag);							//задержка ожидания потока записи
		pthread_mutex_lock(&info->mutex);			//блокируем мьютекс
		int fd = open(info->fileNames[i], O_RDONLY);//получаем десриптор файла
		if(fd < 0)									//если отрицательный, файла нету
		{
			printf("File not found");
			exit(-1);
		}
		(*lib_function)(fd, info->buffer);			//отправляем аргументы в функцию readFromFile
		close(fd);									//закрываем файл
		pthread_mutex_unlock(&info->mutex);			//освобождаем мьютекс
		info->size++;								//увеличиваем счётчик информации
		info->flag = 1;								//сигнализируем потоку записи,что было осуществлено чтение из файла
		usleep(1);
	}
	info->threadsNumber--;							//при окончании чтения данных из файла, декрементируем активность потока(счётчика потоков)
	return NULL;
}

//функция записи, выполняется в другом потоке записи
void *threadWriter(void *arg)
{
	struct Info *info = (struct Info *)arg;								//запоминаем указатель для изменения информации
	void (*lib_function)(int, char *);
	*(void **) (&lib_function) = dlsym(info->library, "writeToFile");	//для работы с функцией writeToFile динамической библиотеки получаем адрес функции в памяти
	pthread_mutex_lock(&info->mutex);
	int fd = open("./files/output.txt", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND);
	pthread_mutex_unlock(&info->mutex);
	usleep(1);
	while(!info->flag);													//задержка ожидания потока чтения(как только будет получена вся информация из фалйа,начнйт работать данный поток)
	for(int i = 0; i < MAX; i++)
	{
		while(!info->flag);												//-//-
		pthread_mutex_lock(&info->mutex);								//блокируем мьютекс
		(*lib_function)(fd, info->buffer);								//переводим дескриптор файла и буфер, считанный из фалйла,в аргументы writeToFile
		pthread_mutex_unlock(&info->mutex);								//освобождаем
		info->flag = 0;													//можно читать новый файл
		usleep(1);
	}
	close(fd);															//закрываем файл записи
	info->threadsNumber--;												//т.к. поток завершил все операции, уменьшаем счётчик
	return NULL;
}

void initInfo(struct Info *info)
{
	info->fileNames = files;									//запоминаем фалы для чтения
	info->size = 0;
	info->library = dlopen("./lib.so", RTLD_LAZY);			//загрузка динамической библиотеки, возвращет дескриптор динамической библиотеки
	info->threadsNumber = 0;
	info->flag = 0;												//устанавливаем флаг на чтение

}

void createMutex(struct Info *info)
{
	pthread_mutex_init(&info->mutex, NULL);						//создаём мьютекс как средство синхронизации потоков
}

void runThreads(struct Info *info)
{
	pthread_create(&info->readThread, NULL, &threadReader, info);	//создаём поток чтения, куда отправляем функцию чтения
	info->threadsNumber++;
	pthread_create(&info->writeThread, NULL, &threadWriter, info);	//аналогично для потока записи
	info->threadsNumber++;
}

void waitThreads(struct Info *info)
{
	while(info->threadsNumber);
	printf("Threads are finished\n");
	dlclose(info->library);											//выгружаем из памяти динамическую библиотеку
}