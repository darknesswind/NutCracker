#include "stdafx.h"
#include "LString.h"
#include <codecvt>
#include <map>

std::locale LString::s_defaultLocale;

LString& LString::assign(CStrPtr pcstr, size_t size, const std::locale& locale)
{
	clear();

	std::vector<wchar_t> buf(size);
	std::mbstate_t state = { 0 };
	const char* from_next = nullptr;
	wchar_t* to_next = nullptr;

	const converter_type& converter = std::use_facet<converter_type>(locale);
	const converter_type::result result = converter.in(
		state, pcstr, pcstr + size, from_next,
		buf.data(), buf.data() + buf.size(), to_next);

	assert(result == converter_type::ok || result == converter_type::noconv);
	Base::assign(buf.data(), buf.size());
	return (*this);
}

LString& LString::setNum(int val, int base /*= 10*/)
{
	const size_t buffsize = 33;
	wchar_t buff[buffsize] = { 0 };
	errno_t err = _itow_s(val, buff, base);
	assert(0 == err);
	Base::assign(buff);
	return (*this);
}

LString& LString::setNum(unsigned int val, int base /*= 10*/)
{
	const size_t buffsize = 33;
	wchar_t buff[buffsize] = { 0 };
	errno_t err = _ultow_s(val, buff, base);
	assert(0 == err);
	Base::assign(buff);
	return (*this);
}

LString& LString::setNum(float val, int prec /*= 6*/)
{
	int decimal, sign;
	char buff[_CVTBUFSIZE + 1] = { 0 };
	_fcvt_s(buff, val, prec, &decimal, &sign);
	buff[_CVTBUFSIZE] = 0;
	assign(buff);
	return (*this);
}

std::string LString::toUtf8() const
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(c_str());
}

int LString::toInt(bool* pOk) const
{
	int result = _wtoi(c_str());
	if (pOk)
		*pOk = (0 == errno);
	return result;
}

double LString::toNumber(bool* pOk) const
{
	double result = _wtof(c_str());
	if (pOk)
		*pOk = (0 == errno);
	return result;
}

LString LString::fromUtf8(const std::string& bytes)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(bytes);
}

//////////////////////////////////////////////////////////////////////////
LStrBuilder::LStrBuilder(CWStrPtr pPattern)
{
	resetPattern(pPattern);
}

LStrBuilder::LStrBuilder(CStrPtr pPattern)
{
	resetPattern(pPattern);
}

LStrBuilder::~LStrBuilder()
{

}

void LStrBuilder::resetPattern(CWStrPtr pPattern)
{
	m_pattern = pPattern;
	reset();
}

void LStrBuilder::resetPattern(CStrPtr pPattern)
{
	m_pattern = pPattern;
	reset();
}

void LStrBuilder::reset()
{
	m_argCount = 0;
	m_chpxes.clear();
	m_args.clear();
	analyzePattern();
}

LString LStrBuilder::apply() const
{
	if (m_chpxes.empty() || m_args.empty())
		return m_pattern;

	LString result;
	size_t pos = 0;
	CWStrPtr pBegin = m_pattern.c_str();
	CWStrPtr pPos = pBegin;
	for (auto iter = m_chpxes.begin(); iter != m_chpxes.end(); ++iter)
	{
		const ChpxInfo& info = *iter;

		size_t prevLen = pBegin + info.begin - pPos;
		result.append(pPos, prevLen);
		result.append(m_args[info.argID]);

		pPos = pBegin + info.begin + info.len;
	}

	if (pPos < pBegin + m_pattern.size())
		result.append(pPos);

	return result;
}

void LStrBuilder::analyzePattern()
{
	if (m_pattern.empty())
		return;

	const wchar_t* pBegin = m_pattern.c_str();
	const wchar_t* pEnd = pBegin + m_pattern.size();

	std::map<size_t, size_t> idMap;
	for (const wchar_t* pch = pBegin; pch < pEnd; ++pch)
	{
		if (*pch != L'%')
			continue;

		++pch;
		if (pch >= pEnd)
			break;

		const wchar_t ch = *pch;

		int num = ch - L'0';
		if (num <= 0 || num > 9)
			continue;

		const wchar_t* pArgBegin = pch - 1;
		if (pch + 1 < pEnd)
		{
			int num2 = pch[1] - L'0';
			if (num2 >= 0 && num2 <= 9)
			{
				++pch;
				num *= 10;
				num += num2;
			}
		}

		ChpxInfo info;
		info.begin = pArgBegin - pBegin;
		info.len = pch - pArgBegin + 1;
		info.argID = num;
		idMap[num] = 0;
		m_chpxes.push_back(info);
	}
	if (idMap.empty())
		return;

	size_t relID = 0;
	for (auto iter = idMap.begin(); iter != idMap.end(); ++iter, ++relID)
		iter->second = relID;

	for (auto iter = m_chpxes.begin(); iter != m_chpxes.end(); ++iter)
		iter->argID = idMap[iter->argID];
}

bool LStrBuilder::isFull()
{
	if (m_args.size() >= m_argCount)
	{
		assert(!"Argument number mismatch!");
		return true;
	}
	return false;
}

LStrBuilder& LStrBuilder::arg(CWStrPtr val)
{
	m_args.push_back(val);
	return (*this);
}

LStrBuilder& LStrBuilder::arg(int val)
{
	m_args.emplace_back(LString::number(val));
	return (*this);
}

LStrBuilder& LStrBuilder::arg(int val, size_t fieldWidth, int base, wchar_t fillChar)
{
	LString str = LString::number(val, base);
	if (str.size() < fieldWidth)
	{
		m_args.emplace_back(LString(fieldWidth, fillChar));
		memcpy(&m_args.back()[fieldWidth - str.size()], str.c_str(), str.size());
	}
	return (*this);
}

LStrBuilder& LStrBuilder::arg(float val)
{
	m_args.emplace_back(LString::number(val));
	return (*this);
}
