#ifndef UTIL_H
#define UTIL_H

#include <ostream>
#include <string>

namespace detail {
void streamEscape(std::ostream&, const char* c);
}

inline std::ostream& streamEscape(std::ostream& s, const char* c)
{ ::detail::streamEscape(s, c); return s; }

inline std::ostream& streamEscape(std::ostream& s, const std::string& c)
{ ::detail::streamEscape(s, c.c_str()); return s; }

#endif // UTIL_H
