// Utilities file 

// Utilities
int atox(const char *s)
{
    int x= 0;
    while (*s) {
        x= x*16+(*s>'9'?(toupper(*s)-'A'+10):*s-'0');
        s++;
    }
    return x;
}

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}
