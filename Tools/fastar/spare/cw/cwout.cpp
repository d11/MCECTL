/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.
// $Revision: 1.1.1.1.2.2 $
// $Date: 2004/06/01 08:12:13 $
#define IN_CWOUT_CPP

#include "cwout.hpp"

CWOutput::CWOutput( const std::set<String>& P, const RTrie& t ) :
		rep( t.size() ) {
//	assert( P.c_inv() );
	assert( t.c_inv() );
	// First make them all null
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		rep.map( q ) = 0;
	}
	// Go through all of the keywords.
//	for( int i = 0; i < P.size(); i++ ) {
	for( std::set<String>::const_iterator i = P.begin(); i != P.end(); i++ ) {
		// Go through keyword P_i, using a STravREV corresponding
		// to P_i.
//		auto STravREV trav( P.iterSelect( i ) );
		auto STravREV trav( *i );
		auto State q = FIRSTSTATE;
//		for( int j = 0; P.iterSelect( i )[j]; j++ ) {
		for( String::const_iterator j = i->begin(); j != i->end(); j++) {
//			q = t.image( q, P.iterSelect( i )[trav.traverse( j )] );
			q = t.image( q, alphabetNormalize( ( *i )[trav.traverse( std::distance( i->begin(), j ) )] ) );
		}
		// q is the state corresponding to P_i.
//		// rep[q] != 0, unless P is messed up:
//		should probably be:
		// rep[q] == 0, unless P is messed up:
		assert( rep[q] == 0 );
//		rep.map( q ) = new String( P.iterSelect( i ) );
		rep.map( q ) = new String( *i );
	}
	assert( c_inv() );
}

CWOutput::CWOutput( const CWOutput& r ) :
		rep( r.rep.size() ) {
	assert( r.c_inv() );
	// Got to make copies of all of the Strings pointed to.
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		if( r.rep[q] != 0 ) {
			rep.map( q ) = new String( *r.rep[q] );
		}
	}
	assert( c_inv() );
}

CWOutput::~CWOutput() {
	assert( c_inv() );
	// Delete each of the Strings pointed to.
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		delete rep[q];
	}
}

bool CWOutput::c_inv() const {
	// There must be at least one that is non empty.
	for( State q = FIRSTSTATE; q < rep.size(); q++ ) {
		if( rep[q] != 0 ) {
			return( rep.c_inv() );
		}
	}
	// This is bad.
	assert( !"This can't be happening" );
	return( false );
}

std::ostream& operator<<( std::ostream& os, const CWOutput& r ) {
	assert( r.c_inv() );
	os << "CWOutput = (\n";
	// Iterate over all of the states, looking for ones with output.
	for( State q = FIRSTSTATE; q < r.rep.size(); q++ ) {
		if( r.rep[q] != 0 ) {
			os << q << "-> \"" << *r.rep[q] << "\"\n";
		}
	}
	os << ")\n";
	assert( r.c_inv() );
	return( os );
}

