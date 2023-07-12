#pragma once


#include "TempAllocator.h"
#include "container/LockFreeQueue.h"
#include <thread>
#include <string>


void LockFreeQueueTest()
{
	constexpr int NumInThread = 10;
	constexpr int NumOutThread = 5;
	constexpr int NumItems = 20000;

	container::LockFreeQueue<std::string, _temp::Allocator, _temp::Deallocator> queue;

	std::thread inThreads[NumInThread];
	std::thread outThreads[NumOutThread];

	for (int i = 0; i < NumInThread; ++i)
	{
		inThreads[i] = std::thread([&](int idx) {
			std::string threadName = std::to_string(idx);
			for (int n = 0; n < NumItems; ++n)
			{
				std::string str = threadName + ": " + std::to_string(n);
				queue.Enqueue(str);
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
				if (queue.Dequeue(str))
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