/*
 * Copyright 2012  Maciej Poleski
 */

#ifndef ADDRESS_H
#define ADDRESS_H

#include <cstdint>

/**
 * Przechowuje pełne informacje o lokalizacji pewnej usługi
 */
class Address
{
public:

    std::uint32_t ip;
    std::uint16_t port;

};

#endif // ADDRESS_H
