#pragma once

#ifndef IPC_H
#define IPC_H

// Definitions
#define IPC_BLOCK_COUNT			128
#define IPC_BLOCK_SIZE			4096

#define IPC_MAX_ADDR			256

// ---------------------------------------
// -- Inter-Process Communication class --
// ---------------------------------------------------------------
// Provides intercommunication between processes and there threads
// ---------------------------------------------------------------
class osIPC
{
public:
	// Block that represents a piece of data to transmit between the client and server
	struct Block
	{
		// Variables
		volatile LONG Next;	// Next block in the single linked list
		DWORD Amount; // Amount of data help in this block
		BYTE Data[IPC_BLOCK_SIZE]; // Data contained in this block
	};

private:
	// Shared memory buffer that contains everything required to transmit
	// data between the client and server
	struct MemBuff
	{
		volatile LONG m_Available; // List of available blocks
		volatile LONG m_Filled; // List of blocks that have been filled with data
		Block m_Blocks[IPC_BLOCK_COUNT]; // Array of buffers that are used in the communication
	};

	// ID Generator
	static DWORD GetID(void)
	{
		// Generate an ID and return id
		static volatile LONG id = 1;
		return (DWORD)InterlockedIncrement(&id);
	};

public:
	// Server class
	class Server
	{
	public:
		// Construct / Destruct
		Server();
		~Server();

	private:
		// Internal variables
		char				*m_sAddr;		// Address of this server
		HANDLE				m_hMapFile;		// Handle to the mapped memory file
		HANDLE				m_hSignal;		// Event used to signal when data exists
		HANDLE				m_hAvail;		// Event used to signal when some blocks become available
		MemBuff				*m_pBuf;		// Buffer that points to the shared memory

	public:
		// Exposed functions
		DWORD read(void *pBuff, DWORD buffSize, DWORD timeout = INFINITE);
		char* getAddress(void) { return m_sAddr; };

		void create(void);
		void close(void);
	};

	// Client class
	class Client
	{
	public:
		// Construct / Destruct
		Client(void);
		Client(char *connectAddr);
		~Client();

	private:
		// Internal variables
		char *m_sAddr; // Address of this server
		HANDLE m_hMapFile; // Handle to the mapped memory file
		HANDLE m_hSignal; // Event used to signal when data exists
		HANDLE m_hAvail; // Event used to signal when some blocks become available
		MemBuff *m_pBuf; // Buffer that points to the shared memory

	public:
		// Exposed functions
		DWORD write(void *pBuff, DWORD amount); // Writes to the buffer
		DWORD write(void *pBuff, DWORD amount, DWORD dwTimeout); // Writes to the buffer
		bool waitAvailable(DWORD dwTimeout = INFINITE); // Waits until some blocks become available

		Block* getBlock(void); // Gets a block
		void postBlock(Block *pBlock); // Posts a block to be processed				

		// Functions
		BOOL IsOk(void) { if (m_pBuf) return true; else return false; };
	};
};

#endif