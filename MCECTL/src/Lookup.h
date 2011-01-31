/*
 * =====================================================================================
 *
 *       Filename:  Lookup.h
 *    Description:  
 *         Author:  Dan Horgan (danhgn), danhgn@googlemail.com
 *
 * =====================================================================================
 */


#ifndef  _LOOKUP_H_
#define  _LOOKUP_H_

#include <map>
#include <vector>
#include <string>
#include <bitset>
#include <set>
#include <boost/checked_delete.hpp>
#include <boost/dynamic_bitset.hpp>
//#include "AutomataConcrete.h"
using namespace std;

template <class T_Entry, class T_Index = unsigned int>
class Lookup {
private:
   T_Index _index;
   map<T_Index, T_Entry*>  _forward_map;
   map<T_Entry*, T_Index> _reverse_map;

public:

   Lookup(const set<T_Entry*> &entries) : _index(0) {
      typename set<T_Entry*>::iterator iter;
      for( iter = entries.begin(); iter != entries.end(); ++iter ) {
         _forward_map[_index] = *iter;
         _reverse_map[*iter]  = _index;
         ++_index;
      }
   }

   T_Index Count() const { return _index; }
   const T_Entry &GetEntry(const T_Index &index) const {
      typename map<T_Index, T_Entry*>::const_iterator iter
         = _forward_map.find(index);

      if (iter == _forward_map.end()) {
         throw runtime_error("Index not found");
      }
      return *iter->second;
   }

   T_Index GetIndex(const T_Entry &entry) const {
      typename map<T_Entry*, T_Index>::const_iterator iter
         = _reverse_map.find(&entry);

      if (iter == _reverse_map.end()) {
         throw runtime_error("Entry not found");
      }
      return iter->second;
   }

   vector<const T_Entry *> GetEntries() const {
      vector<const T_Entry *> entries;
      typename map<T_Entry*, T_Index>::const_iterator iter;
      for( iter = _reverse_map.begin(); iter != _reverse_map.end(); ++iter ) {
         entries.push_back( iter->first );
      }
      return entries;
   }

   ~Lookup() {
      typename map<T_Entry*, T_Index>::const_iterator iter;
      for( iter = _reverse_map.begin(); iter != _reverse_map.end(); ++iter ) {
         delete iter->first;
      }

   }
};



/*

template <>
class Lookup< KripkeState, PushDownConfiguration > {
private:
   Lookup< string,  unsigned int> _state_lookup;
   Lookup< StackSymbol, unsigned int> _symbol_lookup;

public:

   Lookup(const set<PushDownState*> &entries)  {
   }

   PushDownConfiguration Count() const { return ; }
   const PushDownState &GetEntry(const PushDownConfiguration &index) const {
   }

   PushDownConfiguration GetIndex(const T_Entry &entry) const {

   }

   vector<const T_Entry *> GetEntries() const {
      vector<const T_Entry *> entries;
      typename map<T_Entry*, T_Index>::const_iterator iter;
      for( iter = _reverse_map.begin(); iter != _reverse_map.end(); ++iter ) {
         entries.push_back( iter->first );
      }
      return entries;
   }

   ~Lookup() {
      typename map<T_Entry*, T_Index>::const_iterator iter;
      for( iter = _reverse_map.begin(); iter != _reverse_map.end(); ++iter ) {
         delete iter->first;
      }

   }
};

template <class T_Entry>
class LookupSubset {
private:
   const Lookup<T_Entry> *_lookup;
   boost::dynamic_bitset<> _thing;
public:
   LookupSubset(const Lookup<T_Entry> &lookup) : _lookup(&lookup) { }

   // TODO
   vector<const T_Entry *> GetEntries() const {
      return _lookup->GetEntries();
   }

};


typedef Lookup<string> PropositionSet;
typedef Lookup<string> Alphabet;
//typedef Lookup<StackSymbol> StackAlphabet;
typedef LookupSubset<string> Valuation;
*/
#endif
