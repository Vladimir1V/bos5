#include "pch.h"
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include "process.h"
#include <time.h>
#include <locale.h>

int chislo;
int i;
CRITICAL_SECTION critsect;
DWORD res;
HANDLE event;
HANDLE  hThreadArray[1024];

DWORD Thread(LPVOID t)
{
	while (TRUE)
	{
		res = WaitForSingleObject(event, 1);
		if (res == WAIT_OBJECT_0)
		{
			if (TryEnterCriticalSection(&critsect))
			{
				srand(time(NULL));
				chislo = rand() % 500 + 100;
				printf("thread %i\n", t);
				Sleep(chislo);
				LeaveCriticalSection(&critsect);
			}
		}
		else
			break;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "Russian");
	HANDLE thread1;
	HANDLE thread2;
	HANDLE thread3;
	HANDLE mut = CreateMutex(NULL, FALSE, TEXT("FirstStep"));
	DWORD result = WaitForSingleObject(mut, 0);
	char ch;

	event = CreateEvent(NULL, TRUE, FALSE, L"event");
	SetEvent(event);
	if (event == NULL)
	{
		printf("CreateEvent failed (%d)\n", GetLastError());
		return 0;
	}
	if ((argc == 1) || (strcmp(argv[1], "/?") == 0))
	{
		printf("Для запуска программы введите: \nbos5.exe число_потоков \n");
		return 0;
	}
	if (result != WAIT_OBJECT_0)
	{
		printf("Fail program is running\n");
		CloseHandle(mut);
		return 0;
	}
	InitializeCriticalSection(&critsect);
	for (i = 0; i < atoi(argv[1]); i++)
	{
		hThreadArray[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, (int *)i, 0, NULL);
	}
	for (i = 0; i < atoi(argv[1]); i++)
	{
		CloseHandle(hThreadArray[i]);
	}
	if (getchar() == 13)
	{
		ResetEvent(event);
	}
	CloseHandle(event);
	ReleaseMutex(mut);
	CloseHandle(mut);
	system("pause");
	return 0;
}
