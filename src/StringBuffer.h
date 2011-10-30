#ifndef _STRINGBUFFER_H
#define _STRINGBUFFER_H

#include <string>
#include <streambuf>

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <cassert>
#include <cstdlib>

template <int N>
class StringBufferN
{
public:
	StringBufferN();

	bool empty() const;
	size_t size() const;

	void append(char c);
	void append(const char *str, size_t len);
	void append(const char *str);
	void append(const std::string &s);
	void format(const char *format, ...) __attribute((format(printf,1,2)));

	StringBufferN &operator+=(char c);
	StringBufferN &operator+=(const std::string &str);
	StringBufferN &operator+=(const char *str);

	const std::string &str() const;
	const char *c_str() const;

private:
	mutable std::string m_overflow;
	mutable size_t m_fixedSize;
	mutable char m_fixed[N];
};

typedef StringBufferN<128> StringBuffer;

// ------

template <int N>
inline StringBufferN<N>::StringBufferN():
	m_fixedSize(0)
{
	this->fixed[0] = '\0';
}

template <int N>
inline bool StringBufferN<N>::empty() const
{
	return this->overflow.empty() && (this->m_fixedSize == 0);
}

template <int N>
inline size_t StringBufferN<N>::size() const
{
	size_t sz = this->overflow.size();
	return (sz ? sz : m_fixedSize);
}

template <int N>
inline void StringBufferN<N>::append(char c)
{
	if (m_overflow.empty()) {
		assert(m_fixedSize < N);
		m_fixed[m_fixedSize++] = c;
		if (m_fixedSize == N) {
			std::string(m_fixed, m_fixedSize).swap(m_overflow);
			m_fixedSize = 0;
			m_fixed[0] = '\0';
		}
	} else
		m_overflow += c;
}

template <int N>
inline void StringBufferN<N>::append(const char *s, size_t len)
{
	if (m_overflow.empty()) {
		size_t total_len = m_fixedSize + len;
		if (total_len < N) {
			std::memcpy(&m_fixed[m_fixedSize], s, len);
			m_fixedSize = total_len;
			m_fixed[total_len] = '\0';
		} else {
			m_overflow.reserve(total_len);
			m_overflow.append(m_fixed, m_fixedSize);
			m_overflow.append(s, len);
		}
	} else
		m_overflow.append(s, len);
}

template <int N>
inline void StringBufferN<N>::append(const std::string &s)
{ this->append(s.data(), s.size()); }

template <int N>
inline void StringBufferN<N>::append(const char *s)
{ this->append(s, std::strlen(s)); }

template <int N>
inline void StringBufferN<N>::format(const char *fmt, ...)
{
	size_t start = m_fixedSize;

	va_list args;
	va_start(args, fmt);
	int len = vsnprintf(&m_fixed[start], N-start, fmt, args);
	va_end(args);

	if (len < 0) { std::abort(); }

	if (start + len < N) {
		assert(m_fixed[start+len] == '\0'); // vsnprintf should have written the null terminator
		if (! m_overflow.empty()) {
			assert(start == 0);
			assert(m_fixedSize == 0);
			m_overflow.append(m_fixed, len);
			m_fixed[0] = '\0';
		}
	} else {
		if (m_overflow.empty() && start)
			m_overflow.append(m_fixed, start);
		start = m_overflow.size();
		m_overflow.resize(start + len + 1, '\0');
		va_start(args, fmt);
		len = vsnprintf(m_overflow.data() + start, len+1, fmt, args);
		va_end(args);
		assert(start + len + 1 == m_overflow.size());
		m_overflow.resize(start + len); // trim off trailing '\0'
		m_fixedSize = 0;
		m_fixed[0] = '\0';
	}
}

template <int N>
inline StringBufferN<N> &StringBufferN<N>::operator+=(char c)
{ this->append(c); return *this; }

template <int N>
inline StringBufferN<N> &StringBufferN<N>::operator+=(const std::string &s)
{ this->append(s); return *this; }

template <int N>
inline StringBufferN<N> &StringBufferN<N>::operator+=(const char *s)
{ this->append(s); return *this; }

template <int N>
inline const std::string &StringBufferN<N>::str() const
{
	if (m_overflow.empty() && (m_fixedSize > 0)) {
		std::string(m_fixed, m_fixedSize).swap(m_overflow);
		m_fixedSize = 0;
		m_fixed[0] = '\0';
	}
	return m_overflow;
}

template <int N>
inline const char *StringBufferN<N>::c_str() const
{
	return m_overflow.empty() ? m_fixed : m_overflow.c_str();
}

template <int N>
struct StringBufferIO : public std::streambuf
{
};

#endif