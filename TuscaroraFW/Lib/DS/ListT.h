////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIST_T_H_
#define LIST_T_H_


//Notes: Implements doubly linked list. the comparator class is used when checking for duplicates. a true value means they are equal.


template<class T>
class EqualTo {
public:
  bool operator () (T& A, T& B) {
    return A == B;
  }
};

template <class Type>
class Node {
  public:
    Node* next;
    Type data;
    Node* prev;
    
    Node() {
      next = NULL;
      prev = NULL;
    }
 };

template <class Type, bool SupportDuplicates, class Compare >
class ListT {
protected:
	::Node<Type> *head;
	::Node<Type> *tail;
	uint32_t listsize;
	Compare comp;
	
	class IteratorRef {
	private:
		ListT *parent;
	protected:	
		::Node<Type> *nodePtr;
		
	public:
		IteratorRef(ListT *l, ::Node<Type>* _nodePtr= NULL) 
		{
			parent=l;
			nodePtr=_nodePtr;	
		};            
		void operator= (const IteratorRef* d){
			this->nodePtr = d->nodePtr;
		};
		void operator=(const IteratorRef &a){
			this->nodePtr = a.nodePtr;
		};
		Type* operator -> (){
			return &(this->nodePtr->data);
		};
		Type& operator * (){
			return (this->nodePtr->data);
		};
		
		bool operator == (const IteratorRef& d){
			if (this->nodePtr == NULL && d.nodePtr == NULL) return true;
			return(this->nodePtr == (d.nodePtr));
		};
		bool operator != (const IteratorRef& d){
			return (!(*this == d));
		};
		IteratorRef& operator++(){
			if(this->nodePtr !=parent->tail){
				this->nodePtr=this->nodePtr->next;
				return *this;
			}else {
				this->nodePtr=NULL;
			}
			return *this;
		}
		IteratorRef& operator++(int dummy){
			return this->operator++();
		}
		IteratorRef& operator--(){
			if(this->nodePtr !=parent->head){
				this->nodePtr=this->nodePtr->prev;
				return *this;
			}
			return *this;
		}
		IteratorRef& operator--(int dummy){
			return this->operator--();
		}
	};
  
public:
	//friend class IteratorRef;
	typedef IteratorRef Iterator;

	::Node<Type>* FindNode (Type item) {
		::Node<Type> *iterNode=this->head;

		for (uint32_t i=0; i< this->listsize; i++){
			if(this->comp(iterNode->data,item)){
		return iterNode;
			}
			iterNode = iterNode->next;
		}
		return NULL;
	}

  
  ::Node<Type>* FindIndex (uint32_t index) {
    ::Node<Type> *ret_node=NULL;
    if (index >= listsize) {
      return ret_node; 
    }
    
    if(index==0) return head;
    if(index==listsize-1) return tail;
    
    if (index < listsize/2) {
      //printf("Looking at first half for index\n"); fflush(stdout);
      ret_node = head;
      for (uint32_t i=0; i< index; i++){
	ret_node = ret_node->next;
      }
    }else {
      //printf("Looking at second half for index\n"); fflush(stdout);
      ret_node = tail;
      for (uint32_t i=0; i< listsize-index-1; i++){
	ret_node = ret_node->prev;
      }
    }
    return ret_node;
  }
  bool DeleteNode(::Node<Type> *_node);
  bool Insert(Type& object, bool head);
public:
	ListT() {
		listsize=0;
		head=0;
		tail=0;
	}
	
	~ListT() {
		DeleteAll();
		head=0;
		tail=0;
	}

	void DeleteAll(){
		for (uint i=0; i< listsize; i++){
			Delete(i);
		}
	}
  
	void Clear (){DeleteAll ();}
  
	Type GetItem (uint32_t index);
	Type operator [] (uint32_t index){return GetItem(index);}
	uint32_t Size();
	bool InsertFront(Type& object){return Insert(object,true);}
	bool InsertBack(Type& object){return Insert(object,false);}
	bool Delete(uint32_t index);
	bool DeleteItem(Type object);
	Iterator Begin(){ Iterator ret(this,head); return ret;}
	Iterator End(){ Iterator ret(this,NULL); return ret;}
	
  
};


template <class Type, bool SupportDuplicates, class Compare > 
Type ListT<Type, SupportDuplicates, Compare>::GetItem (uint32_t index){
  if(index >= listsize){
#if __cplusplus >= 201103L	  
    Type ret{}; //extended initializer needs atleast c++11 support
#else 
	Type ret;
#endif
    printf("ListT:: GetItem Error: Index %d is above the size of the array. check your logic. Undetminded return value.\n", index); fflush(stdout);
    return ret;
  }else {
    ::Node<Type> * ret = FindIndex(index);
    return ret->data; 
  }
}

template <class Type, bool SupportDuplicates, class Compare >
uint32_t ListT<Type, SupportDuplicates, Compare>::Size(){
  return listsize;
}

template <class Type, bool SupportDuplicates, class Compare >
bool ListT<Type, SupportDuplicates, Compare>::Insert(Type& object, bool insertAtHead){
  ::Node<Type> *new_node = new ::Node<Type>();
  new_node->data = object;
 
  if(!SupportDuplicates){
    ::Node<Type> *ret = FindNode(object);
    if(ret) {
      //printf("Found element in list already\n");
      ret->data = object;
      delete new_node;
      return true;
    }
  }
  //insert at head
  if(listsize==0){
    head =new_node;
    tail =new_node;
  }else {
    if(insertAtHead ) {
      new_node->next = this->head;
      new_node->prev = NULL;
      head->prev = new_node;
      head = new_node; 
    }
    else { //or at tail
      tail->next = new_node;
      new_node->next = NULL;
      new_node->prev = tail;
      tail = new_node;
    }
    }
  listsize++;
  return true;
}

template <class Type, bool SupportDuplicates, class Compare >
bool ListT<Type, SupportDuplicates, Compare>::Delete(uint32_t index){
  if (index > listsize) return false;
  
  ::Node<Type> *node = FindIndex(index);
  return DeleteNode(node);
}


template <class Type, bool SupportDuplicates, class Compare >
bool ListT<Type, SupportDuplicates, Compare>::DeleteItem(Type object){
  ::Node<Type> *node = FindNode(object);
  return DeleteNode(node);
}

template <class Type, bool SupportDuplicates, class Compare >
  bool ListT<Type, SupportDuplicates, Compare>::DeleteNode(::Node<Type> *_node){
  
  if (_node ==NULL) {
    return true;
  }else {
    //only element
    if(listsize == 1){
      head =NULL;
      tail = NULL;
    }
    //this is head
    else if(_node == head ){
      head = head->next;
      head->prev = NULL;
    }
    else if (_node == tail) {
      tail = tail->prev;
      tail->next = NULL;
    }else { // just a middle node
      _node->prev->next = _node->next;
      _node->next->prev = _node->prev;
    }
    delete _node;
    listsize--;
    return true; 
  }
  
  return true;
}


#endif // LIST_T_H_
