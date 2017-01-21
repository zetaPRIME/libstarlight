#include "FSHelper.h"

#include <sstream>

#include <unistd.h>
#include <sys/stat.h>

using std::string;
using std::stringstream;
using std::getline;

using starlight::util::FSHelper;

void FSHelper::AssertDirectory(const string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) mkdir(path.c_str(), 0);
}

void FSHelper::AssertDirPath(const string& path) {
    struct stat st;
    stringstream stm(path);
    string ac(path.length() + 16, ' ');
    string tk = "";
    ac.clear();
    
    while (getline(stm, tk, '/')) {
        ac.append(tk); ac.append("/");
        if (tk.back() == ':') continue; // no further processing on "sdmc:/" etc.
        if (stat(ac.c_str(), &st) != 0) mkdir(ac.c_str(), 0);
    }
}
