////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef BiBST_MAP2T_H_
#define BiBST_MAP2T_H_

#include "AVLBinarySearchTreeT.h"
#include <stdlib.h>
#include <assert.h>

#ifdef PLATFORM_EMOTE
#define abort() exit(3)
#endif

template <class Element, class KEY, class KEYCOMPARATOR = SimpleGenericComparator<KEY> >
class MapElementComparator_KEY{
public:
	static bool LessThan (const Element& A, const Element& B) {
		return (KEYCOMPARATOR::LessThan(A->GetKey(),B->GetKey()));
	}
	static bool EqualTo (const Element& A, const Element& B) {
		return (KEYCOMPARATOR::EqualTo(A->GetKey(),B->GetKey()));
	}
};

template <class Element, class DATA, class DATACOMPARATOR = SimpleGenericComparator<DATA> >
class MapElementComparator_DATA{
public:
	static bool LessThan (const Element& A, const Element& B) {
		return (DATACOMPARATOR::LessThan(A->GetData(),B->GetData()));
	}
	static bool EqualTo (const Element& A, const Element& B) {
		return (DATACOMPARATOR::EqualTo(A->GetData(),B->GetData()));
	}
};

template <class KEY, class DATA >
class BiMapElement{
protected:
	KEY key;
	DATA data;
public:
	BiMapElement(const KEY &_key, const DATA &_data){
	    key=_key;
	    data=_data;
	}
	BiMapElement(const KEY &_key){
	    key=_key;
	}
	//BiMapElement(DATA _data){data=_data;}
	BiMapElement(){}

	const DATA& GetData(){return data;}
	const KEY& GetKey(){return key;}
	//void SetData(DATA &_data){data=_data;}
	//void SetKey(KEY &_key){key=_key;}
	void operator=(const BiMapElement<KEY,DATA> &B){
		key = B.key;
		data = B.data;
	};
	template<class U1, class U2, class U3, class U4> friend class BiBSTMapT;
	//friend class MapElementComparator_KEY<BiMapElement<KEY,DATA> , KEY, SimpleGenericComparator<KEY> > ;
	//friend class MapElementComparator_DATA<BiMapElement<KEY,DATA> , KEY, SimpleGenericComparator<KEY> > ;
};



template <class KEY, class DATA, class DATACOMPARATOR = SimpleGenericComparator<DATA>, class KEYCOMPARATOR = SimpleGenericComparator<KEY> >
class BiBSTMapT  {
private:
	//MapElementComparator_KEY<BiMapElement<KEY,DATA>*,KEY, KEYCOMPARATOR > map_el_comp_wkey;
	//MapElementComparator_DATA<BiMapElement<KEY,DATA>*,DATA, DATACOMPARATOR > map_el_comp_wdata;
	AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_KEY<BiMapElement<KEY,DATA>*, KEY, KEYCOMPARATOR > > myBST1;
	AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_DATA<BiMapElement<KEY,DATA>*, DATA, DATACOMPARATOR > > myBST2;
	class IteratorRef1 {
	protected:
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node;
		AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_KEY< BiMapElement<KEY,DATA>* , KEY, KEYCOMPARATOR > >* myBSTptr;
	public:
		const KEY& First (){ return (node->GetData())->GetKey(); };

		const DATA& Second (){ return (node->GetData())->GetData(); };

		IteratorRef1 (AVLBSTElement< BiMapElement<KEY,DATA>* > *_node
				, AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_KEY< BiMapElement<KEY,DATA>* , KEY, KEYCOMPARATOR > >* _myBST1ptr
				){
			node = _node;
			myBSTptr = _myBST1ptr;
		};

		IteratorRef1 (){
			node = NULL;
			myBSTptr = NULL;
		};

		void operator= (const IteratorRef1* d){
			node = d->node;
			myBSTptr = d->myBSTptr;
		};
		void operator=(const IteratorRef1 &a){
			this->node = a.node;
			this->myBSTptr = a.myBSTptr;
		};
		IteratorRef1* operator -> (){
			return this;
		};
		bool operator == (const IteratorRef1& d){
			if (this->node == NULL && d.node == NULL) return true;
			return(this->node == (d.node));
		};
		bool operator != (const IteratorRef1& d){
			return (!(*this == d));
		};
		friend class BiBSTMapT;
	};
	class IteratorRef2 {
	protected:
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node;
		AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_DATA< BiMapElement<KEY,DATA>* , DATA, DATACOMPARATOR > >* myBSTptr;
	public:
		const KEY& First (){ return (node->GetData())->GetKey(); };

		const DATA& Second (){ return (node->GetData())->GetData(); };

		IteratorRef2 (AVLBSTElement< BiMapElement<KEY,DATA>* > *_node
				, AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_DATA< BiMapElement<KEY,DATA>* , DATA, DATACOMPARATOR > >* _myBST2ptr
				){
			node = _node;
			myBSTptr = _myBST2ptr;
		};

		IteratorRef2 (){
			node = NULL;
			myBSTptr = NULL;
		};

		void operator= (const IteratorRef2* d){
			node = d->node;
			myBSTptr = d->myBSTptr;
		};
		void operator=(const IteratorRef2 &a){
			this->node = a.node;
			this->myBSTptr = a.myBSTptr;
		};
		IteratorRef2* operator -> (){
			return this;
		};
		bool operator == (const IteratorRef2& d){
			if (this->node == NULL && d.node == NULL) return true;
			return(this->node == (d.node));
		};
		bool operator != (const IteratorRef2& d){
			return (!(*this == d));
		};
		friend class BiBSTMapT;
	};

	class FIteratorRef1 : public IteratorRef1{
	public:
		FIteratorRef1(AVLBSTElement< BiMapElement<KEY,DATA>* > *_node
				, AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_KEY< BiMapElement<KEY,DATA>* , KEY, KEYCOMPARATOR > >* _myBST1ptr
				):IteratorRef1(_node, _myBST1ptr){};
		FIteratorRef1():IteratorRef1(){};

		FIteratorRef1& operator ++ (){     // prefix ++
			this->node = (this->myBSTptr->Next(this->node));
			return *this;
		};
		FIteratorRef1& operator -- (){     // prefix ++
			this->node = (this->myBSTptr->Previous(this->node));
			return *this;
		};
		FIteratorRef1 operator+(int offset){
			FIteratorRef1 rit;
			rit = *this;
			while(rit.node && offset>0){
				++rit;
				--offset;
			}
			return(rit);
		};
		FIteratorRef1 operator-(int offset){
			FIteratorRef1 rit;
			rit = *this;
			while(rit.node && offset>0){
				--rit;
				--offset;
			}
			return(rit);
		};
	};
	class RIteratorRef1 : public IteratorRef1{
	public:
		RIteratorRef1(AVLBSTElement< BiMapElement<KEY,DATA>* > *_node
				, AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_KEY< BiMapElement<KEY,DATA>* , KEY, KEYCOMPARATOR > >* _myBST1ptr
				):IteratorRef1(_node, _myBST1ptr){};
		RIteratorRef1():IteratorRef1(){};
		RIteratorRef1& operator++ (){     // prefix ++
			if (this->node != NULL)
				this->node = this->myBSTptr->Previous(this->node);
			return *this;
		};
		RIteratorRef1& operator-- (){     // prefix ++
			if (this->node != NULL)
				this->node = this->myBSTptr->Next(this->node);

			return *this;
		};
		RIteratorRef1 operator+(int offset){
			RIteratorRef1 rit;
			rit = *this;
			while(rit.node != NULL && offset>0){
				++rit;
				--offset;
			}
			return(rit);
		};
		RIteratorRef1 operator-(int offset){
			RIteratorRef1 rit;
			rit = *this;
			while(rit.node != NULL && offset>0){
				--rit;
				--offset;
			}
			return(rit);
		};
	};

	class FIteratorRef2 : public IteratorRef2{
	public:
		FIteratorRef2(AVLBSTElement< BiMapElement<KEY,DATA>* > *_node
				, AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_DATA< BiMapElement<KEY,DATA>* , DATA, DATACOMPARATOR > >* _myBST2ptr
				):IteratorRef2(_node, _myBST2ptr){};
		FIteratorRef2():IteratorRef2(){};

		FIteratorRef2& operator ++ (){     // prefix ++
			this->node = (this->myBSTptr->Next(this->node));
			return *this;
		};
		FIteratorRef2& operator -- (){     // prefix ++
			this->node = (this->myBSTptr->Previous(this->node));
			return *this;
		};
		FIteratorRef2 operator+(int offset){
			FIteratorRef2 rit;
			rit = *this;
			while(rit.node && offset>0){
				++rit;
				--offset;
			}
			return(rit);
		};
		FIteratorRef2 operator-(int offset){
			FIteratorRef2 rit;
			rit = *this;
			while(rit.node && offset>0){
				--rit;
				--offset;
			}
			return(rit);
		};
	};
	class RIteratorRef2 : public IteratorRef2{
	public:
		RIteratorRef2(AVLBSTElement< BiMapElement<KEY,DATA>* > *_node
				, AVLBST_T< BiMapElement<KEY,DATA>*, MapElementComparator_DATA< BiMapElement<KEY,DATA>* , DATA, DATACOMPARATOR > >* _myBST2ptr
				):IteratorRef2(_node, _myBST2ptr){};
		RIteratorRef2():IteratorRef2(){};
		RIteratorRef2& operator++ (){     // prefix ++
			if (this->node != NULL)
				this->node = this->myBSTptr->Previous(this->node);
			return *this;
		};
		RIteratorRef2& operator-- (){     // prefix ++
			if (this->node != NULL)
				this->node = this->myBSTptr->Next(this->node);

			return *this;
		};
		RIteratorRef2 operator+(int offset){
			RIteratorRef2 rit;
			rit = *this;
			while(rit.node != NULL && offset>0){
				++rit;
				--offset;
			}
			return(rit);
		};
		RIteratorRef2 operator-(int offset){
			RIteratorRef2 rit;
			rit = *this;
			while(rit.node != NULL && offset>0){
				--rit;
				--offset;
			}
			return(rit);
		};
	};
public:
	typedef  FIteratorRef1 Iterator1;
	typedef  RIteratorRef1 Reverse_Iterator1;
	typedef  FIteratorRef2 Iterator2;
	typedef  RIteratorRef2 Reverse_Iterator2;
	bool Insert(const KEY &_key, const DATA &_data){
		BiMapElement<KEY,DATA>* new_element = new BiMapElement<KEY,DATA> (_key,_data);
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node1 = myBST1.Search(new_element);
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST2.Search(new_element);
		if(node1 || node2){//The pair exists in one of the maps
			delete new_element;
			return false; //The item already exists in the map
			/*if( (node1->GetData()->GetKey() == new_element->GetKey() && node1->GetData()->GetData() == new_element->GetData())
			&&	(node2->GetData()->GetKey() == new_element->GetKey() && node2->GetData()->GetData() == new_element->GetData())	){
				delete new_element;
				return false; //The item already exists in the map
			}*/
		}
		if(true) {//New data
			if(myBST1.Insert (new_element)){
				if(myBST2.Insert (new_element)){
					return true;
				}
				else {
					if(myBST1.DeleteElement (myBST1.Search(new_element) ) ) {
						delete new_element;
						return false;
					}
					else { //Maps become out of sync
						assert(0); //Something went wrong
					}
				}
			}
			else{
				delete new_element;
				return false;
			}
		}
	};
	/*const DATA& operator [] (KEY _key){
		BiMapElement<KEY,DATA>* onlyKeyElement = new BiMapElement<KEY,DATA> (_key);
		AVLBSTElement< BiMapElement<KEY,DATA>* > *element = myBST1.Search(onlyKeyElement);
		if(element){
			return element->GetData()->GetData();
		}
		else {
			myBST1.Insert (onlyKeyElement);
			AVLBSTElement< BiMapElement<KEY,DATA>* > *element = myBST1.Search(onlyKeyElement);
			return element->GetData().GetData();
		}
	};*/

	Iterator1 GetIterator1(Iterator2 &it2){
		BiMapElement<KEY,DATA> new_element(it2->Second(),it2->First());
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST1.Search(&new_element);
		return(Iterator1(node2,&myBST2));
	};
	Iterator2 GetIterator2(Iterator1 &it1){
		BiMapElement<KEY,DATA> new_element(it1->First(),it1->Second());
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node1 = myBST2.Search(&new_element);
		return(Iterator1(node1,&myBST2));
	};


	bool Erase1(const KEY &_key){
		BiMapElement<KEY,DATA> onlyKeyElement(_key);
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node1 = myBST1.Search(&onlyKeyElement);
		if (node1){
			AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST2.Search(node1->GetData());
			assert(node1->GetData()->GetKey() == node2->GetData()->GetKey()); //Make sure both trees have the same content
			assert(node1->GetData()->GetData() == node2->GetData()->GetData()); //Make sure both trees have the same content
			BiMapElement<KEY,DATA>* map_element = node1->GetData();
			if(myBST1.DeleteElement(node1) && myBST2.DeleteElement(node2)){
				delete map_element;
				return true;
			}
			else abort(); //Something is wrong
		}
		else{
			return false;
		}
	};
	bool Erase2(const DATA &_data){
		KEY k; 
		BiMapElement<KEY,DATA> onlyKeyElement(k);
		onlyKeyElement.data = _data;
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node1 = myBST1.Search(&onlyKeyElement);
		if (node1){
			AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST2.Search(node1->GetData());
			assert(node1->GetData()->GetKey() == node2->GetData()->GetKey()); //Make sure both trees have the same content
			assert(node1->GetData()->GetData() == node2->GetData()->GetData()); //Make sure both trees have the same content
			BiMapElement<KEY,DATA>* map_element = node1->GetData();
			if(myBST1.DeleteElement(node1) && myBST2.DeleteElement(node2)){
				delete map_element;
				return true;
			}
			else abort(); //Something is wrong
		}
		else{
			return false;
		}
	};
	bool Erase1(Iterator1& it){
		assert(it != End1());
		Iterator1 it_temp;
		it_temp = it;
		++it;
		BiMapElement<KEY,DATA> new_element(it_temp->First(),it_temp->Second());
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST2.Search(&new_element);
		BiMapElement<KEY,DATA>* existing_element = node2->GetData();
		bool test1 = myBST1.DeleteElement(it_temp.node);
		bool test2 = myBST2.DeleteElement(node2);
		assert(test1 == test2);
		delete (existing_element);
		return test1;
	};
	bool Erase1(Reverse_Iterator1& it){
		assert(it != REnd1());
		Reverse_Iterator1 it_temp;
		it_temp = it;
		++it;
		BiMapElement<KEY,DATA> new_element(it_temp->First(),it_temp->Second());
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST2.Search(&new_element);
		BiMapElement<KEY,DATA>* existing_element = node2->GetData();
		bool test1 = myBST1.DeleteElement(it_temp.node);
		bool test2 = myBST2.DeleteElement(node2);
		assert(test1 == test2);
		delete (existing_element);
		return test1;
	};
	bool Erase2(Iterator2& it){
		assert(it != End2());
		Iterator1 it_temp;
		it_temp = it;
		++it;
		BiMapElement<KEY,DATA> new_element(it_temp->First(),it_temp->Second());
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST1.Search(&new_element);
		BiMapElement<KEY,DATA>* existing_element = node2->GetData();
		bool test1 = myBST2.DeleteElement(it_temp.node);
		bool test2 = myBST1.DeleteElement(node2);
		assert(test1 == test2);
		delete (existing_element);
		return test1;
	};
	bool Erase2(Reverse_Iterator2& it){
		assert(it != REnd2());
		Reverse_Iterator1 it_temp;
		it_temp = it;
		++it;
		BiMapElement<KEY,DATA> new_element(it_temp->First(),it_temp->Second());
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node2 = myBST1.Search(&new_element);
		BiMapElement<KEY,DATA>* existing_element = node2->GetData();
		bool test1 = myBST2.DeleteElement(it_temp.node);
		bool test2 = myBST1.DeleteElement(node2);
		assert(test1 == test2);
		delete (existing_element);
		return test1;
	};
	/*bool Erase(Reverse_Iterator1 it){
		return(myBST1.DeleteElement(it.node));
	};*/
	uint32_t Size(){
		return(myBST1.Size());
	};


	//Iterator1 Related
	Iterator1 Begin1(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = myBST1.Begin();
		Iterator1 myit(node,&myBST1);
		return (myit);
	};
	Reverse_Iterator1 RBegin1(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = myBST1.End();
		Reverse_Iterator1 myit(node,&myBST1);
		return (myit);
	};
	Iterator1 End1(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = NULL;
		Iterator1 myit(node,&myBST1);
		return (myit);
	};
	Reverse_Iterator1 REnd1(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = NULL;
		Reverse_Iterator1 myit(node,&myBST1);
		return (myit);
	};
	Iterator1 Find1(const KEY& _key){
		BiMapElement<KEY,DATA> onlyKeyElement(_key);
		AVLBSTElement< BiMapElement<KEY,DATA>* > *element = myBST1.Search(&onlyKeyElement);
		Iterator1 myit(element,&myBST1);
		return (myit);
	};

	Iterator2 Begin2(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = myBST2.Begin();
		Iterator2 myit(node,&myBST2);
		return (myit);
	};
	Reverse_Iterator2 RBegin2(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = myBST2.End();
		Reverse_Iterator2 myit(node,&myBST2);
		return (myit);
	};
	Iterator2 End2(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = NULL;
		Iterator2 myit(node,&myBST2);
		return (myit);
	};
	Reverse_Iterator2 REnd2(){
		AVLBSTElement< BiMapElement<KEY,DATA>* > *node = NULL;
		Reverse_Iterator2 myit(node,&myBST2);
		return (myit);
	};
	Iterator2 Find2(const DATA& _data){
		BiMapElement<KEY,DATA> onlyKeyElement;
		onlyKeyElement.data = _data;
		AVLBSTElement< BiMapElement<KEY,DATA>* > *element = myBST2.Search(&onlyKeyElement);
		Iterator2 myit(element,&myBST2);
		return (myit);
	};
};
#endif //BiBST_MAP2T_H_
