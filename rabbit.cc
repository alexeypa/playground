#include <algorithm>
#include <array>
#include <iostream>
#include <random>

#include "AlignmentGuard.h"

int main() {
    AlignmentGuard aligment_guard;

    std::random_device rd;
    std::uniform_int_distribution<unsigned> dist;

    /*
     * Write at an unaligned address.
     */
    std::array<char, 4096> buf;
    *reinterpret_cast<unsigned*>(&buf[1]) = dist(rd);

    /*
     * Make sure writing to the buffer above is not optimized out.
     */
    std::cout << std::accumulate(buf.begin(), buf.end(), unsigned{});

    return 0;
}
