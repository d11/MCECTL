/* (c) Copyright 1995-2004 by Bruce W. Watson / Loek Cleophas */
// SPARE Parts class library.

template<class T>
std::ostream& operator<<( std::ostream& os, const std::vector<T>& v ) {
	os << "Vector = (\n";
	auto typename std::vector<T>::const_iterator iter;
	auto int pos = 0;
	for( iter = v.begin(); iter != v.end(); ++iter, ++pos ) {
    	os << pos << "->" << *iter << "\n";
	}
	os << ")\n";
	return os;
}

template<class T>
std::ostream& operator<<( std::ostream& os, const std::set<T>& v ) {
	os << "Set = (\n";
	auto typename std::set<T>::const_iterator iter;
	auto int pos = 0;
	for( iter = v.begin(); iter != v.end(); ++iter, ++pos ) {
    	os << pos << "->" << *iter << "\n";
	}
	os << ")\n";
	return os;
}
