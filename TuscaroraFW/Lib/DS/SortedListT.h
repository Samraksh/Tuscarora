////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef SORTEDLIST_T_H_
#define SORTEDLIST_T_H_


//Notes: This template class implements a sorted doubly linked list, that can be accessed like an array. A standard comparator for sorting list in ascending order is provided.
// Otherwise users need to provide a custom comparator class
//class allows duplicates to be stored. Hence the comparator is < or =

template<class T>
class StdLess {
public:
  bool LessThan (T& A, T& B) {
    return A < B;
  }
  bool EqualTo (T& A, T& B) {
    return A == B;
  }
};

template <class Type, bool SupportDuplicates, class Compare = StdLess<Type> >
class SortedListT {
protected:
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
  
	Node *head;
	Node *tail;
	uint32_t listsize;
	Compare comp;

	Node* FindNode (Type item) {
		Node *iterNode=head;

		for (uint32_t i=0; i< listsize; i++){
			if(comp.EqualTo(iterNode->data,item)){
				return iterNode;
			}
			iterNode = iterNode->next;
		}
		return NULL;
	}
  
	Node* FindIndex (uint32_t index) {
		Node *ret_node=NULL;
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
  
  bool DeleteNode(Node *_node);
  
public:
	SortedListT(){
		listsize=0;
		head=NULL;tail=NULL;	
	}
	
	Type GetItem (uint32_t index);
	uint32_t Size(){return listsize;}
	bool Insert(Type& object);
	bool Delete(uint32_t index);
	bool DeleteItem(Type object);
};


template <class Type, bool SupportDuplicates, class Compare > 
Type SortedListT<Type, SupportDuplicates, Compare>::GetItem (uint32_t index){
	if(index >= listsize){
		Type ret=0;
		printf("SortedListT:: GetItem Error: Index %d is above the size of the array. check your logic. Undetminded return value.\n", index); fflush(stdout);
		return ret;
	}else {
		Node* ret = FindIndex(index);
		return ret->data; 
	}
}


template <class Type, bool SupportDuplicates, class Compare >
bool SortedListT<Type, SupportDuplicates, Compare>::Insert(Type& object){
	Node *new_node = new Node();
	new_node->data = object;

	if (listsize ==0) {
		//printf("First element being inserted\n");
		head = new_node;
		new_node->next=NULL;
		new_node->prev=NULL;
		tail = new_node;
	}
	else if (listsize == 1) {
		//printf("Inserting %d, head is : %d \n", object, head->data );
		//printf("Second element being inserted\n");
		if(!comp.EqualTo(head->data, object) || SupportDuplicates){
			if(comp.LessThan (head->data, object) ){
				head->next=new_node;
				new_node->prev = head;
				tail = new_node;
				//printf("Element inserted as the tail\n");
			}
			else {
				//printf(" %d is not less than %d. Hence...  ",head->data, object);
				tail->prev=new_node;
				new_node->next = tail;
				head = new_node;
				//printf("Element inserted as the head\n");
			}
		}else{
			printf("Sorted List: Duplicate detected: Not allowed\n");
			delete new_node;
			return false;
		}
	}
	else {
		//printf("Inserting %d, head is : %d \n", object, head->data );
		Node *lower, *upper;
		lower = head;
		upper = head->next;

		//check if new should be head
		if(comp.LessThan(object, lower->data)){
			new_node->next = head;
			new_node->prev = NULL;
			head->prev = new_node;
			head = new_node;  
		}
		else {
			uint32_t i=0;
			//TODO: Implements a linear search. Can be improved to do a log search, since the list is always sorted
			for (i=0; i<listsize-1; i++) {
				if(!comp.EqualTo(lower->data, object) && !comp.EqualTo(upper->data, object)) {
					if(comp.LessThan (lower->data, object)) {
						if(comp.LessThan (object, upper->data)){
							//printf("found the place to insert at %d,inserting object %d between %d, %d\n", i+1, object, lower->data, upper->data);fflush(stdout);
							lower->next = new_node;
							new_node->prev = lower;
							new_node->next = upper;
							upper->prev = new_node;
							break;
						}
						else {
							lower = upper;
							upper = upper->next;
						}
					}
				}else {
					//This is a duplicate object. If dupflag is set insert it next to lower object.
					if (SupportDuplicates) {
						lower->next = new_node;
						new_node->prev = lower;
						new_node->next = upper;
						upper->prev = new_node;
						break;
					}else {
						//Just return the list does not support duplicate objects
						printf("Sorted List: Duplicate detected: Not allowed\n");
						delete new_node;
						return false;
					}
				}
			}
			
			if(i == listsize-1){
				//printf("Didnt find any place to insert, must be put up as tail\n");
				tail->next = new_node;
				new_node->next = NULL;
				new_node->prev = tail;
				tail = new_node;
			}
		}
	}

	listsize++;
	return true;
}

template <class Type, bool SupportDuplicates, class Compare >
bool SortedListT<Type, SupportDuplicates, Compare>::Delete(uint32_t index){
	if (index > listsize) return false;

	Node *node = FindIndex(index);
	return DeleteNode(node);
}


template <class Type, bool SupportDuplicates, class Compare >
bool SortedListT<Type, SupportDuplicates, Compare>::DeleteItem(Type object){
	Node *node = FindNode(object);
	return DeleteNode(node);
}

template <class Type, bool SupportDuplicates, class Compare >
bool SortedListT<Type, SupportDuplicates, Compare>::DeleteNode(Node *_node){
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


#endif // SORTEDLIST_T_H_
