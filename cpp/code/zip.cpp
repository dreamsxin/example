// https://github.com/dreamsxin/cpputils
#include <windows.h>
#include <gdiplus.h>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <string>
#include "zip_file.hpp"

namespace fs = std::filesystem;

std::string trimPrefix(std::string str, const std::string& prefix) {
    if (str.compare(0, prefix.length(), prefix) == 0) { //str.starts_with(prefix)
        str.erase(0, prefix.length());
    }
    return str;
}
std::string replaceAll(std::string& str, std::string oldStr, std::string newStr) {
    std::string::size_type pos = str.find(oldStr);
    while (pos != std::string::npos) {
        str.replace(pos, oldStr.size(), newStr);
        pos = str.find(oldStr);
    }
    return str;
}
int main() {
    std::string rootdir = "D:\\zip\\env\\";
    miniz_cpp::zip_file file;

    for (auto itEntry = fs::recursive_directory_iterator(rootdir);
        itEntry != fs::recursive_directory_iterator();
        ++itEntry) {
        const auto filenameStr = itEntry->path().filename().string();
        std::cout << std::setw(itEntry.depth() * 3) << "";
        std::string dstpath = trimPrefix(itEntry->path().string(), rootdir); 
        std::replace(dstpath.begin(), dstpath.end(), '\\', '/');
        std::cout << "dstpath:  " << dstpath << '\n';
        std::cout << "dir:  " << filenameStr << '\n';
        if (itEntry->is_directory()) {
                continue;
        }
        file.write(itEntry->path().string(), dstpath);
    }

    file.save("test.zip");

    return 0;
}
