#include "stdafx.h"
#include "SqObject.h"
using namespace std;


// ***********************************************************************************************************************
void PrintEscapedString(wostream& out, const LString& str)
{
	for( auto iter = str.begin(); iter != str.end(); ++iter )
	{
		switch(*iter)
		{
			case '\r':	out << "\\r";			break;
			case '\n':	out << "\\n";			break;
			case '\t':	out << "\\t";			break;
			case '\v':	out << "\\v";			break;
			case '\a':	out << "\\a";			break;
			case '\0':	out << "\\0";			break;
			case '\\':	out << "\\\\";			break;
			
			default:
				out << *iter;
				break;
		}
	}
}

// ***********************************************************************************************************************
void SqObject::Load( BinaryReader& reader )
{
	SQObjectType type = (SQObjectType)reader.ReadInt32();
	switch(type)
	{
		default:
			throw Error("Unknown type of object in source binary file: 0x%08X", type);

		case OT_NULL:
			m_string.clear();
			m_integer = 0;
			break;

		case OT_STRING:
			reader.ReadSQString(m_string);
			break;

		case OT_INTEGER:
		case OT_BOOL:
			m_integer = reader.ReadUInt32();
			break;

		case OT_FLOAT:
			m_float = reader.ReadFloat32();
			break;
	}

	m_type = type;
}


// ***********************************************************************************************************************
int SqObject::GetType( void ) const
{
	return m_type;
}


// ***********************************************************************************************************************
const char* SqObject::GetTypeName( void ) const
{
	switch(m_type)
	{
		case 0:				return "Empty";
		case OT_NULL:		return "NULL";
		case OT_STRING:		return "String";
		case OT_INTEGER:	return "Integer";
		case OT_BOOL:		return "Bool";
		case OT_FLOAT:		return "Float";
		default:			return "Unknown";
	}
}


// ***********************************************************************************************************************
const LString& SqObject::GetString(void) const
{
	if (m_type != OT_STRING && m_type != OT_NULL)
		throw Error("Request of String in object of type %s.", GetTypeName());

	return m_string;
}


// ***********************************************************************************************************************
unsigned int SqObject::GetInteger( void ) const
{
	if (m_type != OT_INTEGER && m_type != OT_NULL && m_type != OT_BOOL)
		throw Error("Request of Integer in object of type %s.", GetTypeName());

	return m_integer;
}


// ***********************************************************************************************************************
float SqObject::GetFloat( void ) const
{
	if (m_type != OT_FLOAT && m_type != OT_NULL)
		throw Error("Request of Float in object of type %s.", GetTypeName());

	return m_float;
}

// ***********************************************************************************************************************
bool SqObject::operator == ( const SqObject& other ) const
{
	if (m_type != other.m_type)
		return false;

	switch(m_type)
	{
	case 0:
	case OT_NULL:
		return true;

	case OT_STRING:
		return m_string == other.m_string;
		
	case OT_INTEGER:
		return m_integer == other.m_integer;

	case OT_FLOAT:
		return std::abs(m_float - other.m_float) < 0.00001f;
	}

	return false;
}

// ***********************************************************************************************************************
wostream& operator<< (wostream& os, const SqObject& obj)
{
	switch (obj.m_type)
	{
	default:
		os << "<Unknown>";
		break;

	case 0:
		os << "<Empty>";
		break;

	case OT_NULL:
		os << "NULL";
		break;

	case OT_STRING:
		os << '\"';
		PrintEscapedString(os, obj.m_string);
		os << '\"';
		break;

	case OT_INTEGER:
		os << obj.m_integer;
		break;

	case OT_FLOAT:
		os << std::showpoint << obj.m_float;
		break;

	}
	return os;
}
