#pragma once
#include "starlight/_global.h"

#include <string>
#include <vector>
#include <ios>
#include <iosfwd>

namespace starlight {
    namespace util {
        class Path {
        private:
            std::string strpath;
            
        public:
            // vars...
            //
            
            Path();
            Path(const std::string& path, bool noverify = false);
            Path(const Path& path);
            ~Path();
            
            // navigation
            Path Up(int levels = 1);
            Path Combine(const std::string& token);
            
            // check
            bool Exists();
            bool IsFile();
            bool IsDirectory();
            
            // operation
            Path& CreateDirectory();
            std::fstream Open(std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
            std::ifstream OpenI();
            std::ofstream OpenO();
            
            // semi-obsolete but whatever, it can stay for now
            inline bool IsDirPath() const { return strpath.back() == '/'; }
            
            inline bool operator ==(const Path& o) const { return strpath == o.strpath; }
            inline bool operator !=(const Path& o) const { return strpath != o.strpath; }
            
            inline operator std::string() const { return strpath; }
        };
    }
}
