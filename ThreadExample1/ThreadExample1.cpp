// ThreadExample1.cpp
//
// A Visual c++ v2015 program to show the use of concurrency / multithreading
// using critical sections
// using mutexes
// using semaphores
//
// (c) 2016 Dumarey Frederik

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <process.h>

volatile int count = 0; // the volatile keyword describes that the variable is set in a memory region that is accesible by asynchroneous threads

CRITICAL_SECTION LockSection; // a critical section prevents multi threading access to a code section in the same process without kernel handles

HANDLE mutex; // a mutex is a kernel object that can be locked and unlocked

HANDLE semaphore; // a semaphore is a kernel object that allows a maximum number of accesses to a limited resource (eg. network, usb, disk)

int CheckPrimeNumber(int n)
{
	for (int i = 2; i < (int)(sqrt((float)n) + 1.0); i++) {
		if (n % i == 0) return 0;
	}
	return 1;
}

unsigned int __stdcall AThread(void*)
{
	char* s;
	while (count < 50) {
		while (!TryEnterCriticalSection(&LockSection)) // try to setup a critical section, and check if the critical section is taken
		{
			printf("Waiting for critical section");
		}
		int number = count++;
		LeaveCriticalSection(&LockSection); // set the end of the critical section
		s = "no prime number";
		if (CheckPrimeNumber(number)) s = "a prime number";
		printf("Thread number %d handling count:%d %s\n", GetCurrentThreadId(), number, s);
	}
	return 0;
}

unsigned int __stdcall BThread(void*)
{
	char* s;
	while (count < 50) {
		WaitForSingleObject(mutex, INFINITE); // wait for the mutex to be unlocked and then locks the mutex again
		int number = count++;
		ReleaseMutex(mutex); // unlock the handle
		s = "no prime number";
		if (CheckPrimeNumber(number)) s = "a prime number";
		printf("Thread number %d handling count:%d %s\n", GetCurrentThreadId(), number, s);
	}
	return 0;
}

unsigned int __stdcall CThread(void*)
{
	char* s;
	while (count < 50) {
		WaitForSingleObject(semaphore, INFINITE); // decrement the semaphore, and wait till a timeout
		int number = count++;
		ReleaseSemaphore(semaphore, 1, 0); // increment the semaphore by amount (parameter 2)
		s = "no prime number";
		if (CheckPrimeNumber(number)) s = "a prime number";
		printf("Thread number %d handling count:%d %s\n", GetCurrentThreadId(), number, s);
	}
	return 0;
}

int main()
{
	HANDLE myhandleA, myhandleB; // define Win32 handles for the threads
	
	// Critical section solution
	
	printf("The critical section solution\n");
	InitializeCriticalSectionAndSpinCount(&LockSection, 1000); // initializes the critical section and set the spin count to 1000 = number of times caller tries to enter a critical section befores the thread goes to sleep (suspend mode)
	
	myhandleA = (HANDLE)_beginthreadex(0, 0, &AThread, (void*)0, 0, 0); // start a new WIN32 API thread
	myhandleB = (HANDLE)_beginthreadex(0, 0, &AThread, (void*)0, 0, 0);

	WaitForSingleObject(myhandleA, INFINITE); // wait until the thread terminates for infinite time
	WaitForSingleObject(myhandleB, INFINITE);

	getchar();

	// Mutex solution

	count = 0;
	printf("The mutex solution\n");
	mutex = CreateMutex(0, 0, 0); // initializes the mutex 

	myhandleA = (HANDLE)_beginthreadex(0, 0, &BThread, (void*)0, 0, 0); // start a new WIN32 API thread
	myhandleB = (HANDLE)_beginthreadex(0, 0, &BThread, (void*)0, 0, 0);

	WaitForSingleObject(myhandleA, INFINITE); // wait until the thread terminates for infinite time
	WaitForSingleObject(myhandleB, INFINITE);

	getchar();

	// Semaphore solution

	count = 0;
	printf("The semaphore solution\n");
	semaphore = CreateSemaphore(0, 1, 1, 0); // initializes the semaphore with a parameter 2 = initial count and parameter 3 = the maximum count for the semaphore before locking

	myhandleA = (HANDLE)_beginthreadex(0, 0, &CThread, (void*)0, 0, 0); // start a new WIN32 API thread
	myhandleB = (HANDLE)_beginthreadex(0, 0, &CThread, (void*)0, 0, 0);

	WaitForSingleObject(myhandleA, INFINITE); // wait until the thread terminates for infinite time
	WaitForSingleObject(myhandleB, INFINITE);

	getchar();

	// clean-up all handles (to prevent kernel memory leaks !!!)

	CloseHandle(myhandleA); // close the handles to free kernel memory!
	CloseHandle(myhandleB);

	DeleteCriticalSection(&LockSection); // cleans up the critical section variable
	CloseHandle(mutex); // cleans up the mutex handle
	CloseHandle(semaphore); // cleans up the semaphore handle
	
    return 0;
}