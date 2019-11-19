//
// Created by patr0nus on 11/19/2019.
//
#include "argv.hpp"
#include "util/wstring_utf8.h"

std::vector<std::string> DeskGap::Argv(int argc, const wchar_t** argv) {
    auto result = std::vector<std::string>();
    result.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        result.emplace_back(std::move(WStringToUTF8(argv[i])));
    }
    return result;
}
