/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts and FIRE Lite class libraries.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:12 $
#ifndef STRING_HPP
#define STRING_HPP
#define IN_STRING_HPP


#include <string>

typedef std::string String;

/*
#include "com-misc.hpp"
#include <iostream>
#include <assert.h>

// Represent strings at a higher level than char *.
// This class can be replaced by a standard library class, when
// those become available.

class String {
public:
	String();
	String( const char *str );
	String( const String& str );
	String( istream& is );
	~String();
	const String& operator=( const String& str );

	int operator==( const String& str ) const;
	int operator!=( const String& str ) const;
	int length() const;
	char operator[]( const int index ) const;
	bool c_inv() const;
	friend ostream& operator<<( ostream& os, const String& str );
private:
	#include "string.ppp"
};

#include "string.ipp"
*/

#undef IN_STRING_HPP
#endif

