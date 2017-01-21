#pragma once
#include "starlight/_global.h"

#include <string>

namespace starlight {
    namespace util {
        class FSHelper {
        private:
            //
            
        public:
            FSHelper() = delete;
            
            static void AssertDirectory(const std::string& path);
            static void AssertDirPath(const std::string& path);
        };
    }
}
