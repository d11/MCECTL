/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.

#ifndef COM_IO_HPP
#define COM_IO_HPP
#define IN_COM_IO_HPP

#include <iostream>
#include <vector>
#include <set>

// Output functions for various STL containers.
template<class T>
std::ostream& operator<<( std::ostream& os, const std::vector<T>& v );

template<class T>
std::ostream& operator<<( std::ostream& os, const std::set<T>& v );

#include "com-io_i.hpp"

#undef IN_COM_IO_HPP
#endif

