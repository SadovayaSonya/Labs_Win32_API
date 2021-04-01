#define BYTES_TO_READ 512
#define WDRIVE_PATH L"E:\\"
#define DRIVE_PATH "\\\\.\\E:"

#include "hex_print.h"

#include <bitset>
#include <iostream>
#include <assert.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>
#include <strsafe.h>

#define BITS_IN_BYTE 8
#define BOOT_LOCATION 0
#define BOOT_SIZE 6
#define ROOT_CLUSTER_OFFSET 0x2C
#define NUMBER_OF_RESERVED_SECTORS_OFFSET 0x0E
#define NUMBER_OF_FATS_OFFSET 0x10
#define SECTORS_PER_FAT_OFFSET 0x24
#define ROOT_ENT_COUNT_OFFSET 0x11

HANDLE hDrive;
DWORD dwBytesRead = 0;

template<class TYPE>
TYPE BytesToVar(BYTE* buffer)
{
	TYPE temp = TYPE();
	for (SIZE_T i = 0; i < sizeof(TYPE); i++)
		temp |= ((TYPE)(buffer[i])) << BITS_IN_BYTE * (sizeof(TYPE) - i - 1);

	return temp;
}

BYTE* GetSectorDump(UINT sectorNumber)
{
	BYTE* buff = new BYTE[BYTES_TO_READ];

	std::string device = DRIVE_PATH;

	std::ifstream infile(device.c_str(), std::ios::in | std::ios::binary);
	if (!infile)
	{
		std::cerr << "Device input could not be set!" << std::endl;
		exit(102);
	}

	infile.ignore(sectorNumber * BYTES_TO_READ);

	infile.read((char*)buff, BYTES_TO_READ);   // read in the structure length
	if (infile.gcount() == 0)
		exit(999);

	infile.close();
	return buff;
}

BOOL GetBPB()
{
	DWORD lpSectorsPerCluster;
	DWORD lpBytesPerSector;
	DWORD lpNumberOfFreeClusters;
	DWORD lpTotalNumberOfClusters;

	if (GetDiskFreeSpace(WDRIVE_PATH, &lpSectorsPerCluster, &lpBytesPerSector, &lpNumberOfFreeClusters, &lpTotalNumberOfClusters))
	{
		std::cout << "Количество секторов в кластере: " + std::to_string(lpSectorsPerCluster) << std::endl;
		std::cout << "Количество байт в секторе: " + std::to_string(lpBytesPerSector) << std::endl;
		std::cout << "Количество свободных кластеров: " + std::to_string(lpNumberOfFreeClusters) << std::endl;
		std::cout << "Общее количество кластеров: " + std::to_string(lpTotalNumberOfClusters) << std::endl;
		return TRUE;
	}
	return FALSE;
}

void ReverseBytes(BYTE* buffer, SIZE_T length)
{
	for (std::size_t i = 0; i < length / 2; i++)
		std::swap(buffer[i], buffer[length - i - 1]);
}

template<typename TYPE>
TYPE GetData(BYTE* buffer)
{
	ReverseBytes(buffer, sizeof(TYPE));
	return BytesToVar<TYPE>(buffer);
}

BOOL GetBootFATRoot()
{
	DWORD BPB_SectorsPerCluster;
	DWORD BPB_BytesPerSector;
	DWORD BPB_NumberOfFreeClusters;
	DWORD BPB_TotalNumberOfClusters;

	if (!GetDiskFreeSpace(WDRIVE_PATH, &BPB_SectorsPerCluster, &BPB_BytesPerSector, &BPB_NumberOfFreeClusters, &BPB_TotalNumberOfClusters))
		return FALSE;

	BYTE* temp = GetSectorDump(0);

	USHORT BPB_NumberOfReservedSectors = GetData<USHORT>(temp + NUMBER_OF_RESERVED_SECTORS_OFFSET);
	BYTE BPB_NumberOfFATs = GetData<BYTE>(temp + NUMBER_OF_FATS_OFFSET);
	UINT BPB_SectorsPerFAT = GetData<UINT>(temp + SECTORS_PER_FAT_OFFSET);
	UINT BPB_RootEntCount = GetData<UINT>(temp + ROOT_ENT_COUNT_OFFSET);

	UINT RootCluster = GetData<UINT>(temp + ROOT_CLUSTER_OFFSET);
	UINT FirstDataSector = BPB_NumberOfReservedSectors + (BPB_NumberOfFATs * BPB_SectorsPerFAT);
	UINT FirstSectorOfCluster = ((RootCluster - 2) * BPB_SectorsPerCluster) + FirstDataSector;

	UINT FATOffset = RootCluster * 4;
	UINT FATSectorNumber = BPB_NumberOfReservedSectors + (FATOffset / BPB_BytesPerSector);
	UINT FATEntOffset = FATOffset % BPB_BytesPerSector;
	BYTE* FAT = GetSectorDump(FATSectorNumber);


	UINT RootData = GetData<UINT>(FAT + FATEntOffset);
	UINT RootSectorsCount = BPB_SectorsPerCluster;
	while ((RootData & 0x000000FF) != 0xFF && (RootData & 0x000000FF) != 0xF8)
	{
		RootSectorsCount += BPB_SectorsPerCluster;
		RootData = GetData<UINT>(FAT + (RootData & 0x0FFFFFFF));
	}

	std::cout << "Местоположение BOOT: " << std::dec << BOOT_LOCATION << "-ой сектор" << std::endl;
	std::cout << "Размер BOOT: " << std::dec << BOOT_SIZE << " секторов" << std::endl;

	std::cout << "Местоположение FAT: " << std::dec << BPB_NumberOfReservedSectors << "-ой сектор" << std::endl;
	std::cout << "Размер FAT: " << std::dec << BPB_SectorsPerFAT << " секторов" << std::endl;

	std::cout << "Местоположение ROOT: " << std::dec << FirstSectorOfCluster << "-ой сектор" << std::endl;
	std::cout << "Размер ROOT: " << std::dec << RootSectorsCount << " секторов" << std::endl;

	delete[] FAT;
	delete[] temp;

	return TRUE;
}

int main()
{
	setlocale(LC_ALL, "RUS");

	UINT n;
	std::cout << "Введите номер сектора (от нуля): ";
	std::cin >> n;

	BYTE* temp = GetSectorDump(n);
	hex_dump(temp, BYTES_TO_READ, std::cout);
	delete[] temp;
	std::cout << std::endl;

	GetBPB();
	std::cout << std::endl;

	GetBootFATRoot();

	system("PAUSE");
	return 0;
}
