#pragma once
#ifndef assert
#define assert(exp)
#endif
#define CheckSize(lhs, rhs) assert(lhs == rhs);

class LFile
{
public:
	LFile() {}
	~LFile() { if (m_hFile) fclose(m_hFile); }

	typedef const wchar_t* CWStrPtr;
	typedef const char* CStrPtr;

	bool openRead(CStrPtr pFileName);
	bool openRead(CWStrPtr pFileName);
	template <typename T> size_t readAs(T& var);
	template <typename T> size_t readAs(T* pBuf, size_t cnt);
	template <typename T> T read();

	bool openWrite(CStrPtr pFileName);
	bool openWrite(CWStrPtr pFileName);
	template <typename T> size_t write(T& var);
	template <typename T> size_t write(T* pBuf, size_t cnt);

	bool opened() const { return m_hFile != nullptr; }
	fpos_t size() const { return m_size; }
	bool eof() const { return (0 != feof(m_hFile)); }
	int error() const { return ferror(m_hFile); }

private:
	template <typename CharType, typename Function>
	bool open(const CharType* pFileName, Function fnOpen_s, const CharType* pMode);

private:
	FILE* m_hFile	{ nullptr };
	fpos_t m_size	{ 0 };
};

template <typename T>
size_t LFile::readAs(T& var)
{
	size_t sz = fread_s((void*)&var, sizeof(T), sizeof(T), 1, m_hFile);
	CheckSize(1, sz);
	return sz;
}

template <typename T>
size_t LFile::readAs(T* pBuf, size_t cnt)
{
	size_t sz = fread_s((void*)pBuf, sizeof(T) * cnt, sizeof(T), cnt, m_hFile);
	CheckSize(cnt, sz);
	return sz;
}

template <typename T>
T LFile::read()
{
	T res = 0;
	readAs(res);
	return res;
}

template <typename T>
size_t LFile::write(T& var)
{
	size_t sz = fwrite((void*)&var, sizeof(T), 1, m_hFile);
	CheckSize(sizeof(T), sz);
	return sz;
}

template <typename T>
size_t LFile::write(T* pBuf, size_t cnt)
{
	size_t sz = fwrite(pBuf, sizeof(T), cnt, m_hFile);
	CheckSize(sizeof(T) * cnt, sz);
	return sz;
}

template <typename CharType, typename Function>
bool LFile::open(const CharType* pFileName, Function fnOpen_s, const CharType* pMode)
{
	if (m_hFile)
		return false;

	errno_t err = fnOpen_s(&m_hFile, pFileName, pMode);
	return (0 == err);
}
