////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "SimpleGenericComparator.h"


#ifndef AVLBST_H_
#define AVLBST_H_


typedef int32_t AVLBSTHeight_t;

template <class T>
class AVLBSTElement {
protected:
	AVLBSTHeight_t depth;
	AVLBSTElement<T> *right, *left, *parent;
	T data;
public:

	AVLBSTElement(const T &_data){
		data = _data;
		right = NULL;
		left = NULL;
		parent = NULL;
		//	depth_l = 0;
		//	depth_r = 0;
		depth = 1;
		//printf("Done constructing AVLBSTElement\n");fflush(stdout);
	};
	AVLBSTElement(){
		//data = _data;
		right = NULL;
		left = NULL;
		parent= NULL;
		//	depth_l = 0;
		//	depth_r = 0;
		depth = 1;
		//printf("Done constructing AVLBSTElement\n");fflush(stdout);
	};
	AVLBSTElement<T> * ReturnSidePtrWithMaxDepth(){
		if(DepthL()<DepthR()) return left;
		else return right;
	};
	AVLBSTElement<T>* FindLeftOrRightPtr(AVLBSTElement<T>* inputptr){
		if(left==inputptr) return left;
		else if(right==inputptr) return right;
		else {
			return(NULL);// PROBLEM !!! Careful
		}
	};
	bool ReplaceLeftOrRightPtr(AVLBSTElement<T>* oldnode,AVLBSTElement<T>* newnode){
		if(left==oldnode) {
			left = newnode;
			return true;
		}
		else if(right==oldnode) {
			right = newnode;
			return true;
		}
		return false;
	};
	AVLBSTHeight_t CalculateDepth(){
	  AVLBSTHeight_t dl = DepthL();
	  AVLBSTHeight_t dr = DepthR();
		if(dl>dr) return(dl+1);
		else  return(dr+1);
	};
	AVLBSTHeight_t DepthL(){
		if(left) return(left->depth);
		else return 0;
	};
	AVLBSTHeight_t DepthR(){
		if(right) return(right->depth);
		else return 0;
	};


	AVLBSTHeight_t CalcBalanceFactor(){
		return(DepthL() - DepthR());

	};

/*	T ParentData(){
		if(parent) return(parent->data);
		else return(99);
	};
	T LeftData(){
		if(left) return(left->data);
		else return(99);
	};
	T RightData(){
		if(right) return(right->data);
		else return(99);
	};*/


	const T& GetData(){return data;}

	void operator=(const AVLBSTElement<T> &rhs){
		data = rhs.data;
		right = rhs.right;
		left = rhs.left;
		parent = rhs.parent;
		depth = rhs.depth;
	};
	template <class U, class CompareU> friend class AVLBST_T;
	template <class U2, class U3, class u4> friend class BSTMapT;
	friend class AVLBSTTest;

	template <uint32_t ARRAY_LEN, class U, class CompareU> friend class FAAVLBST_T;
	template <uint32_t ARRAY_LEN, class U2, class U3, class u4> friend class FABSTMapT;
	friend class FAAVLBSTTest;
	friend class FAMapTest;
};


template <class T, class Compare = SimpleGenericComparator<T> >
class AVLBST_T {
	uint32_t noOfElements;
	Compare compareClass;
// 	bool DBG_TEST; // set in Lib/Misc/Debug.cpp
	AVLBSTElement<T> *root;

public:
	AVLBST_T(){
		noOfElements = 0;
		root = NULL;
// 		DBG_TEST=false; // set in Lib/Misc/Debug.cpp
	};
	~AVLBST_T(){Clear();}
	AVLBSTElement<T>* GetRootPtr(){ return root;};
	T PeekRoot(){
		if(root) return root->data;
		else return(99); //BK Change this to errro
	};
	T ExtractRoot(){
		T rtn = root->data;
		Delete(rtn);
		return rtn;
	};
	bool Insert(const T &_data){
		//if(!(&_data){}
		AVLBSTElement<T>* newNode = new AVLBSTElement<T>(_data);
		if(newNode){
			if(root == NULL){ //root is null, 1st AVLBSTElement
				root=newNode;
				root->parent=0;
			}
			else {
				InsertAtRightPlace(root, newNode);
			}
			noOfElements++;
			return true;
		}
		return false;
	};

	AVLBSTElement<T>* Search(const T& _data) const{
		return Search (root, _data);
	};

	bool Delete(const T &_data){
		AVLBSTElement<T>* node = Search(_data);
		return DeleteElement(node);
	};

	uint32_t Size() const {return noOfElements;};

	AVLBSTElement<T>* Begin() const{
		if(!root) return(NULL);
		else{
			return(GoDeepestLeft(root));
		}
	};
	AVLBSTElement<T>* End() const{
		if(!root) return(NULL);
		else{
			return(GoDeepestRight(root));
		}
	};
	T& PeekBegin(){
	  return Begin()->data; 
	}
	T& PeekEnd(){
	  return End()->data; 
	}
	T ExtractBegin(){
	  AVLBSTElement<T>* begin = Begin();
	  T t=  begin->data; 
	  DeleteElement(begin);
	  return t;
	}
	T ExtractEnd(){
	  AVLBSTElement<T>* end = End();
	  T t=  end->data; 
	  DeleteElement(end);
	  return t;
	}
	
	AVLBSTElement<T>* Next(AVLBSTElement<T>* refNode) const{
		if(!refNode) return(NULL);
		else if (refNode->right) return(GoDeepestLeft(refNode->right));
		else return(FPPWL(refNode));
	};
	AVLBSTElement<T>* Previous(AVLBSTElement<T>* refNode) const{
		if(!refNode) return(NULL);
		else if (refNode->left) return(GoDeepestRight(refNode->left));
		else return(FPPWR(refNode));
	};

	bool DeleteElement(AVLBSTElement<T>* node){
		if (!node) return false;
		//bool leftChildPresent;
		//bool rightChildPresent;

		if((!node->left) &&  (!node->right)){
			Delete0ChildNode(node);
		}
		else if((!node->left) ||  (!node->right)){ //no left children, easy case
			Delete1ChildNode(node);
		}
		else { //Both children present
			if (node->DepthL() >= node->DepthR()) {
				AVLBSTElement<T>* maxChild = FindMaxChild(node->left); //in-order predecessor
				//Change Places node and maxchild
				ExchangePlaces(maxChild, node);
				Delete1ChildNode(node);
			}
			else{
				AVLBSTElement<T>* minChild = FindMinChild(node->right); //in-order successor
				ExchangePlaces(minChild, node);
				Delete1ChildNode(node);
			}
		}
		noOfElements--;
		return true;
	}

	void Clear(){
		while(noOfElements != 0){
			DeleteElement(Begin());
		}
	}

private:

	void ExchangeParentAndChild(AVLBSTElement<T>* parent, AVLBSTElement<T>* child){
		if(!parent || !child) return;
		AVLBSTElement<T> parentcopy = *parent;
		AVLBSTElement<T> childcopy = *child;
		child->parent = parent->parent;

		if (child->parent) child->parent->ReplaceLeftOrRightPtr(parent,child);
		if(parentcopy.left){
			if(parentcopy.left == child) {
				child->left = parent;
				child->left->parent = child;
			}
			else  {
				child->left = parentcopy.left;
				child->left->parent = child;
			}
		}
		else {
			child->left = NULL;
		}
		if(parentcopy.right){
			if(parentcopy.right == child) {
				child->right = parent;
				child->right->parent = child;
			}
			else  {
				child->right = parentcopy.right;
				child->right->parent = child;
			}
		}
		else{
			child->right = NULL;
		}

		parent->left = childcopy.left;
		parent->right = childcopy.right;
		if(childcopy.left){
			childcopy.left->parent = parent;
		}
		if(childcopy.right) {
			childcopy.right->parent = parent;
		}
		parent->depth = parent->CalculateDepth();
		child->depth = child->CalculateDepth();
	};
	void ExchangePlaces(AVLBSTElement<T>* A, AVLBSTElement<T>* B){
		if (A->depth > B->depth) ExchangeOldandYoungLocations(A,B);
		else ExchangeOldandYoungLocations(B,A);
	};

	void ExchangeOldandYoungLocations(AVLBSTElement<T>* parent, AVLBSTElement<T>* child){
		if(root ==parent) root=child;
		AVLBSTElement<T> tempNode;
		AVLBSTElement<T>* curElement;
		if(parent->FindLeftOrRightPtr(child)){
			ExchangeParentAndChild(parent, child);
		}
		else if(child->FindLeftOrRightPtr(parent)){
			ExchangeParentAndChild(child, parent);
		}
		else{
			//AVLBSTElement<T> parentcopy = *parent;
			//AVLBSTElement<T> childcopy = *child;
			//adjust parents' left and right ptrs
			//a child->parent = parent->parent;
			if(parent->parent) parent->parent->ReplaceLeftOrRightPtr(parent,child);
			if(child->parent) child->parent->ReplaceLeftOrRightPtr(child,parent);

			if(parent->left && parent->left!=child) parent->left->parent = child;
			if(parent->right && parent->left!=child) parent->right->parent = child;
			if(child->left && child->left!=parent) child->left->parent = parent;
			if(child->right && child->left!=parent) child->right->parent = parent;

			curElement = parent->parent;
			parent->parent = child->parent;
			child->parent = curElement;

			curElement = parent->left;
			parent->left = child->left;
			child->left = curElement;

			curElement = parent->right;
			parent->right = child->right;
			child->right = curElement;

			parent->depth = parent->CalculateDepth();
			child->depth = child->CalculateDepth();
		}
	};


	/* void PrintTreeStructured(){
    //DebugNoTime_Printf(DBG_TEST, "********BEGIN Tree*********\n");
    //rintElemnentsofTreeDepthFirst(shortAVLBST->root);
    PrintElemnentsofTreeStructured(root);
    //DebugNoTime_Printf(DBG_TEST, "\n--------END  Tree---------\n");
  }
  void PrintElemnentsofTreeStructured(AVLBSTElement<T> *curElement, AVLBSTHeight_t offsetheight=0 ){
    if(curElement){
      if(curElement->left) {
	      PrintElemnentsofTreeStructured(curElement->left,curElement->DepthL() < curElement->DepthR() ? offsetheight + (curElement->DepthR() - curElement->DepthL())  : offsetheight);
	      ////DebugNoTime_Printf(DBG_TEST,"            ");
      }
      else {
      //	//DebugNoTime_Printf(DBG_TEST,"\n");
      }
      for(int i=0; i < ((curElement->CalculateDepth()-1)+ offsetheight); i++){
	      //DebugNoTime_Printf(DBG_TEST,"                      ");
      }
      //DebugNoTime_Printf(DBG_TEST,"%2d(%2d_%2d_%2d)(%2d_%2d)",curElement->data, curElement->LeftData(), curElement->RightData(), curElement->ParentData(), curElement->DepthL(),curElement->DepthR());
      //DebugNoTime_Printf(DBG_TEST,"\n");
      if(curElement->right) {
	      PrintElemnentsofTreeStructured(curElement->right,curElement->DepthR() < curElement->DepthL() ? offsetheight + (curElement->DepthL() - curElement->DepthR())  : offsetheight);
      }
      else{
	      ////DebugNoTime_Printf(DBG_TEST,"\n");
      }
    }
  }*/

	AVLBSTElement<T> * GoDeepestLeft(AVLBSTElement<T> * refNode) const{
		while(refNode->left){
			refNode = refNode->left;
		}
		return(refNode);
	};

	AVLBSTElement<T> * GoDeepestRight(AVLBSTElement<T> * refNode) const{
		while(refNode->right){
			refNode = refNode->right;
		}
		return(refNode);
	};

	AVLBSTElement<T> * FPSDMS(AVLBSTElement<T> * refNode) const{ //First parent that is smaller than myself
		AVLBSTElement<T> * rptr;
		if(refNode){
			rptr = refNode;
			while(rptr->parent){
				rptr = rptr->parent;
				if( Compare::LessThan(rptr->data, refNode->data) ){ //go next parent
					return(rptr);
				}
			}
		}
		return(NULL);
	};
	AVLBSTElement<T> * FPLDMS(AVLBSTElement<T> * refNode)const{ //First parent that is larger  than or equal to myself
		AVLBSTElement<T> * rptr;
		if(refNode){
			rptr = refNode;
			while(rptr->parent){
				rptr = rptr->parent;
				if( Compare::LessThan(refNode->data, rptr->data) ){ //if t
					return(rptr);
				}
			}
		}
		return(NULL);
	};

	AVLBSTElement<T> * FPPWL(AVLBSTElement<T> * refNode)const{ //First parent that points towards me with left ptr
		AVLBSTElement<T> * rptr;
		if(refNode){
			rptr = refNode;
			while(rptr->parent){
				if( IsPointedByLeft(rptr) ){ //go next parent
					rptr = rptr->parent;
					return(rptr);
				}
				rptr = rptr->parent;
			}
		}
		return(NULL);
	};

	AVLBSTElement<T> * FPPWR(AVLBSTElement<T> * refNode)const{ //First parent that points towards me with right ptr
		AVLBSTElement<T> * rptr;
		if(refNode){
			rptr = refNode;
			while(rptr->parent){
				if( IsPointedByRight(rptr) ){ //go next parent
					rptr = rptr->parent;
					return(rptr);
				}
				rptr = rptr->parent;
			}
		}
		return(NULL);
	};

	bool IsPointedByLeft(AVLBSTElement<T>* inputptr)const{
		if(!inputptr) return false;
		else if(!(inputptr->parent)) return false;
		else if(!(inputptr->parent->left)) return false;
		else if ( (inputptr->parent->left) == inputptr) return true;
		else return false;
	};
	bool IsPointedByRight(AVLBSTElement<T>* inputptr)const{
		if(!inputptr) return false;
		else if(!(inputptr->parent)) return false;
		else if(!(inputptr->parent->right)) return false;
		else if ( (inputptr->parent->right) == inputptr) return true;
		else return false;
	};

	void UpdateParentsDepth(AVLBSTElement<T> * refNode ) {
		if(refNode != NULL){
			refNode->depth = refNode->CalculateDepth();
			if(refNode->parent != NULL && refNode->parent->depth != refNode->depth+1) {
				UpdateParentsDepth(refNode->parent);
			}
		};
	};
	void RecursivelyCheckParentsForRotation(AVLBSTElement<T> * refNode){
		if( refNode->CalcBalanceFactor()>=2) CheckLeftforRotation(refNode);
		else if( refNode->CalcBalanceFactor()<=-2 ) CheckRightforRotation(refNode);
		if(refNode->parent) RecursivelyCheckParentsForRotation(refNode->parent);
	};
	void Delete0ChildNode(AVLBSTElement<T> * node){
		if(node->parent != NULL){//Adjust parent's pointer
			node->parent->ReplaceLeftOrRightPtr(node,NULL);
			UpdateParentsDepth(node->parent);
			//DebugNoTime_Printf(DBG_TEST, "216: PRINTING TREE AFTER PARENTS DEPTH \n");
			//PrintTreeStructured();
			//DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
			RecursivelyCheckParentsForRotation(node->parent);
			delete(node);
		}
		else {
			root = NULL;
			delete(node);
		}
	};
	void Delete1ChildNode(AVLBSTElement<T> * node){
		//double check no child case
		if(!(node->right)&& !(node->left)){
			Delete0ChildNode(node);
		}
		else if ((node->right) && !(node->left)){ //if there is right child
			if(node->parent != NULL){
				node->right->parent = node->parent;
				node->parent->ReplaceLeftOrRightPtr(node,node->right);
			}
			else{
				node->right->parent = NULL;
				root = node->right;
			}
			UpdateParentsDepth(node->parent);
			//DebugNoTime_Printf(DBG_TEST, "237: PRINTING TREE AFTER PARENTS DEPTH \n");
			//PrintTreeStructured();
			//DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
			RecursivelyCheckParentsForRotation(node);
			delete(node);
		}
		else if (!(node->right) && (node->left)){
			if(node->parent != NULL){
				node->left->parent = node->parent;
				node->parent->ReplaceLeftOrRightPtr(node, node->left);
			}
			else{
				node->left->parent = NULL;
				root = node->left;
			}
			UpdateParentsDepth(node->parent);
			//DebugNoTime_Printf(DBG_TEST, "252: PRINTING TREE AFTER PARENTS DEPTH \n");
			//PrintTreeStructured();
			//DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
			RecursivelyCheckParentsForRotation(node);
			delete(node);
		}
		else {
			//error case: How did you come here?
		}

	};
	AVLBSTElement<T>* FindMaxChild(AVLBSTElement<T> * refNode)const{
		if(refNode->right) return(FindMaxChild(refNode->right));
		else return(refNode);
	};
	AVLBSTElement<T>* FindMinChild(AVLBSTElement<T> * refNode)const{
		if(refNode->left) return(FindMinChild(refNode->left));
		else return(refNode);
	};

	void InsertAtRightPlace(AVLBSTElement<T> *& refNode, AVLBSTElement<T> * newNode){ //Return refNode's depth
		//compare the reference node data with new data
		if(Compare::LessThan(refNode->GetData(), newNode->GetData() )){ //New node should be on the left of refNode
			//printf("Inserting on the left side of refNode\n"); fflush(stdout);
			if(refNode->left == NULL) {//Insert below reference node
				refNode->left = newNode;
				newNode->parent = refNode;
				UpdateParentsDepth(newNode->parent);
				RecursivelyCheckParentsForRotation(newNode->parent);
			}
			else{
				InsertAtRightPlace(refNode->left,newNode);
			}
		}
		else{
			//printf("Inserting on the right side of refNode\n");fflush(stdout);
			if(refNode->right == NULL) {//Insert below reference node
				refNode->right = newNode;
				newNode->parent = refNode;
				UpdateParentsDepth(newNode->parent);
				RecursivelyCheckParentsForRotation(newNode->parent);
			}
			else{
				InsertAtRightPlace(refNode->right,newNode);
			}
		}
	};

	AVLBSTElement<T>* Search(AVLBSTElement<T> *branch_root, const T& _data)const{
		if(!branch_root) {
			//printf("AVLBST::Search: Something wrong: Your branch_root is not valid\n");fflush(stdout);
			return NULL;
		}
		if(!(&branch_root->data)){
		  printf("data is at branch root is corrupt??\n"); fflush(stdout);
		  return NULL;
		}
		if(Compare::EqualTo(branch_root->data, _data)){
			
			return branch_root;
		}
		else if(Compare::LessThan(branch_root->data, _data)){
			//printf("Searching left branch\n"); fflush(stdout);
			if(branch_root->left) return Search(branch_root->left, _data);
		}
		else {
			//printf("Searching right branch\n"); fflush(stdout);
			if(branch_root->right) return Search(branch_root->right, _data);
		}
		return ((AVLBSTElement<T>*) NULL);
	};

	AVLBSTHeight_t CalcBalanceFactor(AVLBSTElement<T> * refNode)const{
		return(refNode->DepthL() - refNode->DepthR());
	};

	void TreeRotateLeft(AVLBSTElement<T>* refNode, AVLBSTElement<T> * pivot){
		if(refNode->parent) {
			refNode->parent->ReplaceLeftOrRightPtr(refNode, pivot);
			pivot->parent = refNode->parent;
		}
		else {
			root = pivot;
			pivot->parent = NULL;
		}
		refNode->parent = pivot;
		if (pivot->left) refNode->right = pivot->left;
		else refNode->right =NULL;
		pivot->left = refNode;
		if(refNode->right) refNode->right->parent = refNode;

		refNode->depth = refNode->CalculateDepth();
		pivot->depth = pivot->CalculateDepth();

	};

	void TreeRotateRight(AVLBSTElement<T> * refNode, AVLBSTElement<T> * pivot){
		if(refNode->parent) {
			refNode->parent->ReplaceLeftOrRightPtr(refNode, pivot);
			pivot->parent = refNode->parent;
		}
		else {
			root = pivot;
			pivot->parent = NULL;
		}
		refNode->parent = pivot;
		if (pivot->right) refNode->left = pivot->right;
		else refNode->left =NULL;
		pivot->right = refNode;
		if(refNode->left) refNode->left->parent = refNode;

		refNode->depth = refNode->CalculateDepth();
		pivot->depth = pivot->CalculateDepth();
	};

	void CheckLeftforRotation(AVLBSTElement<T> *  refNode){
		if(refNode->left->DepthL() >= refNode->left->DepthR() ){
			BalanceLeftLeftCase(refNode);
		}
		else {
			BalanceLeftRightCase(refNode);
		}
	};

	void CheckRightforRotation(AVLBSTElement<T> *  refNode){
		//2Cases eaither rightright ot tightleft
		if(refNode->right->DepthR() >= refNode->right->DepthL() ){
			BalanceRightRightCase(refNode);
		}
		else {
			BalanceRightLeftCase(refNode);
		}
	};

	void BalanceLeftLeftCase(AVLBSTElement<T> *& refNode) {
		TreeRotateRight(refNode, refNode->left);
		UpdateParentsDepth(refNode->parent);
		////DebugNoTime_Printf(DBG_TEST, "381: PRINTING TREE AFTER PARENTS DEPTH \n");
		//PrintTreeStructured();
		////DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
		////DebugNoTime_Printf(DBG_TEST, "398:PRINTING TREE AFTER BALANCING  \n");
		//PrintTreeStructured();
		////DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER BALANCING \n");
	};

	void BalanceRightRightCase(AVLBSTElement<T> *& refNode) {
		TreeRotateLeft(refNode, refNode->right);
		UpdateParentsDepth(refNode->parent);
		////DebugNoTime_Printf(DBG_TEST, "391: PRINTING TREE AFTER PARENTS DEPTH \n");
		//PrintTreeStructured();
		////DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
		RecursivelyCheckParentsForRotation(refNode->parent);
		////DebugNoTime_Printf(DBG_TEST, "407:PRINTING TREE AFTER BALANCING  \n");
		//PrintTreeStructured();
		////DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER BALANCING \n");
	};

	void BalanceLeftRightCase(AVLBSTElement<T> *& refNode) {
		TreeRotateLeft(refNode->left, refNode->left->right);
		TreeRotateRight(refNode, refNode->left);
		UpdateParentsDepth(refNode->parent);
		////DebugNoTime_Printf(DBG_TEST, "401: PRINTING TREE AFTER PARENTS DEPTH \n");
		//PrintTreeStructured();
		////DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
		RecursivelyCheckParentsForRotation(refNode->parent);
		////DebugNoTime_Printf(DBG_TEST, "417:PRINTING TREE AFTER BALANCING  \n");
		//PrintTreeStructured();
		////DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER BALANCING \n");
	};

	void BalanceRightLeftCase(AVLBSTElement<T> *& refNode) {
		TreeRotateRight(refNode->right, refNode->right->left);
		TreeRotateLeft(refNode, refNode->right);
		UpdateParentsDepth(refNode->parent);
		//DebugNoTime_Printf(DBG_TEST, "411:PRINTING TREE AFTER PARENTS DEPTH \n");
		//PrintTreeStructured();
		//DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER PARENTS DEPTH \n");
		RecursivelyCheckParentsForRotation(refNode->parent);
		//DebugNoTime_Printf(DBG_TEST, "427:PRINTING TREE AFTER BALANCING  \n");
		//PrintTreeStructured();
		//DebugNoTime_Printf(DBG_TEST, "END OF PRINTING TREE AFTER BALANCING \n");

	};

	/*void UpdateNodeDepth( AVLBSTElement<T> * refNode, AVLBSTHeight_t newdepth ){
	  if(refNode) refNode->depth = newdepth;
	  if(refNode->left) UpdateDepth(refNode->left, newdepth+1);
	  if(refNode->rigth) UpdateDepth(refNode->rigth, newdepth+1);
  };*/
};

#endif
