#include <iostream.h>
#include "../array/array.h"
#include "../re/re.h"

#ifndef CFG_H
#define CFG_H

//we conside the terminal and the nonterminal symbols are the same 
//type. the left side of each production of the cfg must be a 
//single nonterminal symbol. the right side of each production is 
//either a catenation of symbols of a empty string


template <class T>
class product {

  friend istream& operator>>(istream&, product<T> &);
  friend ostream& operator<<(ostream&, const product<T> &);

  protected:
	 T              left;    //left side of the production
         list<T>        right;   //right side of the production
  public:
                        product()
                         {;}                    
                        product(const product<T> &p)  //assign constructor
                          {
			    left  = p.left;
			    right = p.right;
                          }
                        
                        ~product()
			  {;}
        void            assign(const T& l, const list<T>& r)
	                  {
			    left = l;
			    right = r;
			  }
	product<T>&     operator=(const product<T>& p)
	                {
			  if (this != &p)        //avoid self assignment
			    {
			      left = p.left;
			      right= p.right;
			    }
			  return *this;
			}
	int             operator==(const product<T> &p) const  
                        //return 1 if they are equal, 0 otherwise
                          {
			    return (left==p.left)&&(right==p.right);
			  }
	int             operator!=(const product<T> &p) const
			  {return *this==p? 0:1;}
	T               getleft() const
	                  {return left;}
	list<T>         getright() const
			  {return right;}
        void            setleft(const T& t)
                          {left=t;}
        void            setright(const list<T>& r)
	                  {rigth=r;}
	int             compare(const product<T> &p) const
	                //if this product is less than p, return -1
			//if this product is equal to p, return 0
			//if this product is great than p, return 1
			  {
			    if ((left==p.left)&&(right==p.right)) return 0;
			    if ((left==p.left)&&(right<p.right)) return -1;
			    if (left<p.left) return -1;
			    return 1;
    			  }  
	int             operator<(const product<T> &p) const     
			  {return this->compare(p)<0? 1:0;}
	int             operator>(const product<T> &p) const
                          {return this->compare(p)>0?1:0;}
        int             is_left(const T& s)const
                          { return left==s;}
        int             right_find(const T& t)const
	                //find the first ocurrence of the given symbol in 
		        //the right side of the product, and return the 
		        //position 
 
                	  {
                            return right.member(t);
                          }
	void           rewrite(const T& s, const list<T>& r)
	               //rewrite the first appearance of the given symbol 
		       //s in the right hand side of the product as the 
		       //given list<T>
	                 {
			   int index = right_find(s);
			   int length = right.size + r.size() - 1;
			   list<T> tmp(length);
			   for (int i = 0; i<index; i++)
			     tmp[i] = right[i];
			   for (i = index; i<length; i++)
			     tmp[i] = r[i-index];
			   right = tmp;
			   tmp.clear();
                         }
        void           rewrite(const product<T>& p)
	               //rewrite the first appearance of the symbol of the 
		       //p's left side as the right side of p in the right 
                       //of this product
	                 {
                           rewrite(p.left,p.right);
			 }
};

template <class T>
class cfg {
  friend istream& operator>>(istream &, cfg<T>&);
  friend ostream& operator<<(ostream &, const cfg<T>&);

  protected:
       T                  start;      //the sentence symbol
       array<T>           sym_list;   //list of the symbols of the grammer
       array<product<T> >  prod_list; //list of the productions of the grammer

       array<T>       sreachable()const;
       array<T>       suseful()const;
       T              findnewsym()const;
       array<T>       nullable()const;
       
  public:
               	  cfg() {;}
		  cfg(const cfg<T>& c)
                    :start(c.start),
                     sym_list(c.sym_list),
                     prod_list(c.prod_list)
		     {;}
	          ~cfg();
      T           getstart()const              //return the start symbol
                   {return start;}
      array<T>    getsymlist()const            //return the symbol list array
                   {return sym_list;}
      array<product<T> > getprodlist()const
                         {return prod_list;}
      void        setstart(const T& term)
                   {start=term;}
      void        setsymlist(const array<T>& sl)
                   {sym_list=sl;}
      void        setprodlist(const array<product<T> >& pl)
                    {prod_list=pl;}
      int         is_terminal(const T&) const;  //test to see it is a terminal
      int         is_nonterminal(const T&)const;// test to see is noterminal 
      int         is_nullable(const T&) const; //to see it is nullable or not
      array<T>    terminals() const;
      array<T>    nonterminals()const;
      cfg<T>      operator+=(const cfg<T> &);  //union of this cfg<T> with the given cfg<T>
      cfg<T>      operator*=(const cfg<T> &);  //catenat the given cfg<T> to this cfg<T>
      cfg<T>      operator=(const cfg<T>& c)   //assignment of cfg
	            {
                       cfg<T> target;
                       target.setstart(c.start);
                       target.setsymlist(c.sym_list);
                       target.setprodlist(c.prod_list);
                       return target;
                    }
      cfg<T>      star();                      //do the kleene star operation for this cfg<T>     
      cfg<T>      factoring()const;  //factor the given cfg<T> into factored cfg<T>
				     //i.e. the right side of each production is either containing
			             //only nonterminal symbol or a single terminal symbol, or empty string
      cfg<T>      useful()const;     //transfer the given cfg<T> into a cfg<T> only contains
                                     //useful symbols   					      
      cfg<T>      reachable()const;  //transfer the cfg<int> into a cfg<T> which only contains
			             //reachable symbol.
      cfg<T>      unitfree()const;   //transfer the given cfg<T> into a unit free cfg<T>
      cfg<T>      lambdafree()const; //transfer the given cfg<T> into a lamda free equivalent cfg<T>
      cfg<T>      cnf() const;       //transfer the given cfg<T> into a equivalent cnf form


};
#endif
