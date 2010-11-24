/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.1 $
// $Date: 2004/05/27 12:13:13 $
#ifndef CWOUT_HPP
#define CWOUT_HPP
#define IN_CWOUT_HPP

#include "state.hpp"
#include "string.hpp"
//#include "set.hpp"
#include <set>
#include <iterator>
#include "stateto.hpp"
#include "stravrev.hpp"
#include "tries.hpp"
#include <assert.h>
#include <iostream>

// The Commentz-Walter output function.

class CWOutput {
public:
	CWOutput( const std::set<String>& P, const RTrie& t );
	CWOutput( const CWOutput& r );
	~CWOutput();
	
	// Is q a keyword?
	int isKeyword( const State q ) const;
	// Yes, what is the keyword?
	const String& operator[]( const State q ) const;
	bool c_inv() const;
	friend std::ostream& operator<<( std::ostream& os, const CWOutput& r );
private:
	#include "cwout_p.hpp"
};

#include "cwout_i.hpp"

#undef IN_CWOUT_HPP
#endif

