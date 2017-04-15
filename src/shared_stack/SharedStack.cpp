#include "SharedStack.h"

void SharedStack::push(int num) {
    shmem[_size] = num;
    _size++;
}

size_t SharedStack::size() {
    return this->_size;
}

int SharedStack::top() {
    std::cout << "viendo la carta " << _size << std::endl;

    int n = shmem[_size - 1];
    std::cout << "listo la carta" << std::endl;
    return n;
}

void SharedStack::clear() {
    _size = 0;
}

int SharedStack::pop() {
    int num = shmem[_size - 1];
    _size--;
    return num;
}

SharedStack::SharedStack(const std::string &file, char char_id, size_t total_size, size_t current_size) : shmem(file,
                                                                                                                char_id,
                                                                                                                total_size) {
    _size = current_size;
}
