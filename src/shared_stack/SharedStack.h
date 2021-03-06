#ifndef CONCUDARING_SHAREDSTACK_H
#define CONCUDARING_SHAREDSTACK_H

#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "../shmem/MemoriaCompartidaArray.h"

class SharedStack {
    MemoriaCompartidaArray<int> shmem;

public:
    SharedStack(const std::string &file, char char_id, size_t total_size);

    void push(int num);

    size_t size();

    int top();

    void clear();

    int pop();

    ~SharedStack() {

    }

    std::string show();
};


#endif //CONCUDARING_SHAREDSTACK_H
