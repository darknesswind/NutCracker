
#pragma once

#include "Errors.h"
#include "BinaryReader.h"

#define SQOBJECT_REF_COUNTED	0x08000000
#define SQOBJECT_NUMERIC		0x04000000
#define SQOBJECT_DELEGABLE		0x02000000
#define SQOBJECT_CANBEFALSE		0x01000000

// #define _RT_MASK 0x00FFFFFF
// #define _RAW_TYPE(type) (type&_RT_MASK)

#define _RT_NULL			0x00000001
#define _RT_INTEGER			0x00000002
#define _RT_FLOAT			0x00000004
#define _RT_BOOL			0x00000008
#define _RT_STRING			0x00000010
#define _RT_TABLE			0x00000020
#define _RT_ARRAY			0x00000040
#define _RT_USERDATA		0x00000080
#define _RT_CLOSURE			0x00000100
#define _RT_NATIVECLOSURE	0x00000200
#define _RT_GENERATOR		0x00000400
#define _RT_USERPOINTER		0x00000800
#define _RT_THREAD			0x00001000
#define _RT_FUNCPROTO		0x00002000
#define _RT_CLASS			0x00004000
#define _RT_INSTANCE		0x00008000
#define _RT_WEAKREF			0x00010000
#define _RT_OUTER			0x00020000

// ****************************************************************************************************************************
class QTextStream;
class SqObject
{
public:
	enum SQObjectType : int
	{
		OT_NULL = (_RT_NULL | SQOBJECT_CANBEFALSE),
		OT_INTEGER = (_RT_INTEGER | SQOBJECT_NUMERIC | SQOBJECT_CANBEFALSE),
		OT_FLOAT = (_RT_FLOAT | SQOBJECT_NUMERIC | SQOBJECT_CANBEFALSE),
		OT_BOOL = (_RT_BOOL | SQOBJECT_CANBEFALSE),
		OT_STRING = (_RT_STRING | SQOBJECT_REF_COUNTED),
		OT_TABLE = (_RT_TABLE | SQOBJECT_REF_COUNTED | SQOBJECT_DELEGABLE),
		OT_ARRAY = (_RT_ARRAY | SQOBJECT_REF_COUNTED),
		OT_USERDATA = (_RT_USERDATA | SQOBJECT_REF_COUNTED | SQOBJECT_DELEGABLE),
		OT_CLOSURE = (_RT_CLOSURE | SQOBJECT_REF_COUNTED),
		OT_NATIVECLOSURE = (_RT_NATIVECLOSURE | SQOBJECT_REF_COUNTED),
		OT_GENERATOR = (_RT_GENERATOR | SQOBJECT_REF_COUNTED),
		OT_USERPOINTER = _RT_USERPOINTER,
		OT_THREAD = (_RT_THREAD | SQOBJECT_REF_COUNTED),
		OT_FUNCPROTO = (_RT_FUNCPROTO | SQOBJECT_REF_COUNTED), //internal usage only
		OT_CLASS = (_RT_CLASS | SQOBJECT_REF_COUNTED),
		OT_INSTANCE = (_RT_INSTANCE | SQOBJECT_REF_COUNTED | SQOBJECT_DELEGABLE),
		OT_WEAKREF = (_RT_WEAKREF | SQOBJECT_REF_COUNTED),
		OT_OUTER = (_RT_OUTER | SQOBJECT_REF_COUNTED) //internal usage only
	};

private:
	SQObjectType m_type;
	QString m_string;

	union
	{
		unsigned int m_integer;
		float m_float;
	};

public:
	SqObject()
	{
		m_type = OT_NULL;
		m_integer = 0;
	}

	void Load( BinaryReader& reader );

	int GetType( void ) const;
	const char* GetTypeName( void ) const;

	const QString& GetString( void ) const;
	unsigned int GetInteger( void ) const;
	float GetFloat( void ) const;

	bool operator == ( const SqObject& other ) const;
	bool operator != ( const SqObject& other ) const
	{
		return !(operator == (other));
	}

	friend QTextStream& operator << (QTextStream& os, const SqObject& obj);
};
