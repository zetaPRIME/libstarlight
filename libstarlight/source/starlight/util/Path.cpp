#include "Path.h"

#include <sstream>

#include <unistd.h>
#include <sys/stat.h>

using std::string;
using std::stringstream;
using std::getline;

using starlight::util::Path;

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
