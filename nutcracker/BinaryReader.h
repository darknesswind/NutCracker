
#pragma once

#include <iostream>
#include <algorithm>
#include "Errors.h"


// ************************************************************************************************************************************
class BinaryReader
{
private:
	std::istream& _in;

	// Delete default methods
	BinaryReader();
	BinaryReader( const BinaryReader& );
	BinaryReader& operator = ( const BinaryReader& );

public:
	explicit BinaryReader( std::istream& in )
	: _in(in)
	{
	}


	// ******************************************************************************
	unsigned int ReadUInt32( void )
	{
		unsigned int value;

		//static_assert(sizeof(value) == 4, "Invalid size of int type");

		_in.read((char*)&value, 4);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	int ReadInt32( void )
	{
		int value;

		//static_assert(sizeof(value) == 4, "Invalid size of int type");

		_in.read((char*)&value, 4);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	unsigned short int ReadUInt16( void )
	{
		unsigned short int value;

		//static_assert(sizeof(value) == 2, "Invalid size of short int type");

		_in.read((char*)&value, 2);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	short int ReadInt16( void )
	{
		short int value;

		//static_assert(sizeof(value) == 2, "Invalid size of short int type");

		_in.read((char*)&value, 2);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	char ReadByte( void )
	{
		char value;

		//static_assert(sizeof(value) == 1, "Invalid size of char type");

		_in.read((char*)&value, 1);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	float ReadFloat32( void )
	{
		float value;

		//static_assert(sizeof(value) == 4, "Invalid size of float type");

		_in.read((char*)&value, 4);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	double ReadFloat64( void )
	{
		double value;

		//static_assert(sizeof(value) == 8, "Invalid size of double type");

		_in.read((char*)&value, 8);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	bool ReadBool( void )
	{
		bool value;

		_in.read((char*)&value, sizeof(bool));

		if (_in.fail())
			throw Error("I/O Error while reading from file.");

		return value;
	}


	// ******************************************************************************
	void Read( void* buffer, int size )
	{
		if (size < 1)
			return;

		_in.read((char*)buffer, size);

		if (_in.fail())
			throw Error("I/O Error while reading from file.");
	}


	// ******************************************************************************
	void ConfirmOnPart( void )
	{
		if (ReadInt32() != 'PART')
			throw Error("Bad format of source binary file (PART marker was not match).");
	}


	// ******************************************************************************
	void ReadSQString( std::string& str )
	{
		int len = ReadInt32();
		str.clear();
		str.reserve(len);

		while(len > 0)
		{
			char buffer[128];
			int chunk = std::min(128, len);

			Read(buffer, chunk);

			str.append(buffer, chunk);
			len -= chunk;
		}
	}


	// ******************************************************************************
	void ReadSQStringObject( std::string& str )
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
};
