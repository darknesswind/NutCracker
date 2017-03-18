#pragma once
#include "Errors.h"
#include "LFile.h"

// ************************************************************************************************************************************
typedef void(*ReaderHooker)(void* obj, void* buffer, int size, bool bString);
class BinaryReader
{
private:
	LFile& m_file;

	static ReaderHooker fnHook;
	static void* s_hookObj;

	// Delete default methods
	BinaryReader() = delete;
	BinaryReader( const BinaryReader& ) = delete;
	BinaryReader& operator = ( const BinaryReader& ) = delete;

public:
	explicit BinaryReader(LFile& in)
		: m_file(in)
	{
	}

	// ******************************************************************************
	unsigned int	ReadUInt32( void ){ return ReadValue<unsigned int>(); }
	int				ReadInt32( void ){ return ReadValue<int>(); }
	unsigned short	ReadUInt16( void ){ return ReadValue<unsigned short>(); }
	short int		ReadInt16( void ){ return ReadValue<short int>(); }
	char			ReadByte( void ){ return ReadValue<char>(); }
	float			ReadFloat32( void ){ return ReadValue<float>(); }
	double			ReadFloat64( void ){ return ReadValue<double>(); }
	bool			ReadBool( void ) { return ReadValue<bool>(); }

	template <typename T>
	T ReadValue()
	{
		T value;

		Read((char*)&value, sizeof(T));

		return value;
	}

	// ******************************************************************************
	void Read( void* buffer, int size, bool bString = false )
	{
		if (size < 1)
			return;

		size_t nReaded = m_file.readAs((char*)buffer, size);

		if (nReaded < 0)
			throw Error("I/O Error while reading from file.");

		if (fnHook)
			fnHook(s_hookObj, buffer, size, bString);
	}


	// ******************************************************************************
	void ConfirmOnPart( void )
	{
		if (ReadInt32() != 'PART')
			throw Error("Bad format of source binary file (PART marker was not match).");
	}


	// ******************************************************************************
	void ReadSQString(LString& str)
	{
		static std::vector<char> buf;
		int len = ReadInt32();
		if (len < 0)
			len = 0;
		if (buf.size() < (size_t)len)
			buf.resize(len);

		Read((void*)buf.data(), len, true);

		str.assign(buf.data(), len);
	}


	// ******************************************************************************
	void ReadSQStringObject(LString& str)
	{
		int type = ReadInt32();

		if (type == OT_STRING)
			ReadSQString(str);
		else if (type == OT_NULL)
			str.clear();
		else
			throw Error("Expected string object not found in source binary file.");
	}

	static void SetReaderHook(ReaderHooker fn, void* obj)
	{
		fnHook = fn;
		s_hookObj = s_hookObj;
	}
	static void SetLocale(const char* pName)
	{
		try
		{
			LString::setLocal(std::locale(pName));
			setlocale(LC_ALL, pName);
		}
		catch (std::exception& e)
		{
			throw Error(e.what());
		}
	}
};

__declspec(selectany) void* BinaryReader::s_hookObj = nullptr;

__declspec(selectany) ReaderHooker BinaryReader::fnHook = nullptr;
