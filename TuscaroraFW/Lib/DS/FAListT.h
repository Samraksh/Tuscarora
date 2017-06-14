/*
 * FAListT.h
 *
 *  Created on: December 2016
 *      Author: karaoglu, sridhara
 */

#ifndef FALIST_T_H_
#define FALIST_T_H_


//Notes: Implements doubly linked list. the comparator class is used when checking for duplicates. a true value means they are equal.


template<class T>
class FAEqualTo {
public:
  bool operator () (T& A, T& B) {
    return A == B;
  }
};

template <class Type>
class FANode {
  public:
    FANode* next;
    Type data;
    FANode* prev;

    FANode() {
      next = NULL;
      prev = NULL;
    }
 };

template <uint32_t ARRAY_LEN, class Type, bool SupportDuplicates, class Compare >
class FAListT {
protected:
 ::FANode<Type> array[ARRAY_LEN];

	virtual void DeAllocateNode(::FANode<Type> * node){

		if(node == NULL) return;
		else if (listsize == 0) return;
		else if(node == &array[listsize-1]) {
			--listsize;
		}
		else{ //Delete. Be careful since there is no check whether the element belongs to the array in performing the deletion.
			*node = array[listsize-1];
			if(head == &array[listsize-1]) head = node;
			if(tail == &array[listsize-1]) tail = node;

			if(array[listsize-1].next) array[listsize-1].next->prev = node;
			if(array[listsize-1].prev) array[listsize-1].prev->next = node;


			array[listsize].next = NULL;
			array[listsize].prev = NULL;


			--listsize;
		}
	}
	virtual ::FANode<Type> * AllocateNode(){
		if(listsize < ARRAY_LEN ) {
			array[listsize].next = NULL;
			array[listsize].prev = NULL;
			return &array[listsize++];
		}
		else return NULL;
	}
	virtual ::FANode<Type> * AllocateNode(const Type &_data){
		auto p = AllocateNode();
		if(p) p->data = _data;
		return p;
	}



  ::FANode<Type> *head;
  ::FANode<Type> *tail;
  uint32_t listsize;
  Compare comp;

  ::FANode<Type>* FindNode (Type item) {
    ::FANode<Type> *iterNode=head;
    
    for (uint32_t i=0; i< listsize; i++){
      if(comp(iterNode->data,item)){
	return iterNode;
      }
      iterNode = iterNode->next;
    }
    return NULL;
  }

  
  ::FANode<Type>* FindIndex (uint32_t index) {
    ::FANode<Type> *ret_node=NULL;
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
  bool DeleteNode(::FANode<Type> *_node);
  bool Insert(Type& object, bool head);
public:
  FAListT() {
    listsize=0;
    head=0;
    tail=0;
  }
  Type GetItem (uint32_t index);
  Type operator [] (uint32_t index){return GetItem(index);}
  uint32_t Size();
  bool InsertFront(Type& object){return Insert(object,true);}
  bool InsertBack(Type& object){return Insert(object,false);}
  bool Delete(uint32_t index);
  bool DeleteItem(Type object);
};


template <class Type, bool SupportDuplicates, class Compare > 
Type FAListT<Type, SupportDuplicates, Compare>::GetItem (uint32_t index){
  if(index >= listsize){
    Type ret=0;
    printf("FAListT:: GetItem Error: Index %d is above the size of the array. check your logic. Undetminded return value.\n", index); fflush(stdout);
    return ret;
  }else {
    ::FANode<Type> * ret = FindIndex(index);
    return ret->data; 
  }
}

template <class Type, bool SupportDuplicates, class Compare >
uint32_t FAListT<Type, SupportDuplicates, Compare>::Size(){
  return listsize;
}

template <class Type, bool SupportDuplicates, class Compare >
bool FAListT<Type, SupportDuplicates, Compare>::Insert(Type& object, bool insertAtHead){
  ::FANode<Type> *new_node = AllocateNode();
  if(!new_node) return false;
  new_node->data = object;
 
  if(!SupportDuplicates){
    ::FANode<Type> *ret = FindNode(object);
    if(ret) {
      //printf("Found element in list already\n");
      ret->data = object;
      //delete new_node;
      DeAllocateNode(new_node);
      return true;
    }
  }
  //insert at head
  if(listsize==0){
    head =new_node;
    tail =new_node;
  }else {
    if(insertAtHead ) {
      new_node->next = head;
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
//  listsize++;
  return true;
}

template <class Type, bool SupportDuplicates, class Compare >
bool FAListT<Type, SupportDuplicates, Compare>::Delete(uint32_t index){
  if (index > listsize) return false;
  
  ::FANode<Type> *node = FindIndex(index);
  return DeleteNode(node);
}


template <class Type, bool SupportDuplicates, class Compare >
bool FAListT<Type, SupportDuplicates, Compare>::DeleteItem(Type object){
  ::FANode<Type> *node = FindNode(object);
  return DeleteNode(node);
}

template <class Type, bool SupportDuplicates, class Compare >
  bool FAListT<Type, SupportDuplicates, Compare>::DeleteNode(::FANode<Type> *_node){
  
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
    //delete _node;
    DeAllocateNode(_node);
    //listsize--;
    return true; 
  }
  
  return true;
}


#endif // LIST_T_H_
