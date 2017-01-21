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
            
            Path(const std::string& path, bool noverify = false);
            Path(const Path& path);
            ~Path() = default;
            
            Path Up(int levels = 1);
            Path Combine(const std::string& token);
            
            // semi-obsolete but whatever, it can stay for now
            inline bool IsDirPath() const { return strpath.back() == '/'; }
            
            inline operator std::string () const { return strpath; }
        };
    }
}
