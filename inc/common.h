#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <iostream>
#include <iomanip>

const std::string ERR_PFX	= "Error: ";
const std::string WARN_PFX	= "Warning: ";
const std::string INFO_PFX	= "Info: ";
const std::string HEX_PFX	= "0x";
const std::string SEP		= ", ";

#define COUT	  std::cout
#define ENDL	  std::endl
#define FUNC	  STR(__PRETTY_FUNCTION__, 1) << ": "
#define HEX(n, w) HEX_PFX << std::hex << std::setw((w)) << std::setfill('0') << std::right << (n)
#define DEC(n, w) std::dec << std::setw((w)) << std::setfill(' ') << std::right << (n)
#define STR(s, w) std::setw((w)) << std::setfill(' ') << std::left << (s)
#define ERR(s)	  COUT << STR(ERR_PFX, 1) << FUNC << STR((s), 1)
#define WARN(s)	  COUT << STR(WARN_PFX, 1) << FUNC << STR((s), 1)
#define INFO(s)	  COUT << STR(INFO_PFX, 1) << STR((s), 1);

#ifdef DEBUG_GRFX
const std::string DBG_PFX	= "Debug: ";
#define DBG(s)    COUT << STR(DBG_PFX, 1) << FUNC << STR((s), 1)
#else
#define DBG(...)
#endif //DEBUG_GRFX

#endif // __COMMON_H__
