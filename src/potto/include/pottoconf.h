#ifndef POTTOCONF_H_
#define POTTOCONF_H_
#pragma once

// 
#if defined(_MSC_VER)
#	define POTTO_DECLARE_NOVTABLE		__declspec(novtable)

#elif defined(__GNUC__)
#	define POTTO_DECLARE_NOVTABLE

#endif


#endif // POTTOCONF_H_