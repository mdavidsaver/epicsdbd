
#include "util.h"

namespace detail {
void streamEscape(std::ostream& s, const char* c)
{
    s.put('"');
    while(true) {
        char t=*c;
        if(t==0)
            break;
        else if(t<' ' || t>=0x7f)
            s<<int(t);
        else switch(t)
        {
        case '\\':
        case '"':
            s.put('\\');
        default:
            s.put(t);
        }
        c++;
    }
    s.put('"');
}
}//namespace detail
