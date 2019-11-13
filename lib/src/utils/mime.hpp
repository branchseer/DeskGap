#ifndef DESKGAP_UTILS_MINE_H
#define DESKGAP_UTILS_MINE_H

#include <unordered_map>
#include <string>

namespace DeskGap {
    inline std::string GetMimeTypeOfExtension(const std::string& extension) {
        static std::unordered_map<std::string, std::string> mimeTypesByExtension {
            { "txt", "text/plain" },
            { "bmp", "image/bmp" },
            { "css", "text/css" },
            { "git", "image/gif" },
            { "htm", "text/html" },
            { "html", "text/html" },
            { "ico", "image/vnd.microsoft.icon" },
            { "jpg", "image/jpeg" },
            { "jpeg", "image/jpeg" },
            { "js", "text/javascript" },
            { "json", "application/json" },
            { "mjs", "application/javascript" },
            { "mp3", "audio/mpeg" },
            { "mpeg", "video/mpeg" },
            { "png", "image/png" },
            { "pdf", "application/pdf" },
            { "svg", "image/svg+xml" },
            { "tif", "image/tiff" },
            { "tiff", "image/tiff" },
            { "tff", "font/ttf" },
            { "wav", "audio/wav" },
            { "weba", "audio/webm" },
            { "webm", "video/webm" },
            { "webp", "image/webp" },
            { "woff", "font/woff" },
            { "woff2", "font/woff2" },
            { "xhtml", "application/xhtml+xml" },
            { "xml", "application/xml" },
        };

        auto findResult = mimeTypesByExtension.find(extension);
        if (findResult == mimeTypesByExtension.end()) {
            return "application/octet-stream";
        }
        return findResult->second;
    };
}

#endif
