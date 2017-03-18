#pragma once

// ******************************************************************************
struct indent
{
	int _n;

	indent( int n ) : _n(n) {}

	friend std::wostream& operator << (std::wostream& os, const indent& _i)
	{
		for(int i = 0; i < _i._n; ++i)
			os << '\t';

		return os;
	}
};


// ******************************************************************************
struct spaces
{
	int _n;

	spaces( int n ) : _n(n) {}

	friend std::wostream& operator<< (std::wostream& os, const spaces& _i)
	{
		for(int i = 0; i < _i._n; ++i)
			os << ' ';

		return os;
	}
};

// ******************************************************************************
inline void AssureIndents( LString& text, int n )
{
	LString::size_type pos = 0;
	LString indents(n, L'\t');

	while (pos < text.size())
	{
		pos = text.indexOf(L'\n', pos);
		if (pos == LString::npos)
			break;

		pos += 1;
		text.insert(pos, indents);
		pos += n;
	}
}
