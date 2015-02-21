
#pragma once

#include <QTextStream>
// ******************************************************************************
struct indent
{
	int _n;

	indent( int n ) : _n(n) {}

	friend QTextStream& operator << (QTextStream& os, const indent& _i)
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

	friend QTextStream& operator<< (QTextStream& os, const spaces& _i)
	{
		for(int i = 0; i < _i._n; ++i)
			os << ' ';

		return os;
	}
};

// ******************************************************************************
inline void AssureIndents( QString& text, int n )
{
	QString::size_type pos = 0;
	QString indents(n, '\t');

	while (pos < text.size())
	{
		pos = text.indexOf('\n', pos);
		if (pos < 0)
			break;

		pos += 1;
		text.insert(pos, indents);
		pos += n;
	}
}
