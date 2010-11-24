#ifndef QueUe_h
#define QueUe_h
/******************************************************************************************
 File: 	classes/afa/queue.h
 -----

 Description:
 ------------
	This file contains the type definition for the template classe "queue".
	The member functions are all defined inline below (for no apparent reason).
	  
 Revision History:
 -----------------
	X. Wu				 Initial version of source code
	S. Huerter			 Header and comments
	
******************************************************************************************/
template <class T>
class queue
{
   private:
               mynode<T>* front;
               mynode<T>* end;
   public:
               queue() {front = 0; end = 0;}                  //default constructor
               queue(const queue<T>& q)                       //copy constructor
	         {
	           mynode<T>* iterator=q.front;
	           mynode<T>* tmp1=0, *tmp2=0;
	           if (iterator == 0)
	             {
	               front = 0;
	               end = 0;
	               return;
	             }
		   T item = iterator->get_item();
	           tmp1 = new mynode<T>(item);
	           front= tmp1;
	           iterator = iterator->get_next();
	           while (iterator != 0)
	              {
			 item = iterator->get_item();
	                 tmp2 = new mynode<T>(item);
	                 tmp1->set_next(tmp2);
			 tmp1 = tmp2;
	                 iterator = iterator->get_next();
	              }
	           end = tmp1;
	           tmp1 = 0;
	           tmp2 = 0;
       	        }
	     ~queue()                                          //destructor
	       {
		 mynode<T>* tmp=front;
		 end = 0;
		 while (tmp != 0)
		    {
		      front= front->get_next();
		      delete tmp;
		      tmp = front;
		    }
	       }
      int    empty()                                          //test if queue is empty
	       {return front == 0? 1: 0;}
      int    full() {return 0;}                               //full queue? (it never is)
      void   push(T a)                                        //PUSH operation
	       {
		 if (!full())
		   {
		     mynode<T>* tmp = new mynode<T>(a);
		     if (empty())           //front = end = 0
		       {
			 front = tmp;
			 end = tmp;
			 tmp = 0;
		       }
		     else                   //keep the front pointer
		       {                    //append the newly created
					    //node to the end of the queue
					    //and set the end pointer to 
					    //the new end.
			 end->set_next(tmp);
			 end = tmp;
			 tmp = 0;
		       }
		   }
		 else
		   {
		     cerr<<"the queue is full."<<endl;
		     exit(1);
		   }
	       }
      T     pop();                                             //POP operation

};

template <class T>
T queue<T>::pop() 
	      {
		if (!empty())
		  {
		    T item=front->get_item();
		    if (front==end)   //there only has one node on the queue
		      {
			front = 0;
			end = 0;
			return item;
		      }
		    mynode<T>* tmp = front;
		    front = front->get_next();
		    tmp->set_next(0);
		    delete tmp;
		    return item;
		  }
		else
		  {
		    cerr<<"the queue is empty."<<endl;
		    exit(1);
		  }
	      }

#endif
