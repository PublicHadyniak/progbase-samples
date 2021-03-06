#pragma once

#include <cstdlib>
#include <cassert>

class List
{
    float *items;
    size_t capacity;
    size_t size_;

    void realloc(size_t newCapacity);

public:
    List();
    ~List();

    size_t size();
    float & operator [](int index);
    void insert(int index, float value);
    void removeAt(int index);
    void add(float value);
    void remove(float value);
    int indexOf(float value);
    bool contains(float value);
    bool isEmpty();
    void clear();
};

