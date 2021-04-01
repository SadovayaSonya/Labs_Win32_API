#include "StdAfx.h"
#include "helper.h"
#include "ipc.h"

// Static variables
char *server_addr = NULL;
long stats_server_rcv_count = 0;
long stats_client_rcv_count = 0;

// Client thread
DWORD WINAPI test_client(LPVOID context)
{
	// Create the IPC client
	osIPC::Client client(server_addr);

	// Declare vars
	DWORD testData[4000];

	// Continuously write data
	for (;;)
	{
		// Write some data
		client.write(testData, 1);
		stats_client_rcv_count++;
	}

	// Success
	return 0;
};

// Server thread
int _tmain(int argc, _TCHAR* argv[])
{
	// Create the IPC server
	osIPC::Server server;

	// Get the address
	server_addr = server.getAddress();

	// Start lots of threads
	for (int n = 0; n < 1; n++)
	{
		HANDLE hThread = CreateThread(NULL, 0, test_client, NULL, 0, NULL);
		if (!hThread) return false;
	}

	// Declare vars
	DWORD timerOutput = GetTickCount();
	unsigned __int64 N = 0;
	unsigned __int64 lN = 0;
	unsigned __int64 received = 0;

	// Enter a loop reading data
	for (;;N ++)
	{
		// Read the data
		osIPC::Block *pBlock = server.getBlock();
		if (pBlock->Amount > 0)
		{
			received += pBlock->Amount;
			stats_server_rcv_count++;
		}
		server.retBlock(pBlock);
		
		// Check the timer
		DWORD curTime = GetTickCount();
		if (curTime - timerOutput > 1000)
		{
			timerOutput = curTime;

			// Print the data
			printf("Server:%01d\tClient%01d\tRate: %01u\tAmount: %01u\n", stats_server_rcv_count, stats_client_rcv_count, (DWORD)(N - lN), (DWORD)received);
			lN = N;
			received = 0;
		}
	}

	// Success
	return 0;
};