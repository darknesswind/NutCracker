#pragma once
#include <string>
#include <sstream>

class LString : public std::wstring
{
	typedef std::wstring Base;
	typedef wchar_t* WStrPtr;
	typedef const wchar_t* CWStrPtr;
	typedef char* StrPtr;
	typedef const char* CStrPtr;
	typedef std::codecvt<wchar_t, char, mbstate_t> converter_type;
public:
	LString() = default;
	LString(CWStrPtr wcstr) : Base(wcstr) {}
	LString(CStrPtr cstr) { assign(cstr); }
	LString(Base&& base) : Base(base) {}
	LString(size_t n, wchar_t c) : Base(n, c) {}

	using Base::assign;
	using Base::append;

	LString& assign(CStrPtr pcstr, size_t size, const std::locale& locale);
	LString& assign(CStrPtr pcstr, size_t size) { return assign(pcstr, size, s_defaultLocale); }
	LString& assign(CStrPtr pcstr) { return assign(pcstr, strlen(pcstr)); }

	LString& append(char c) { Base::push_back(c); return (*this); }

	LString& setNum(int val, int base = 10);
	LString& setNum(unsigned int val, int base = 10);
	LString& setNum(float val, int prec = 6);

	std::string toUtf8() const;

	int toInt(bool* pOk = nullptr) const;
	double toNumber(bool* pOk = nullptr) const;

	size_type indexOf(wchar_t c, size_type begin = 0) const { return Base::find(c, begin); }
	LString mid(size_type off, size_type size = Base::npos) const { return Base::substr(off, size); }
public:
	static LString number(int val, int base = 10) { return LString().setNum(val, base); }
	static LString number(float val, int prec = 6) { return LString().setNum(val, prec); }
	static LString fromUtf8(const std::string& bytes);
	static void setLocal(std::locale& locale) { s_defaultLocale = locale; }

protected:
	static std::locale s_defaultLocale;
};

class LStrBuilder
{
	typedef wchar_t* WStrPtr;
	typedef const wchar_t* CWStrPtr;
	typedef char* StrPtr;
	typedef const char* CStrPtr;
public:
	LStrBuilder(CWStrPtr pPattern);
	LStrBuilder(CStrPtr pPattern);
	~LStrBuilder();

	operator LString() { return apply(); }
	void resetPattern(CWStrPtr pPattern);
	void resetPattern(CStrPtr pPattern);
	LString apply() const;

	LStrBuilder& arg(CWStrPtr val);
	LStrBuilder& arg(int val);
	LStrBuilder& arg(int val, size_t fieldWidth, int base, wchar_t fillChar);
	LStrBuilder& arg(float val);

	LStrBuilder& arg(const std::wstring& val) { return arg(val.c_str()); }

private:
	void reset();
	void analyzePattern();
	bool isFull();

private:
	LString m_pattern;

	struct ChpxInfo
	{
		size_t begin	= 0;
		size_t len		= 0;
		size_t argID		= 0;
	};
	std::vector<ChpxInfo> m_chpxes;
	std::vector<LString> m_args;
	size_t m_argCount = 0;
};
