#ifndef POTTOUUID_H_
#define POTTOUUID_H_
#pragma once

#include <assert.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <iterator>
#include <string>
#include <cctype>

/// <summary>
/// The potto namespace.
/// </summary>
namespace Potto
{
#ifdef POTTO_USE_GUID
	/// <summary>
	/// The UUID class.
	/// </summary>
	class PottoUuid
	{
	public:
		uint32_t Data1;
		uint16_t Data2;
		uint16_t Data3;
		uint8_t  Data4[8];

		/// <summary>
		/// Constructs GUID from string.
		/// </summary>
		PottoUuid(const char* id)
		{
			assert(id);
			uint32_t  d1 = 0;
			uint16_t d2 = 0, d3 = 0;
			uint8_t d4[8] = { 0 };
#pragma warning(push)
#pragma warning(disable: 4996)
			int err = sscanf(id,
				"%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
				&d1, &d2, &d3, &d4[0], &d4[1], &d4[2], &d4[3], &d4[4], &d4[5], &d4[6], &d4[7]);
#pragma warning(pop)
			assert(11 == err);
			Data1 = d1; Data2 = d2; Data3 = d3;
			for (int i = 0; i < 8; i++) Data4[i] = d4[i];
		}

		/// <summary>
		/// Converts to string.
		/// </summary>
		std::string ToString() const
		{
			std::vector<char> buf(37);
#pragma warning(push)
#pragma warning(disable: 4996)
			int err = sprintf(buf.data(),
				"%08lx-%04hx-%04hx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
				Data1, Data2, Data3, Data4[0], Data4[1], Data4[2], Data4[3], Data4[4], Data4[5], Data4[6], Data4[7]);
#pragma warning(pop)
			assert(36 == err);
			return buf.data();
		}

		/// <summary>
		/// 
		/// </summary>
		bool operator <(const PottoUuid& rhs) const
		{
			return (memcmp(this, &rhs, sizeof(PottoUuid)) < 0);
		}
	} PottoUuid;

	/// <summary>
	/// 
	/// </summary>
	inline bool IsEqualUuid(const PottoUuid& a, const PottoUuid& b)
	{
		return (0 == memcmp(&a, &b, sizeof(PottoUuid)));
	}

#else
	/// <summary>
	/// The UUID class.
	/// </summary>
	class PottoUuid : public std::string
	{
	public:
		/// <summary>
		/// 
		/// </summary>
		using std::string::string;

		/// <summary>
		/// Converts to string.
		/// </summary>
		std::string ToString() const
		{
			std::string s;
			std::for_each(this->begin(), this->end(),
				[&s](const char& c) { if ('{' != c && '}' != c) s.push_back(std::tolower(c)); });
			return s;
		}

		bool operator<(const PottoUuid& id) const
		{
			std::string self;
			std::for_each(this->begin(), this->end(),
				[&self](const char& c) { if ('{' != c && '}' != c) self.push_back(std::tolower(c)); });

			std::string other;
			std::for_each(id.begin(), id.end(),
				[&other](const char& c) { if ('{' != c && '}' != c) other.push_back(std::tolower(c)); });

			return self < other;
		}
	};

	/// <summary>
	/// Checks whether the two UUIDs are identical.
	/// </summary>
	inline bool IsEqualUuid(const PottoUuid& a, const PottoUuid& b)
	{
		if (a.empty() || b.empty())
			return false;

		std::string aa;
		std::for_each(a.begin(), a.end(),
			[&aa](const char& c) { if ('{' != c && '}' != c) aa.push_back(std::tolower(c)); });

		std::string bb;
		std::for_each(b.begin(), b.end(),
			[&bb](const char& c) { if ('{' != c && '}' != c) bb.push_back(std::tolower(c)); });

		if (aa.length() != bb.length())
			return false;

		return (0 == aa.compare(bb));
	}
#endif

	/// <summary>
	/// PottoUuid list.
	/// </summary>
	typedef std::vector<PottoUuid> PottoUuidList;
}

/// <summary>
/// Gets the id of interface.
/// </summary>
#define IIDOF(x) IID_##x

/// <summary>
/// Get the id of class.
/// </summary>
#define CLSIDOF(x) CLSID_##x

#endif // POTTOUUID_H_