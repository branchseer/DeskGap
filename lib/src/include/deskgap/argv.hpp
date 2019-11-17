//
// Created by patr0nus on 2019/11/13.
//

#ifndef DESKGAP_ARGV_HPP
#define DESKGAP_ARGV_HPP

#include <vector>
#include <string>

namespace DeskGap {
#ifdef WIN32
     std::vector<std::string> Argv(int argc, const wchar_t** argv);
#else
    inline std::vector<std::string> Argv(int argc, const char** argv) {
        return std::vector<std::string>(argv, argv + argc);
    }
#endif
}

#endif //DESKGAP_ARGV_HPP
