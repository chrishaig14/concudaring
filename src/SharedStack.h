//
// Created by chris on 13/04/17.
//

#ifndef CONCUDARING_SHAREDSTACK_H
#define CONCUDARING_SHAREDSTACK_H

#include <string>
#include "shmem/MemoriaCompartidaArray.h"

class SharedStack {
    MemoriaCompartidaArray<int> shmem;
    size_t _size;

public:
    SharedStack(const std::string &file, char char_id, size_t total_size, size_t current_size = 0);

    void push(int num);

    size_t size();

    int top();

    void clear();

    int pop();

    ~SharedStack() {

    }
};


#endif //CONCUDARING_SHAREDSTACK_H
