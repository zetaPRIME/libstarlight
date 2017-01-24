#include "Path.h"

#include <sstream>
#include <fstream>

#include <unistd.h>
#include <sys/stat.h>

using std::string;
using std::stringstream;
using std::getline;

using std::fstream;
using std::ifstream;
using std::ofstream;

using starlight::util::Path;

Path::Path() {
    strpath = "sdmc:";
}

Path::Path(const string& path, bool noverify) {
    if (noverify) { strpath = path; return; }
    size_t sls = path.find('/');
    if (sls == string::npos) strpath = "sdmc:/" + path;
    else if (path.rfind(':', sls) != sls-1) strpath = "sdmc:/" + path;
    else strpath = path;
    if (strpath.back() == '/') strpath.pop_back(); // clip trailing slash
}

Path::Path(const Path& path) {
    strpath = path.strpath; // no need to check here!
}

Path::~Path() {
    
}

Path Path::Up(int levels) {
    if (levels < 1) return *this;
    size_t spos = IsDirPath() ? strpath.length() - 1 : string::npos;
    for (int i = 0; i < levels; i++) {
        size_t pos = strpath.rfind('/', spos-1);
        if (pos == string::npos) break;
        if (strpath[pos] == ':') break; // don't move past root
        spos = pos;
    }
    return Path(strpath.substr(0, spos), true);
}

Path Path::Combine(const string& token) {
    string path(strpath);
    stringstream st(token);
    string tk;
    while (getline(st, tk, '/')) {
        if (tk == "") { } // ...
        else if (tk == ".") { } // ignore
        else if (tk == "..") {
            size_t pos = path.rfind('/');
            if (pos != string::npos && path[pos-1] != ':') path.erase(pos);
        }
        else if (tk.back() == ':') { // root switch...
            path.clear();
            path.append(tk);
        }
        else {
            if (path.back() != '/') path.append("/");
            path.append(tk);
        }
    }
    return Path(path, true);
}

bool Path::Exists() {
    struct stat st;
    return stat(strpath.c_str(), &st) == 0;
}

bool Path::IsFile() {
    struct stat st;
    if (stat(strpath.c_str(), &st) != 0) return false;
    return S_ISREG(st.st_mode);
}

bool Path::IsDirectory() {
    struct stat st;
    if (stat(strpath.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

Path& Path::CreateDirectory() {
    if (IsDirectory()) return *this;
    if (mkdir(strpath.c_str(), 0) != 0) { // try creating in place, else...
        Up().CreateDirectory(); // create parent recursively
        mkdir(strpath.c_str(), 0);
    }
    
    return *this;
}

fstream Path::Open(std::ios_base::openmode mode) {
    if (mode & std::ios_base::out) Up().CreateDirectory(); // make sure path exists if writing
    return fstream(strpath, mode);
}

ifstream Path::OpenI() {
    return ifstream(strpath);
}

ofstream Path::OpenO() {
    Up().CreateDirectory(); // assert path beforehand
    return ofstream(strpath);
}
