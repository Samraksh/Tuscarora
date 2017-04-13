////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

template<class T>
class MyLess {
public:
  bool operator () (T& A, T& B) {
    return A < B;
  }
};

template <class T>
class Element {
public:
  T data;
  Element<T> *right, *left, *parent;
  
  Element(T _data){
    data = _data;
    right =0;
    left=0;
    parent=0;
    //printf("Done constructing element\n");fflush(stdout);
  }
};


template <class T, class Compare = MyLess<T> >
class BST_T {
  uint32_t noOfElements;
  Element<T> *leaf;
  Element<T> *root;
  Compare compareClass;
  
public:
  T PeekRoot(){
    return root->data;
  }
  T ExtractRoot(){
    T rtn = root->data;
    Delete(rtn);
    return rtn;
  }
  
  bool Insert(T _data){
    Element<T>* newNode = new Element<T>(_data);
    uint16_t height=0;
    if(newNode){
      if(root==NULL){ //root is null, 1st element
	root=newNode;
	leaf=newNode;
	root->parent=0;
      }else {
	InsertAtRightPlace(root, newNode,height);
      }
      //printf("Inserted node %p at height %d, new leaf is %d\n",newNode, height, leaf->data);
      noOfElements++;
      return true;
    }
    return false;
  }
  
  
  Element<T>* Search(T _data){
    return Search (root, _data);
  }
  
  bool Delete(T _data){
    Element<T>* node=Search(_data);
    //bool leftChildPresent;
    //bool rightChildPresent;
    
    if((!node->left) && (!node->left)){ //no children, easy case 
    }
    return 0; 
  }
  
  uint32_t NoOfElements() {return noOfElements;} 
  
private:
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

  
  void InsertAtRightPlace(Element<T> * refNode,Element<T> * newNode, uint16_t &height){    
    //compare the reference node data with new data
    height++;
    if(compareClass(refNode->data, newNode->data)){ 
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
      else if(compareClass(refNode->left->data, newNode->data))
         InsertAtRightPlace(refNode->left, newNode, height);
      else if(compareClass(refNode->right->data, newNode->data))
         InsertAtRightPlace(refNode->right, newNode, height);
      else //insert between node and its any one child
         InsertBetween(refNode, refNode->left, newNode);
    }
    else {  //if new obj is in betweeen reference node and its parent value
      //insert between node and its parent
      InsertBetween(refNode->parent, refNode, newNode);
    }
  }
  
  Element<T>* Search(Element<T> *branch_root, T _data){
    if(!branch_root || branch_root->data == _data){
      return branch_root;
    }
    else if(compareClass(branch_root->left->data, _data)){
      return Search(branch_root->left, _data);
    }
    else {
      return Search(branch_root->right, _data);
    }
  }
};