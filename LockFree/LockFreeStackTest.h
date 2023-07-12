#pragma once


#include "TempAllocator.h"
#include "container/LockFreeStack.h"
#include <thread>
#include <string>


void LockFreeStackTest()
{
	constexpr int NumInThread = 10;
	constexpr int NumOutThread = 5;
	constexpr int NumItems = 20000;

	container::LockFreeStack<std::string, _temp::Allocator, _temp::Deallocator> stack;

	std::thread inThreads[NumInThread];
	std::thread outThreads[NumOutThread];

	for (int i = 0; i < NumInThread; ++i)
	{
		inThreads[i] = std::thread([&](int idx) {
			std::string threadName = std::to_string(idx);
			for (int n = 0; n < NumItems; ++n)
			{
				std::string str = threadName + ": " + std::to_string(n);
				stack.Push(str);
			}
		}, i);
	}

	for (int i = 0; i < NumOutThread; ++i)
	{
		outThreads[i] = std::thread([&](int idx) {
			const int totalNumItems = NumItems * NumInThread;
			const int step = totalNumItems / NumOutThread;
			for (int n = 0; n < step; ++n)
			{
				std::string str;
				if (stack.Pop(str))
				{
					printf("%s\n", str.c_str());
				}
				else
				{
					printf("faild\n");
				}
			}
		}, i);
	}

	for (int i = 0; i < NumInThread; ++i)
	{
		inThreads[i].join();
	}
	for (int i = 0; i < NumOutThread; ++i)
	{
		outThreads[i].join();
	}
}