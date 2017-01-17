// okay, this is super ugly, but as of right now devkitARM seems to be missing some string conversion functions :(

#pragma once

// !! UNCOMMENT if using devkitARM r45 or earlier !!
//#include <string>
//#include <sstream>

// add in missing string functions
#if defined(_GLIBCXX_STRING) && !defined(_GLIBCXX_USE_C99)
// !! UNCOMMENT if using devkitARM r45 or earlier !!
/*namespace std { // whee ugly hacks!
    template <typename T>
    std::string to_string(T value) {
        std::ostringstream os;
        os << value;
        return os.str();
    }
    int stoi(const string& str) {
       stringstream ss(str);
       int N;
       ss<<str;
       ss>>N;
       return N;
    }
    float strtof(const string& str) {
       stringstream ss(str);
       float N;
       ss<<str;
       ss>>N;
       return N;
    }
    double strtod(const string& str) {
       stringstream ss(str);
       double N;
       ss<<str;
       ss>>N;
       return N;
    }
    long double strtold(const string& str) {
       stringstream ss(str);
       long double N;
       ss<<str;
       ss>>N;
       return N;
    }
}//*/
#endif
