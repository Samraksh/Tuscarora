/*
 * FABSTMapT.h
 *	Implements a map structure that stores KEY,DATA pairs.
 *	Search, insertion, erase are O(log n) both in average and in worst cases.
 *	The underlying data structure is an AVLBinarySearchTree.
 *
 *
 *  Created on: Feb 23, 2015
 *      Author: Bora Karaoglu
 */

#ifndef BST_MAPT_H_
#define BST_MAPT_H_

#include "FAAVLBinarySearchTreeT.h"


template <class Element, class KEY, class KEYCOMPARATOR = SimpleGenericComparator<KEY> >
class FAMapElementComparator{
public:
	static bool LessThan (Element& A, Element& B) {
		return (KEYCOMPARATOR::LessThan(A.key,B.key));
	}
	static bool EqualTo (Element& A, Element& B) {
		return (KEYCOMPARATOR::EqualTo(A.key,B.key));
	}
};

template <class KEY, class DATA, class KEYCOMPARATOR = SimpleGenericComparator<KEY> >
class FAMapElement{
	KEY key;
	DATA data;
	FAMapElementComparator<FAMapElement<KEY,DATA,KEYCOMPARATOR>,KEY,KEYCOMPARATOR> MapComp;
public:
	FAMapElement(KEY const _key, DATA const _data){key=_key; data=_data;}
	FAMapElement(KEY const _key){key=_key;}
	FAMapElement(){}
	bool operator == (const FAMapElement<KEY,DATA,KEYCOMPARATOR>& B){
		return MapComp.EqualTo(*this, B);
	}
	bool operator < (const FAMapElement<KEY,DATA,KEYCOMPARATOR>& B){
		return MapComp.LessThan(*this, B);
	}
	DATA& GetData(){return data;}
	KEY& GetKey(){return key;}
	void SetData(DATA _data){data=_data;}
	void operator=(const FAMapElement<KEY,DATA,KEYCOMPARATOR> &B){
		key = B.key;
		data = B.data;
	};
	friend class FAMapElementComparator<FAMapElement<KEY,DATA,KEYCOMPARATOR> , KEY, SimpleGenericComparator<KEY> > ;
};



template <FAAVLBST_Size_t ARRAY_LEN, class KEY, class DATA, class KEYCOMPARATOR = SimpleGenericComparator<KEY> >
class FABSTMapT  {
public:
	FAAVLBST_T< ARRAY_LEN, FAMapElement<KEY,DATA,KEYCOMPARATOR>, FAMapElementComparator< FAMapElement<KEY,DATA,KEYCOMPARATOR> , KEY, SimpleGenericComparator<KEY> > > myBST;
private:
	class IteratorRef {
	protected:
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *node;
		FAAVLBST_T< ARRAY_LEN, FAMapElement<KEY,DATA,KEYCOMPARATOR>, FAMapElementComparator< FAMapElement<KEY,DATA,KEYCOMPARATOR> , KEY, SimpleGenericComparator<KEY> > > const * myBSTptr;
	public:
		const KEY& First (){ return node->data.GetKey(); };
                
		DATA& Second (){ return node->data.GetData(); };
                
		IteratorRef (FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > * const _node, const FAAVLBST_T< ARRAY_LEN, FAMapElement<KEY,DATA,KEYCOMPARATOR>, FAMapElementComparator<FAMapElement<KEY,DATA,KEYCOMPARATOR> , KEY, SimpleGenericComparator<KEY> > >* const _myBSTptr) : myBSTptr( _myBSTptr)
		{
			node = _node;
		};
                
		IteratorRef (){
			node = NULL;
			myBSTptr = NULL;
		};
		void operator= (const IteratorRef* d){
			node = d->node;
			myBSTptr = d->myBSTptr;
		};
		void operator=(const IteratorRef &a){
			this->node = a.node;
			this->myBSTptr = a.myBSTptr;
		};
		IteratorRef* operator -> (){
			return this;
		};
		bool operator == (const IteratorRef& d){
			if (this->node == NULL && d.node == NULL) return true;
			return(this->node == (d.node));
		};
		bool operator != (const IteratorRef& d){
			return (!(*this == d));
		};
		friend class FABSTMapT;
	};
	class FIteratorRef : public IteratorRef{
	public:
		FIteratorRef(FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > * const _node, const FAAVLBST_T< ARRAY_LEN, FAMapElement<KEY,DATA,KEYCOMPARATOR>, FAMapElementComparator<FAMapElement<KEY,DATA,KEYCOMPARATOR> , KEY, SimpleGenericComparator<KEY> > >* const _myBSTptr):IteratorRef(_node,_myBSTptr){};
		FIteratorRef():IteratorRef(){};

		FIteratorRef& operator ++ (){     // prefix ++
			this->node = this->myBSTptr->Next(this->node);
			return *this;
		};
		FIteratorRef& operator -- (){     // prefix ++
			this->node = this->myBSTptr->Previous(this->node);
			return *this;
		};
		FIteratorRef operator+(int offset){
			FIteratorRef rit;
			rit = *this;
			while(rit.node && offset>0){
				++rit;
				--offset;
			}
			return(rit);
		};
		FIteratorRef operator-(int offset){
			FIteratorRef rit;
			rit = *this;
			while(rit.node && offset>0){
				--rit;
				--offset;
			}
			return(rit);
		};
		void operator=(const IteratorRef *d){
			this->node = d->node;
			this->myBSTptr = d->myBSTptr;
		};
		void operator=(const IteratorRef &a){
			this->node = a.node;
			this->myBSTptr = a.myBSTptr;
		};
	};
	class RIteratorRef : public IteratorRef{
	public:
		RIteratorRef(FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > * const _node, const FAAVLBST_T< ARRAY_LEN, FAMapElement<KEY,DATA,KEYCOMPARATOR>, FAMapElementComparator<FAMapElement<KEY,DATA,KEYCOMPARATOR> , KEY, SimpleGenericComparator<KEY> > >* const _myBSTptr):IteratorRef(_node,_myBSTptr){};
		RIteratorRef():IteratorRef(){};
		RIteratorRef& operator++ (){     // prefix ++
			if (this->node != NULL)
				this->node = this->myBSTptr->Previous(this->node);
			return *this;
		};
		RIteratorRef& operator-- (){     // prefix ++
			if (this->node != NULL)
				this->node = this->myBSTptr->Next(this->node);

			return *this;
		};
		RIteratorRef operator+(int offset){
			FIteratorRef rit;
			rit = *this;
			while(rit.node != NULL && offset>0){
				++rit;
				--offset;
			}
			return(rit);
		};
		RIteratorRef operator-(int offset){
			FIteratorRef rit;
			rit = *this;
			while(rit.node != NULL && offset>0){
				--rit;
				--offset;
			}
			return(rit);
		};
		void operator=(const IteratorRef *d){
			this->node = d->node;
			this->myBSTptr = d->myBSTptr;
		};
		void operator=(const IteratorRef &a){
			this->node = a.node;
			this->myBSTptr = a.myBSTptr;
		};
	};
public:

	typedef  FABSTMapT::FIteratorRef Iterator;
	typedef  FABSTMapT::RIteratorRef Reverse_Iterator;
	bool Insert(KEY _key, DATA _data){
		FAMapElement<KEY,DATA,KEYCOMPARATOR> new_element(_key,_data);
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *node = myBST.Search(new_element);

		//The key already exist, update the data
		if(node){
			node->data.SetData(_data);
			return true;
		}else {
			return myBST.Insert (new_element);
		}
	};
	DATA& operator [] (KEY _key){
		FAMapElement<KEY,DATA,KEYCOMPARATOR> onlyKeyElement(_key);
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *element = myBST.Search(onlyKeyElement);
		if(element){
			return element->data.GetData();
		}
		else {
			myBST.Insert (onlyKeyElement);
			FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *element = myBST.Search(onlyKeyElement);
			return element->data.GetData();
		}
	};

	bool Erase(const KEY &_key){
		FAMapElement<KEY,DATA,KEYCOMPARATOR> onlyKeyElement(_key);
		return(myBST.Delete(onlyKeyElement));
	};
	bool Erase(Iterator& it){
		Iterator it2;
		it2 = it;
		++it;
		bool test = myBST.DeleteElement(it2.node);
		return test;
	};
	bool Erase(Reverse_Iterator& it){
		Reverse_Iterator it2;
		it2 = it;
		++it;
		return(myBST.DeleteElement(it2.node));
	};
	/*bool Erase(Reverse_Iterator it){
		return(myBST.DeleteElement(it.node));
	};*/
	uint32_t Size() const{
		return(myBST.Size());
	};


	//Iterator Related
	Iterator Begin() const{
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *node = myBST.Begin();
		Iterator myit(node,&myBST);
		return (myit);
	};
	Reverse_Iterator RBegin() const {
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *node = myBST.End();
		Reverse_Iterator myit(node,&myBST);
		return (myit);
	};
	Iterator End() const{
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *node = NULL;
		Iterator myit(node,&myBST);
		return (myit);
	};
	Reverse_Iterator REnd() const{
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > *node = NULL;
		Reverse_Iterator myit(node,&myBST);
		return (myit);
	};
	Iterator Find(const KEY _key) const{
		FAMapElement<KEY,DATA,KEYCOMPARATOR> onlyKeyElement(_key);
		FAAVLBSTElement< FAMapElement<KEY,DATA,KEYCOMPARATOR> > * const element = myBST.Search(onlyKeyElement);

		Iterator myit(element,&myBST);
		return (myit);
	};
};
#endif //BST_MAPT_H_
