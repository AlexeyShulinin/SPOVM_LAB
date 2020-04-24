#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <aio.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE 150

void readFromFile(int fd, char *buffer)
{
	memset(buffer, 0, SIZE * sizeof(char));

	struct aiocb aioInfo;                       //структура для асинхронного ввода/вывода
	memset(&aioInfo, 0, sizeof(struct aiocb));
	aioInfo.aio_fildes = fd;                    //присваиваем дескриптор файла
    aioInfo.aio_buf = buffer;                   //присваиваем адрес буфера в памяти
	aioInfo.aio_nbytes = SIZE;                  //число передаваемых байт
	aioInfo.aio_lio_opcode = LIO_READ;          //запрошенная операция чтения

	if(aio_read(&aioInfo) == -1)                //возможно ли асинхронная чтение:0 - если запрос был принят
	{
		printf("Error at aio_read\n");
		close(fd);
		exit(EXIT_FAILURE);
	}
	while(aio_error(&aioInfo) == EINPROGRESS);  //цикл работает,пока выполняется операция, когда завершится, возвращает 0

}


void writeToFile(int fd, char *buffer)
{
	struct aiocb aioInfo;                       //структура для асинхронного ввода/вывода
	memset(&aioInfo, 0, sizeof(struct aiocb));
	aioInfo.aio_fildes = fd;                    //присваиваем дескриптор файла
	aioInfo.aio_buf = buffer;                   //присваиваем адрес буфера в памяти
	aioInfo.aio_nbytes = strlen(buffer);        //число передаваемых байт
	aioInfo.aio_lio_opcode = LIO_WRITE;         //запрошенная операция записи

	if(aio_write(&aioInfo) == -1)               //возможно ли асинхронное чтение
	{
		printf("Error at aio_write\n");
		close(fd);
		exit(EXIT_FAILURE);
	}

	while(aio_error(&aioInfo) == EINPROGRESS);  //цикл работает,пока выполняется операция, когда завершится, возвращает 0

}