#include "stdafx.h"
#include "LFile.h"

bool LFile::openRead(CStrPtr pFileName)
{
	if (!open(pFileName, fopen_s, "rb"))
		return false;

	fseek(m_hFile, 0, SEEK_END);
	fgetpos(m_hFile, &m_size);
	fseek(m_hFile, 0, SEEK_SET);
	return true;
}

bool LFile::openRead(CWStrPtr pFileName)
{
	if (!open(pFileName, _wfopen_s, L"rb"))
		return false;

	fseek(m_hFile, 0, SEEK_END);
	fgetpos(m_hFile, &m_size);
	fseek(m_hFile, 0, SEEK_SET);
	return true;
}

bool LFile::openWrite(CStrPtr pFileName)
{
	return open(pFileName, fopen_s, "wb");
}

bool LFile::openWrite(CWStrPtr pFileName)
{
	return open(pFileName, _wfopen_s, L"wb");
}
