////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef HEAPT_H_
#define HEAPT_H_

template<class T>
class MyLess {
public:
  bool LessThan (T& A, T& B) {
    return A < B;
  }
};

/*template<class T>
class MyGreater {
public:
  bool LessThan (T& A, T& B) {
    return A > B;
  }
};
*/

template <class T>
class Element {
public:
  T data;
  Element<T> *right, *left, *parent;
  //uint16_t index;
  
  Element(T obj){
    data = obj;
    right =0;
    left=0;
    parent=0;
    //printf("Done constructing heap element\n");fflush(stdout);
  }
};

template <class T, class Compare = MyLess<T> >
class HeapT {
protected:
  uint32_t noOfElements;
  Element<T> *leaf;
  Element<T> *root;
  Compare compareClass;
      
public:
  HeapT(){
    //printf("Inside Heap constructor\n");
    noOfElements=0;
    root=0;
    leaf=0;
  }
  
  
  inline void Heapify(){
    BubbleDown(root);
  }

  uint32_t Size() {return noOfElements;}

  //Create a new leaf node, add it as the right chilf of current leaf
  //set current leaf as parent of new leaf.
/*  bool Insert(T obj){
    Element<T> *new_leaf;
    new_leaf= new Element<T>(obj);
    if(new_leaf){
      //new_leaf->index=++noOfElements;
      if(leaf){
	leaf->right=new_leaf; //attach the newleaf to right of currrent leaf
	new_leaf->parent = leaf; //set current leaf as parent of newleaf
	leaf = new_leaf; //set leaf to newleaf
	BubbleUp(leaf); //bubble up
      }else {
	//When no elements are in heap
	leaf=new_leaf; //attach the newleaf to right of currrent leaf
	new_leaf->parent = new_leaf; //set yourself as the parent
	root= new_leaf; //set root to  to newleaf
	//BubbleUp(leaf); //No need to bubble up
      }
      
      return true;
    }
    else {
      //Something wrong, memory allocation failed
      return false;
    }    
  }
*/

  bool Insert(T obj){
    Element<T>* newNode = new Element<T>(obj);
    uint16_t height=0;
    if(newNode){
      if(!root){ //root is null, 1st element
	root=newNode;
	leaf=newNode;
	root->parent=0;
      }else {
	//printf("Root address is %p \n", root);fflush(stdout);
	InsertAtRightPlace(root, newNode,height);
      }
      //printf("Inserted node %p at height %d, new leaf is %d\n",newNode, height, leaf->data);
      //printf("before increment size %d\n",noOfElements);
      noOfElements++;
      //printf("after increment size %d\n",noOfElements);
      return true;
    }
    return false;
  }

  /*  
  T ExtractTop() {
    bool heapEmpty=false;
    T rtn = root->data;
    Element<T>* oldroot = root;
    if(root->left){
      root = root->left;
      printf("Setting left as new root\n");fflush(stdout);
    }else if(root->right){
      root=root->right;
      printf("Setting right as new root\n");fflush(stdout);
    }else {
      root=0;
      heapEmpty=true;
      printf("Heap is empty\n");fflush(stdout);
    }
    delete(oldroot);
    noOfElements--;
    if(!heapEmpty){
      BubbleDown(root);
    }
    return rtn;
  }
  */
  
  //Leaf should never have any child, both left and right child are null
  T ExtractTop(){
    if(root == NULL) {
      printf("HeapT:: Error: Extracting from empty heap \n");fflush(stdout);
      return (T)0;
    }
    
    T rtn = root->data;
    root->data = leaf->data;
    //Element<T>* oldroot = root;
    noOfElements--;
    //printf("new size %d\n",noOfElements);
    if(leaf == root) { // the root is the only node
      delete(root);
      //printf("HeapT:: ExtractTop: Deleted last node \n");fflush(stdout);
      root = NULL;
    } else {
      Element<T>* oldLeaf = leaf;  
      //newRoot->left = root->left;
      //newRoot->right = root->right;    
      leaf=FindNewLeaf(leaf);
      //printf("New leaf is %p, root before bubble down is %p\n", leaf, root);
      delete(oldLeaf);
      oldLeaf=NULL;
      //root = newRoot;
      //Swap(root,leaf);
      BubbleDown(root);
    }
    
    return rtn;
  }
 
  T PeekTop(){
    if(root)
    {
      return root->data;
    }
    else {	//Heap is empty, this behavior is undefined.
      return (T)0;  
    }
  }
private:

  /*
  Element<T>* FindNode(uint16_t _index){
  
    if(_index==0)
      return root;
    else {
      if(_index % 2)
    }
  }*/

  ///Given the current leaf, finds a new leaf
  ///First finds the sibling of current leaf, checks if valid, and goes down the tree to find a leaf
  ///If sibling is null, returns the parent as newleaf.
  Element<T>* FindNewLeaf (Element<T> *leaf){
    Element<T> *sibling=0, *newleaf=0;
    //bool isLeftChild=false;
    if(leaf->parent==NULL){
      printf("FindNewLeaf::Error:: A leaf node with no parent\n");fflush(stdout);
      return NULL;
    }else {
      //printf("FindNewLeaf:Current Leaf's parent is %p, with value %d\n",leaf->parent, leaf->parent->data);fflush(stdout);
    }
    if(leaf->parent->left==leaf){
      sibling=leaf->parent->right;
      leaf->parent->left=NULL;//found the sibling, now lose pointer to current leaf    
      //printf("FindNewLeaf:: Leaf is left child \n");
    }else if(leaf->parent->right==leaf){
      sibling=leaf->parent->left;
      leaf->parent->right=NULL;//found the sibling, now lose pointer to current leaf
      //printf("FindNewLeaf:: Leaf is right child \n");
    }
    else {
      printf("FindNewLeaf:: Insert Error: None of my parent's child is me\n");
    }
    if(sibling==0){
      //printf("FindNewLeaf:: sibling is null, parent is new leaf\n");fflush(stdout);
      newleaf=leaf->parent;
    }else {
      //printf("FindNewLeaf::Sibling of current leaf is %p, with value %d\n", sibling, sibling->data);fflush(stdout);
      newleaf=FindLeaf(sibling);
    }
    newleaf->right=NULL;
    newleaf->left=NULL;
    
    return newleaf;
  }
  
  Element<T>* FindLeaf (Element<T> *node){
    Element<T>* _leaf;
    if(node==NULL){
      printf("FindLeaf:: Error: the starting point of tree is NULL \n");fflush(stdout);
    }
    //printf("FindLeaf:: Current Node value %d \n", node->data);
    if(node->left==0 && node->right==0){
      _leaf=node;
    }
    else {
      if(node->left){
	_leaf=FindLeaf(node->left);
      }else{
	_leaf=FindLeaf(node->right);
      }
    }
    return _leaf;
  }
  
 
  
  void BubbleDown(Element<T>* e)
  {
    Element<T>* candidatePtr = e;

    bool swapFlag=false;
    //bool leftFlag=false;
    // Compare with both the children and replace with the
    
    //compare with left first
    if(e->left){
      if(e->left && compareClass.LessThan(e->left->data, e->data)){
	candidatePtr=e->left;
	swapFlag=true;
      }
    }
    
    //Based on above result, compare either left and right or current element and right
    if(e->right && swapFlag) {
      if(e->right && compareClass.LessThan(e->right->data, e->left->data)){
	candidatePtr=e->right;
	//leftFlag=true;
      }
    }else {
      if(e->right){
	if(compareClass.LessThan(e->data, e->right->data)){
	  candidatePtr=e;
	  swapFlag=false;
	}else {
	  candidatePtr=e->right;
	  swapFlag=true;
	}
      }
    }  
      
    if(swapFlag && candidatePtr!=e)
    {
      Swap(e,candidatePtr);
      BubbleDown(candidatePtr);
    }

  }

  void BubbleUp(Element<T>* e)
  {
    if(e == root) return; /* at root/root of heap, there are no parents */

    //compare e with its parent, if condition is true, swap and bubble up again.  
    if(compareClass(e->data, e->parent->data))
    {
      Swap(e ,e->parent);
      BubbleUp(e->parent);
    }
  }
  
  void Swap(Element<T>* e1, Element<T>* e2)
  {
    T temp = e1->data;
    e1->data = e2->data;
    e2->data = temp;
  }
/*
  Element<T> * GetLastParent(Element<T> *parent, uint32_t &height) {
    height++;
    if (parent->left==NULL || parent->right==NULL)
        return parent;
    else {
      uint32_t left_height=0,right_height=0;
      Element<T> *left = GetLastParent(parent->left,&left_height);
      Element<T> *right = GetLastParent(parent->right,&right_height);
      if (left_height == right_height){
	height += right_height;
	return right;
      }
      else if (left_height > right_height) {
	height += left_height;
	return left;
      }
    }
  }
  */
  void InsertAtRightPlace(Element<T> * refNode,Element<T> * newNode, uint16_t &height){    
    //compare the reference node data with new data
    height++;
    if(compareClass.LessThan(refNode->data, newNode->data)){ 
      //insert below reference node
      if(refNode->left==NULL){
	  leaf = newNode;
          refNode->left = newNode;
	  newNode->parent = refNode;
      }
      else if(refNode->right==NULL) {
	  leaf = newNode;
          refNode->right = newNode;
	  newNode->parent = refNode;
      }
      else if(compareClass.LessThan(refNode->left->data, newNode->data))
         InsertAtRightPlace(refNode->left, newNode, height);
      else if(compareClass.LessThan(refNode->right->data, newNode->data))
         InsertAtRightPlace(refNode->right, newNode, height);
      else //insert between node and its any one child
         InsertBetween(refNode, refNode->left, newNode);
    }
    else {  //if new obj is in betweeen reference node and its parent value
      //insert between node and its parent
      InsertBetween(refNode->parent, refNode, newNode);
    }
  }
  
  //Inserts a node between parent p and child c in the tree
  void InsertBetween (Element<T> *p, Element<T> *c, Element<T> *newnode){
    
    if(p==0)  //if node c is root
    { 
      //new node become root and takes the left branch of oldroot,
      //oldroot becomes the right branch of newroot. oldroots, left branch is 
      //now empty
      Element<T> *oldroot=root;
      root = newnode;
      root->parent=0;
      //if the old root had a left branch, new root takes it as its left branch
      if(oldroot->left){
	root->left = oldroot->left;
	oldroot->left->parent=root;
	oldroot->left=0;
      }
      //old root becomes new roots right branch
      root->right = oldroot;
      oldroot->parent=root; 
    }
    else
    {
      //insert newnode inbetween p and c, newnodes both branchs are empty
       newnode->parent = p;
       newnode->left  = c;
       c->parent=newnode;
       if(p->left == c)
       {
           p->left = newnode;
       }
       else p->right = newnode;
    }
  }
};


#endif /* HEAPT_H_ */
