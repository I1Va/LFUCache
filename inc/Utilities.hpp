#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <iostream>


namespace ut {

bool logIfstreamError(const std::istream &inputStream, std::ostream &outputStream) {
    if (inputStream.good()) return false; 

    if (inputStream.bad()) {
        std::cerr << "Fatal I/O error while reading.\n";
        return true;
    } else if (inputStream.fail()) {
        std::cerr << "Logical read error (e.g., wrong type).\n";
        return true;
    } else if (inputStream.eof()) {
        std::cout << "Reached end of file.\n";
        return true;
    } else {
        return true;
    }
}

} // namespace ut


#endif // UTILITIES_HPP