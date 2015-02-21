
#pragma once

#include <iostream>
#include <algorithm>
#include "Errors.h"
#include <QTextCodec>
#include <QDataStream>

// ************************************************************************************************************************************
typedef void(*ReaderHooker)(void* obj, void* buffer, int size, bool bString);
class BinaryReader
{
private:
	QIODevice* m_pDevice;

	static ReaderHooker fnHook;
	static void* s_hookObj;
	static QTextCodec* s_srcCodec;

	// Delete default methods
	BinaryReader();
	BinaryReader( const BinaryReader& );
	BinaryReader& operator = ( const BinaryReader& );

public:
	explicit BinaryReader(QIODevice* in)
	: m_pDevice(in)
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

		qint64 nReaded = m_pDevice->read((char*)buffer, size);

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
	void ReadSQString( QString& str )
	{
		static QByteArray buff;
		int len = ReadInt32();
		buff.resize(len);
		Read((void*)buff.data(), len, true);

		str = s_srcCodec->toUnicode(buff);
	}


	// ******************************************************************************
	void ReadSQStringObject(QString& str)
	{
		static const int StringObjectType = 0x10 | 0x08000000;
		static const int NullObjectType = 0x01 | 0x01000000;

		int type = ReadInt32();

		if (type == StringObjectType)
			ReadSQString(str);
		else if (type == NullObjectType)
			str.clear();
		else
			throw Error("Expected string object not found in source binary file.");
	}

	static void SetReaderHook(ReaderHooker fn, void* obj)
	{
		fnHook = fn;
		s_hookObj = s_hookObj;
	}
	static void SetCodec(const char* pName)
	{
		s_srcCodec = QTextCodec::codecForName(pName);
	}
};

__declspec(selectany) QTextCodec* BinaryReader::s_srcCodec = QTextCodec::codecForName("Shift-JIS");

__declspec(selectany) void* BinaryReader::s_hookObj = nullptr;

__declspec(selectany) ReaderHooker BinaryReader::fnHook = nullptr;
