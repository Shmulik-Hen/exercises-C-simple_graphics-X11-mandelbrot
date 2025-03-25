#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

const std::string ERR_PFX	= "Error: ";
const std::string WARN_PFX	= "Warning: ";
const std::string INFO_PFX	= "Info: ";
const std::string HEX_PFX	= "0x";
const std::string SEP		= ", ";

#define ENDL	 std::endl
#define FUNC	 STR(__PRETTY_FUNCTION__, 1) << ": "
#define HEX(n,w) HEX_PFX << std::hex << std::setw((w)) << std::setfill('0') << std::right << (n)
#define DEC(n,w) std::dec << std::setw((w)) << std::setfill(' ') << std::right << (n)
#define FLT(n,p) std::setprecision(p) << std::fixed << (n)
#define STR(s,w) std::setw((w)) << std::setfill(' ') << std::left << (s)
#define ERR(s)	 std::cerr << STR(ERR_PFX, 1)  << FUNC << s << ENDL
#define WARN(s)	 std::cerr << STR(WARN_PFX, 1) << FUNC << s << ENDL
#define INFO(s)	 std::cout << STR(INFO_PFX, 1) << FUNC << s << ENDL

#ifdef DEBUG_GRFX
const std::string DBG_PFX	= "Debug: ";
#define DBG(s)	  std::cout << STR(DBG_PFX, 1) << FUNC << s << std::endl
#else
#define DBG(...)
#endif //DEBUG_GRFX

#endif // __COMMON_H__
