#include "SharedStack.h"

void SharedStack::push(int num) {
    shmem[shmem[0] + 1] = num;
    shmem[0]++;
}

size_t SharedStack::size() {
    return shmem[0];
}

int SharedStack::top() {
    if (shmem[0] > 0) {
        int n = shmem[shmem[0]];
        return n;
    }
    // Error
}

void SharedStack::clear() {
    shmem[0] = 0;
}

int SharedStack::pop() {
    if (shmem[0] > 0) {
        int num = shmem[shmem[0]];
        shmem[0]--;
        return num;
    }
    // Error
}

std::string SharedStack::show() {
    std::ostringstream s;
    if(shmem[0] == 0) return "| |";
    for (int i = 0; i < shmem[0]; i++) {
        s << "|" << std::setw(2) << std::setfill('0') << shmem[i + 1];
    }
    s << "|";
    return s.str();
}

SharedStack::SharedStack(const std::string &file, char char_id, size_t total_size) : shmem(file, char_id,
                                                                                           total_size + 1) {
}
