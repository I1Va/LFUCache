#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>

namespace ut {

inline bool logIfstreamError(const std::istream &inputStream,
                             std::ostream &outputStream = std::cerr) {
    if (inputStream.good())
        return false;

    if (inputStream.bad()) {
        outputStream << "Fatal I/O error while reading.\n";
        return true;
    }

    if (inputStream.fail()) {
        outputStream << "Logical read error (e.g., wrong type).\n";
        return true;
    }

    if (inputStream.eof()) {
        outputStream << "Reached end of file.\n";
        return true;
    }

    return true;
}

} // namespace ut

#endif // UTILITIES_HPP