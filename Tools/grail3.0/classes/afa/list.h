#ifndef mYlist
#define mYlist
/******************************************************************************************
 File: 	classes/afa/list.h
 -----

 Description:
 ------------
	This file contains the type definitions for the template classes "mynode"
	and "mylist", and defines all of their member functions.
	  
 Revision History:
 -----------------
	X. Wu				 Initial version of source code
	S. Huerter			 Header and comments
	
******************************************************************************************/
template <class T>
class mynode{
     private:
            T           item;
	    mynode<T>   *next;
     public:
	    mynode()                             {next = 0;}                //default constructor
	    mynode(T& a)                         {item = a; next =0;}       //assign constructor
	    mynode(const mynode<T>& n)           {item = n.item; next = 0;} //copy constructor
	    ~mynode()                            {;}                        //destructor
	    T          get_item()const           {return item;}             //access "item" private member
	    mynode<T>* get_next()const           {return next;}             //access "next" private member
	      void       set_next(mynode<T>* p)    {next = p;}              //set "next" private member
	  };
/*****************************************************************************************/
template <class T> 
class mylist{
     private:
            mynode<T>* head;
	    int        count;
     public:
	    mylist()                                //default constructor 
              {head = 0; count =0;}
	    mylist(const mynode<T>& n)              //assign constructor 
	      {head = new mynode<T>(n); count = 1;}
	    mylist(const mylist<T>& );              //copy constructor
	    ~mylist();                              //destructor
	    int  get_count()const;                  //access private "count" member 
	    T*   get_ith_elem(int )const;           //access ith element of list
	    int  onlist(const T& );                 //test for membership in list
	    void add_node(const mynode<T>&);        //add node to list
	  };
/******************************************************************************************

                       mylist<T>:: mylist(const mylist<T>& l)

Description:
        This function is a public member of template class "mylist".  This function is a copy
	constructor for this class.  The parameter list l is copied to this list.

	This function can be called in the following way(s):
	          list.mylist(that_list);

	Parameters:
	         const mylist<T>& l                    -the list to copy to this list

*****************************************************************************************/        
template <class T>
mylist<T>:: mylist(const mylist<T>& l)
{
  count = l.count;
  mynode<T>* tmp = l.head;
  if (count >0)
    {
      for (int i=1; i <= count; i++)
	{
	  add_node(*tmp);
	  tmp = tmp->get_next();
	}
    }
}
/******************************************************************************************

                       void mylist<T>:: add_node(const mynode<T>& n)

Description:
        This function is a public member of template class "mylist".  This function appends the 
	parameter node n to the end of this list if n is not already on this list.

	This function can be called in the following way(s):
	          list.add_node(node1);

	Parameters:
	         const mynode<T>& n                    -the node to append

*****************************************************************************************/     
template <class T>
void mylist<T>:: add_node (const mynode<T>& n)
{
  if (count ==0)           //list is empty
    {
      head =new mynode<T>(n);
      count +=1;
    }
  else                     //list is not empty 
    {
      int indic = 0;       //to indicate that if the item is 
                 	   //already on the list or not
	  mynode<T>* tmp =head;
      for (int i = 1; i<count; i++)
	{
	  T item = tmp->get_item();
	  if (item != n.get_item())
	    {
	      tmp = tmp->get_next();
	    }
	  else              //node n is on the list, don't add
	    {
	      indic = 1;
	      break;
	    }
        }
      if (indic == 0)
        {
          mynode<T>* p = new mynode<T>(n);
          tmp->set_next(p);
          p = 0;
          count++;
        }
    }
}
/******************************************************************************************

                                    mylist<T>::~mylist()

Description:
        This function is a public member of template class "mylist".  This function is the
        destructor for the mylist template class.

	This function can be called in the following way(s):
	          list.~mylist();

*****************************************************************************************/
template <class T>
mylist<T>::~mylist() 
{
  for (int i = 1; i<=count; i++)
    {
      mynode<T>* tmp = head;
      head = tmp->get_next();
      delete tmp;
    }
  count = 0;
}
/******************************************************************************************

                               int mylist<T>::get_count() const 

Description:
        This function is a public member of template class "mylist".  This function returns
	the private "count" member of this list.

	This function can be called in the following way(s):
	          result = list.get_count();

	Return Values:
	            int                               - the count member of this list

*****************************************************************************************/
template <class T>
int mylist<T>::get_count() const
{ return count;}
/******************************************************************************************

                        T* mylist<T>::get_ith_elem(int i)const
                               

Description:
        This function is a public member of template class "mylist".  This function returns
        a pointer to the item of the ith node on this list, or returns 0 if i is out of 
        range of this list.

	This function can be called in the following way(s):
	          addr = list.get_ith_elem(pos);

	Return Values:
	            T*                               - address of ith element
		    0                                - if i is out of range of this list

*****************************************************************************************/
template <class T>
T* mylist<T>::get_ith_elem(int i)const
{
  T* item = new T;
  if ((i<0)||(i>count))
    return 0;            //i is out of  the range of the list
  mynode<T> *tmp = head;
  for (int k=1; k<i; k++)
    tmp = tmp->get_next();  //move to the ith node of the list
  *item = tmp->get_item();
  return item;
}
/******************************************************************************************

                             int mylist<T>::onlist(const T& a)
                               

Description:
        This function is a public member of template class "mylist".  This function 
        determines whether the parameter item a is an item of a node on the list 
        or not.

	This function can be called in the following way(s):
	          result = list.onlist(item1);

	Return Values:
	            int                              - if a is an item of a node on this list
		                                       then the position of the node on this list
                                                       is returned;
		    0                                - otherwise

*****************************************************************************************/
template <class T> 
int mylist<T>::onlist(const T& a)
{
  mynode<T>* tmp=head;
  for (int i=1; i<=count; i++)
    {
      if (tmp->get_item() == a)
	return i;
      tmp = tmp->get_next();
    }
  return 0;
}

#endif

