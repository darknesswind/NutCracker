#pragma once


// ****************************************************************************************************************************
class SqObject
{
private:
	SQObjectType m_type;
	LString m_string;

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

	const LString& GetString( void ) const;
	unsigned int GetInteger( void ) const;
	float GetFloat( void ) const;

	bool operator == ( const SqObject& other ) const;
	bool operator != ( const SqObject& other ) const
	{
		return !(operator == (other));
	}

	friend std::wostream& operator<< (std::wostream& os, const SqObject& obj);
};
