#ifndef mYlist
#define mYlist

template <class T>
class mynode{
     private:
            T           item;
	    mynode<T>   *next;
     public:
	    mynode()                             {next = 0;}
	    mynode(T& a)                         {item = a; next =0;}
	    mynode(const mynode<T>& n)           {item = n.item; next = 0;}
	    ~mynode()                            {;}
	    T          get_item()const           {return item;}
	    mynode<T>* get_next()const           {return next;}
	    void       set_next(mynode<T>* p)    {next = p;}
	  };




//on the list there are no duplicate items
template <class T> 
class mylist{
     private:
            mynode<T>* head;
	    int        count;
     public:
	    mylist() {head = 0; count =0;}
	    mylist(const mynode<T>& n) 
	      {head = new mynode<T>(n); count = 1;}
	    mylist(const mylist<T>& );
	    ~mylist();
	    int  get_count()const;
	    T*   get_ith_elem(int )const;
	    int  onlist(const T& );
	    void add_node(const mynode<T>&);
	  };
/*************************************************************************/

//copy constructor, copy the given list to this list
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

/************************************************************************/

//append the given node on the end of the list if the node is not already on
template <class T>
void mylist<T>:: add_node (const mynode<T>& n)
{
  if (count ==0)           //list is empty
    {
      head =new mynode<T>(n);
      count +=1;
    }
  else                       //list is not empty 
    {
      int indic = onlist(n.get_item());
      if (indic > 0)         //the node is already on the list    
	return;
      else                   //the node is not on the list
	{
	  mynode<T>* tmp = head;
	  for (int i = 1; i<count; i++)         //get to the end of the list
	    tmp = tmp->get_next();
	  mynode<T>* p =new mynode<T>(n);
	  tmp->set_next(p);                     //append the new node at the end
	  count++;                              //the list now has one more node
	  p = 0;
	}
    }
}

/************************************************************************/

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

/*************************************************************************/

template <class T>
int mylist<T>::get_count() const
{
  return count;
}

/************************************************************************/

//return the pointer to the item of the ith node on the list, or return 0 if 
//i is out of the range of the list

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

/*************************************************************************/

//to see if the given item a is an item of a node of the list or not. if it 
//is, return the index of the node, otherwise return 0

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

