#pragma once
#include "..\\ActinidiaPacker\\TiXML\\tinyxml.h"
#pragma comment(lib, "..\\ActinidiaPacker\\TiXML\\tinyxml.lib")
#include "..\\ActinidiaUnpacker\\ActinidiaUnpacker.h"
#include <codecvt>

bool bDirectMode = false;		// load resources from directory directly

LPCSTR szIndexFile = "res.dat";
LPCSTR szPackFile = "res.pak";
FILE *pIndexFile, *pPackFile;	// not use in direct mode
TiXmlDocument index;

LPBYTE ResStack[1000];			// store resources of a single map
unsigned long iter = 0;			// stack iterator


inline bool InitLoader()
{
	assert(false == bDirectMode);

	if (0 == fopen_s(&pIndexFile, szIndexFile, "rb") && loadEncryptedXML(&index, pIndexFile))
	{
		if (fopen_s(&pPackFile, szPackFile, "rb"))
			return false;	// cannot find package
		else
			return true;
	}
	else
		return false;		// cannot access specific path
}


long GetFileFromPackage(LPCSTR lpUTF8FilePath, LPBYTE* ppMem)
{
	*ppMem = (LPBYTE)malloc(32 * 1024 * 1024);	// 32MB
	if (NULL == ppMem)	return -1;	// fail

	fseek(pPackFile, 0, SEEK_SET);	// fflush

	long size = AGetRes<LPBYTE>(lpUTF8FilePath, reinterpret_cast<TiXmlElement*>(&index), pPackFile, *ppMem,
		[](short ch, LPBYTE& pMem)->void {*pMem = (BYTE)ch;++pMem;}
	);
	
	if (-1 == size)
	{
		free(*ppMem);
		return -1;	// fail, cannot find specific res
	}
	else
	{
		realloc(*ppMem, size);
		ResStack[iter++] = *ppMem;	// succuss, push
		return size;
	}
}

long GetDirectFile(LPCSTR lpUTF8FilePath, LPBYTE* ppMem)
{
	// UTF-8 to UTF-16
	std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
	std::wstring szFileToGet;
	try {
		szFileToGet = cvt.from_bytes(lpUTF8FilePath);
	}
	catch (std::range_error re) {
		return -1;
	}

	FILE* pFileToGet;
	if (_wfopen_s(&pFileToGet, szFileToGet.c_str(), L"rb"))
		return -1;

	fseek(pFileToGet, 0, SEEK_END);
	long filelen = ftell(pFileToGet);
	fseek(pFileToGet, 0, SEEK_SET);

	LPBYTE pMem = *ppMem = (LPBYTE)malloc(filelen);
	if (NULL == pMem) {
		fclose(pFileToGet);
		return -1;	// fail
	}
	
	ResStack[iter++] = pMem;		// succuss, push

	short ch;
	while ((ch = fgetc(pFileToGet)) != EOF)
	{
		*pMem = (BYTE)ch;
		++pMem;
	}

	fclose(pFileToGet);
	return filelen;
}


/*
 *	Allocate memory and read resources.
 *	
 *	@param		szFileToGet		UTF-8 filepath
 *	@param		ppMem			pointer to memory address pointer
 *	@return		Memory size, -1 for fail.
 */
inline long GetFile(LPCSTR szFileToGet, LPBYTE* ppMem)
{
	if (bDirectMode)
		return GetDirectFile(szFileToGet, ppMem);
	else
		return GetFileFromPackage(szFileToGet, ppMem);
}
