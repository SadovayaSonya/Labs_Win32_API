#include "StdAfx.h"
#include "helper.h"
#include "ipc.h"

osIPC::Server::Server(void)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
	m_sAddr = NULL;

	// create the server
	create();
};

osIPC::Server::~Server(void)
{
	// Free memory
	if (m_sAddr)
	{
		free(m_sAddr);
		m_sAddr = NULL;
	}

	// Close the server
	close();
};

void osIPC::Server::create(void)
{
	// Determine the name of the memory
	DWORD ProcessID = GetCurrentProcessId();
	DWORD ThreadID = GetCurrentThreadId();
	DWORD ServerID = osIPC::GetID();

	m_sAddr = (char*)malloc(1024);
	if (!m_sAddr) return;
	sprintf_s(m_sAddr, 1024,"IPC_%04u_%04u_%04u", ProcessID, ThreadID, ServerID);

	char *m_sEvtAvail = (char*)malloc(1024);
	if (!m_sEvtAvail) { printf("server_create: failed: %01d\n", __LINE__); return; }
	sprintf_s(m_sEvtAvail, 1024, "%s_evt_avail", m_sAddr);

	char *m_sEvtFilled = (char*)malloc(1024);
	if (!m_sEvtFilled) { free(m_sEvtAvail); printf("server_create: failed: %01d\n", __LINE__); return; }
	sprintf_s(m_sEvtFilled, 1024, "%s_evt_filled", m_sAddr);

	char *m_sMemName = (char*)malloc(1024);
	if (!m_sMemName) { free(m_sEvtAvail); free(m_sEvtFilled); printf("server_create: failed: %01d\n", __LINE__); return; }
	sprintf_s(m_sMemName, 1024, "%s_mem", m_sAddr);

	// Create the events
	m_hSignal = CreateEventA(NULL, FALSE, FALSE, (LPCSTR)m_sEvtFilled);
	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE) { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); printf("server_create: failed: %01d\n", __LINE__); return; }
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, (LPCSTR)m_sEvtAvail);
	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE) { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); printf("server_create: failed: %01d\n", __LINE__); return; }

	// Create the file mapping
	m_hMapFile = CreateFileMapping
	(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(MemBuff),
		(LPCWSTR)m_sMemName
	);

	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE)  { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); printf("server_create: failed: %01d\n", __LINE__); return; }
 
	// Map to the file
	m_pBuf = (MemBuff*)MapViewOfFile
	(	m_hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		sizeof(MemBuff)
	);

	if (m_pBuf == NULL) { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); printf("server_create: failed: %01d\n", __LINE__); return; }

	// Clear the buffer and initialize the lists
	ZeroMemory(m_pBuf, sizeof(MemBuff));
	m_pBuf->m_Available = 1;
	m_pBuf->m_Filled = 0;

	// Add all the blocks into the available list
	for (int N = 1; N < IPC_BLOCK_COUNT-1; N++)
	{
		// Add this block into the available list
		m_pBuf->m_Blocks[N].Next = N+1;
	}

	// Release memory
	free(m_sEvtAvail);
	free(m_sEvtFilled);
	free(m_sMemName);
};

void osIPC::Server::close(void)
{
	// Close the event
	if (m_hSignal)
	{
		HANDLE handle = m_hSignal;
		m_hSignal = NULL;
		CloseHandle(handle);
	}

	// Close the event
	if (m_hAvail)
	{
		HANDLE handle = m_hAvail;
		m_hAvail = NULL;
		CloseHandle(handle);
	}

	// Unmap the memory
	if (m_pBuf)
	{
		MemBuff *pBuff = m_pBuf;
		m_pBuf = NULL;
		UnmapViewOfFile(pBuff);
	}

	// Close the file handle
	if (m_hMapFile)
	{
		HANDLE handle = m_hMapFile;
		m_hMapFile = NULL;
		CloseHandle(handle);
	}
};

DWORD osIPC::Server::read(void *pBuff, DWORD buffSize, DWORD timeout)
{
	// Validate the connection
	if (!m_pBuf) return 0;
	
	// Enter a continous loop
	for (;;)
	{
		// Grab all the blocks that have been placed in the filled list
		// Loop attempting to grab a block
		for (;;)
		{
			LONG blockIndex = m_pBuf->m_Filled;
			if (blockIndex == 0) break;
			Block *pBlock = m_pBuf->m_Blocks + blockIndex;
			if (InterlockedCompareExchange(&m_pBuf->m_Filled, pBlock->Next, blockIndex) == blockIndex)
			{
				// Get the amount we can write
				DWORD written = buffSize;
				if (written > pBlock->Amount) written = pBlock->Amount;
				
				// Copy the data
				if (written > 0)
					memcpy(pBuff, pBlock->Data, written);

				// Loop attempting to add the block to
				// the singlely linked list
				for (;;)
				{
					LONG iFirst = pBlock->Next = m_pBuf->m_Available;
					if (InterlockedCompareExchange(&m_pBuf->m_Available, blockIndex, iFirst) == iFirst) break;
				}

				// Signal the available event
				SetEvent(m_hAvail);

				// Return the amount written
				return written;
			}
		}

		// Wait on the signal
		if (WaitForSingleObject(m_hSignal, timeout) != WAIT_OBJECT_0)
		{
			// Timeout
			return 0;
		}
	}
};

osIPC::Client::Client(void)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;
};

osIPC::Client::Client(char *connectAddr)
{
	// Set default params
	m_hMapFile = 0;
	m_hSignal = 0;
	m_hAvail = 0;
	m_pBuf = NULL;

	// Determine the name of the memory
	m_sAddr = (char*)malloc(1024);
	if (!m_sAddr) return;
	strcpy_s(m_sAddr, 1024, connectAddr);
	
	char *m_sEvtAvail = (char*)malloc(1024);
	if (!m_sEvtAvail) return;
	sprintf_s(m_sEvtAvail, 1024, "%s_evt_avail", m_sAddr);
	
	char *m_sEvtFilled = (char*)malloc(1024);
	if (!m_sEvtFilled) { free(m_sEvtAvail); return; }
	sprintf_s(m_sEvtFilled, 1024, "%s_evt_filled", m_sAddr);
	
	char *m_sMemName = (char*)malloc(1024);
	if (!m_sMemName) { free(m_sEvtAvail); free(m_sEvtFilled); return; }
	sprintf_s(m_sMemName, 1024, "%s_mem", m_sAddr);
	
	// Create the events
	m_hSignal = CreateEventA(NULL, FALSE, FALSE, (LPCSTR)m_sEvtFilled);
	if (m_hSignal == NULL || m_hSignal == INVALID_HANDLE_VALUE) { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); return; }
	m_hAvail = CreateEventA(NULL, FALSE, FALSE, (LPCSTR)m_sEvtAvail);
	if (m_hAvail == NULL || m_hSignal == INVALID_HANDLE_VALUE) { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); return; }

	// Open the shared file
	m_hMapFile = OpenFileMapping
	(
		FILE_MAP_ALL_ACCESS, // read/write access
		FALSE, // do not inherit the name
		(LPCWSTR)m_sMemName // name of mapping object
	);
	
	if (m_hMapFile == NULL || m_hMapFile == INVALID_HANDLE_VALUE)  { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); return; }
 
	// Map to the file
	m_pBuf = (MemBuff*)MapViewOfFile
	(
		m_hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		sizeof(MemBuff)
	); 

	if (m_pBuf == NULL) { free(m_sEvtAvail); free(m_sEvtFilled); free(m_sMemName); return; }

	// Release memory
	free(m_sEvtAvail);
	free(m_sEvtFilled);
	free(m_sMemName);
};

osIPC::Client::~Client(void)
{
	// Close the event
	CloseHandle(m_hSignal);

	// Close the event
	CloseHandle(m_hAvail);

	// Unmap the memory
	UnmapViewOfFile(m_pBuf);

	// Close the file handle
	CloseHandle(m_hMapFile);
};

osIPC::Block* osIPC::Client::getBlock(void)
{
	// Loop attempting to grab a block
	for (;;)
	{
		LONG blockIndex = m_pBuf->m_Available;
		if (blockIndex == 0) return NULL;
		Block *pBlock = m_pBuf->m_Blocks + blockIndex;
		if (InterlockedCompareExchange(&m_pBuf->m_Available, pBlock->Next, blockIndex) == blockIndex)
			return pBlock;
	}
};

void osIPC::Client::postBlock(Block *pBlock)
{
	// Loop attempting to add the block to
	// the singlely linked list
	LONG blockIndex = (PointerConverter(pBlock).ptrLong - PointerConverter(&m_pBuf->m_Blocks).ptrLong) / sizeof(Block);
	for (;;)
	{
		LONG iFirst = pBlock->Next = m_pBuf->m_Filled;
		if (InterlockedCompareExchange(&m_pBuf->m_Filled, blockIndex, iFirst) == iFirst) break;
	}
	
	// Signal the event
	SetEvent(m_hSignal);
};

DWORD osIPC::Client::write(void *pBuff, DWORD amount)
{
	// Validate the connection
	if (!m_pBuf) return 0;

	// Loop attempting to grab a block
	LONG blockIndex;
	Block *pBlock;
	for (;;)
	{
		blockIndex = m_pBuf->m_Available;
		if (blockIndex == 0) return 0;
		pBlock = m_pBuf->m_Blocks + blockIndex;
		if (InterlockedCompareExchange(&m_pBuf->m_Available, pBlock->Next, blockIndex) == blockIndex)
			break;
	}
	
	// Set the amount we can write
	DWORD written = amount;
	if (written > IPC_BLOCK_SIZE) written = IPC_BLOCK_SIZE;
	pBlock->Amount = written;
	
	// Copy the data to the block
	if (written > 0)
		memcpy(pBlock->Data, pBuff, written);

	// Loop attempting to add the block to
	// the singlely linked list
	for (;;)
	{
		LONG iFirst = pBlock->Next = m_pBuf->m_Filled;
		if (InterlockedCompareExchange(&m_pBuf->m_Filled, blockIndex, iFirst) == iFirst) break;
	}

	// Signal the event
	SetEvent(m_hSignal);

	// Return the amount written
	return written;
};

DWORD osIPC::Client::write(void *pBuff, DWORD amount, DWORD dwTimeout)
{
	// Perform a quick check
	if (amount <= 0) return 0;

	// Enter a continous loop
	for (;;)
	{
		// Attempt to send the data
		DWORD written = write(pBuff, amount);
		if (written > 0) return written;

		// Wait for some available data and try again
		WaitForSingleObject(m_hAvail, dwTimeout);
	}
};

bool osIPC::Client::waitAvailable(DWORD dwTimeout)
{
	// Wait on the available event
	if (WaitForSingleObject(m_hAvail, dwTimeout) != WAIT_OBJECT_0)
		return false;

	// Success
	return true;
};