#pragma once
#include "starlight/_global.h"

#include <string>
#include <vector>
#include <iosfwd>

namespace starlight {
    namespace util {
        class Path {
        private:
            std::string strpath;
            
        public:
            // vars...
            static std::string destructed;
            
            Path(const std::string& path, bool noverify = false);
            Path(const Path& path);
            ~Path();
            
            // navigation
            Path Up(int levels = 1);
            Path Combine(const std::string& token);
            
            // operation
            Path& CreateDirectory();
            
            // semi-obsolete but whatever, it can stay for now
            inline bool IsDirPath() const { return strpath.back() == '/'; }
            
            inline operator std::string () const { return strpath; }
        };
    }
}
