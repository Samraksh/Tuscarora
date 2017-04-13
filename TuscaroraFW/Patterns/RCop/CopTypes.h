////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _COP_TYPES_H_
#define _COP_TYPES_H_

#include <cassert>
#include <cstring>  // memcpy
#include <string>   // std::string
#include <sstream>  // std::stringstream
#include <Base/FrameworkTypes.h> // NodeId_t
#include <Sys/netconvert.h>
//#include "Lib/Misc/datastructs.h" // map_t, vector_t
#include <Lib/DS/BSTMapT.h>
#include <Lib/DS/BiBSTMapT.h>
#include <Lib/DS/ListT.h>


#include "../PTN_PacketHandling.h"

using namespace PAL;
using namespace std;

namespace Patterns {
namespace Cop {

static const int COP_INFO_TARGET_SIZE=32;
static const int COP_DATA_SIZE=COP_INFO_TARGET_SIZE - sizeof(uint16_t);

typedef Patterns::PatternTimestamp_t CopTimestamp_t;
typedef float Distance_t;  
  
/* XXXXX this is a hack to save typing.  The assert()s should be removed
 * after debugging. */
#define READ_TYPE(buf,t,limit,dest) {	\
  assert(buf+sizeof(t)<limit); \
  memcpy(&dest,buf,sizeof(t)); \
  buf += sizeof(t); \
  dest = ntoh(dest); }

  

// data structures for *Cop
struct NodeState {
	NodeId_t node;    // which node
	CopTimestamp_t ts;  // sourceTime time of info
	NodeState(): node(0), ts(0) {};
	NodeState(NodeId_t s, CopTimestamp_t t): node(s), ts(t) {};
	uint8_t *ToWire(uint8_t *buffer) const;

	bool operator<(NodeState const& r) const {
		return (node < r.node);
	}
};



// Basic element that stores information about a node
struct CopNodeInfo {
	static const uint16_t MAXPRIO = 0xffff;
	NodeId_t source;
	Distance_t distance;
	CopTimestamp_t sourceTime;
	uint16_t prio;  // "benefit" to receiver; LOWER is better (not xmitted)
	uint16_t dataSize;
	uint8_t data[COP_DATA_SIZE];

	bool operator<(CopNodeInfo const& r) const {
		if (distance == r.distance) return (source<r.source);
		return (distance < r.distance);
	}
	
	bool operator==(CopNodeInfo const& r) const {
		return (source==r.source) & (distance==r.distance);
	}

	bool InfoLessThan (CopNodeInfo const& r) const {
		return (prio < r.prio)
	||   ((prio==r.prio) && (sourceTime > r.sourceTime)) // newer
	||   ((prio==r.prio) && (sourceTime==r.sourceTime) && (distance < r.distance)); // closer
	}
	
	// NOTE: prio not considered in equality 
	bool InfoEquality(CopNodeInfo const& r) const {
		return (source==r.source) && (distance==r.distance) && (sourceTime==r.sourceTime)
		&& (dataSize==r.dataSize) && (memcmp(data,r.data,dataSize)==0);
	}

	static uint16_t MinLength() {
		uint16_t ret= sizeof(NodeId_t) + sizeof(uint16_t) + sizeof(CopTimestamp_t) +
			sizeof(uint16_t);
		return ret;
	}
	uint16_t GetWireLength() const {return dataSize;}
	NodeId_t GetSource() const { return this->source; }
	//float GetDistance() const  { return this->distance; }
	CopTimestamp_t GetSourceTime() const { return this->sourceTime; }
	bool SetData(uint16_t size, const uint8_t *_data){
		if (size > COP_DATA_SIZE) return false;
		
		memcpy(data,_data,size);
		return true;
	}
	
	uint8_t* ToWire(uint8_t *buffer, uint16_t bufsize) const{
		uint8_t* ret=buffer;
		if(int(bufsize) >= sizeof(CopNodeInfo)) {
			memcpy(buffer, this, sizeof(CopNodeInfo));
			ret+=sizeof(CopNodeInfo);
		}
		return ret;
	}
	
	uint8_t* FromWire(uint8_t *buffer, uint8_t bufsize){
		uint8_t* ret=buffer;
		if(bufsize >= int(sizeof(CopNodeInfo))) {
			memcpy(this, buffer, sizeof(CopNodeInfo));
			ret+=sizeof(CopNodeInfo);
		}
		return ret;
	}
	
	std::string ToString() const{
		string ret;
		ostringstream oss;
		oss << source << distance << sourceTime << prio << dataSize ;
		ret.append(oss.str());
		ret.append(string((char*)data));
		return ret;
	}
	
	std::string ToStringVerbose() const{
		string ret;
		ostringstream oss;
		oss << "Node Id:"<< source <<", Distance:"<< distance << ", Source Timestamp:"<< 	sourceTime << ", Priority:"<< prio <<", Datasize:"<< dataSize << ", Data:";
		ret.append(oss.str());
		ret.append(string((char*)data));
		return ret;
	}
	
}; //struct CopNodeInfo 


/* This is what is stored in the Cop table.
* Cop (at least this version) doesn't keep separate info on neighbors.
*/
struct Row {
	CopNodeInfo item; // the node'sinfo (composition statt subclassing)
	CopTimestamp_t received;     // time this information was received
	CopTimestamp_t rebroadcast;  // time this information was rebroadcast
	uint8_t seen; // the number of times this data was seen on the network
	Row(){
		received=((CopTimestamp_t) 0);
		rebroadcast=((CopTimestamp_t) 0);
		seen=0; 
	};
	string ToStr() {
		std::stringstream ss;
		ss << "recvd=" << this->received << ",rebrcst=" << this-rebroadcast;
		ss << ",seen=" << ((int) this->seen) << "\n";
		return this->item.ToStringVerbose() + ss.str();
	};
	
	/*bool operator < (const Row &B) {
		if (item.distance == B.item.distance) return (item.source < B.item.source); 
		return (item.distance < B.item.distance);
	}
	
	bool operator == (const Row &B) {
		return (item.distance == B.item.distance) && (item.source == B.item.source);
	}*/
};



class CompareCopNodeInfo {
public:
	bool operator () (CopNodeInfo& A,  CopNodeInfo& B) {
		return (A.source == B.source);
	}
	bool LessThan (CopNodeInfo& A, CopNodeInfo& B) {
		return A.source < B.source;
	}
	bool DuplicateOf (CopNodeInfo& A,  CopNodeInfo& B) {
		return (A.source == B.source) ;
	}
};


//typedef map_t < NodeId_t, Row*>::type CopTable_t;


/*
struct CopNodeId {
	NodeId_t nodeId;
	Distance_t distance;
	
	CopNodeId(NodeId_t n=0, Distance_t d=-1){
		nodeId =n; distance = d;
	}
	
	bool operator < (const CopNodeId &B){
		if (distance == B.distance) return (nodeId < B.nodeId); 
		return (distance < B.distance);
	}
	
	bool operator == (const CopNodeId &B){
		return (distance == B.distance) && (nodeId == B.nodeId);
	}
};
*/

class RowPtrComparator {
public:
	bool LessThan (Row* A, Row* B) {
		return (A->item < B->item);
	}
	bool EqualTo (Row* A, Row* B) {
		return (A->item == B->item);
	}
};



//typedef BSTMapT< CopNodeId, Row*> CopTable_t;
typedef BSTMapT< NodeId_t, Row*> CopTable_t;
//typedef BiBSTMapT< NodeId_t, Row*, RowPtrComparator > CopTable_t;
	
typedef ListT<CopNodeInfo, false, CompareCopNodeInfo> CopItemList_t;
typedef SortedListT<Row*, false, RowPtrComparator> RowList_t;

/* This class is mainly for parsing and formatting messages.
 * Nothing in this class should ever call "new".
 * Invariants:
 *    wireLength is the length of the encoded message created by toWire().
 *    sourceInfo is valid.
 */
class CopMessage {
	CopNodeInfo sourceInfo;                    // Sender's Id, timestamp, data
	uint8_t sequence;                      // used when sending multiple msgs;
	CopTimestamp_t sentTime;               // time message was sent
	//std::set<NodeState> have;           // nodes I have info about
	CopItemList_t items;            // info from my CopTable
	uint16_t wireLength;   // invariant: this is constant
	//bool wireLengthValid;

	public:
	static const int COPMESSAGE_MAXHAVES = 20;
	CopMessage() {
		//wireLengthValid = false;
		wireLength = sizeof(CopNodeInfo);
		sentTime = 0;
		sequence = 0;
	};  // everything zero - only used for fromWire?

	
	// Note: sourceInfo is final
	CopMessage(CopNodeInfo si, uint8_t seq, CopTimestamp_t t) {
		//wireLengthValid = true;
		wireLength = sizeof(CopNodeInfo);
		sourceInfo = si;
		sentTime = t;
		sequence = seq;
	};

	// NB not const b/c of updating wireLength field. 
	//Mukundan. Not sure of this. Looks like wirelength is intended to be updated by this call. but no parameter is passed.
	uint16_t GetWireLength() {return wireLength;} 

	uint16_t GetHeaderLength() const;

	/*std::set<NodeState>::const_iterator HavesBegin() {
		return this->have.begin();
	}
	std::set<NodeState>::const_iterator HavesEnd() {
	return this->have.end();
	}
	
	std::set<CopNodeInfo>::const_iterator ItemsBegin() {
	return this->items. .begin();
	}
	std::set<CopNodeInfo>::const_iterator ItemsEnd() {
	return this->items.end();
	}
	*/
	
	CopItemList_t::Iterator ItemsBegin() {
		return this->items.Begin();
	}
	CopItemList_t::Iterator ItemsEnd() {
		return this->items.End();
	}
	
	/* accessors */
	CopNodeInfo GetSourceInfo() { return this->sourceInfo; }
	uint8_t GetSequence() const { return this->sequence; }
	CopTimestamp_t GetSentTime() const { return this->sentTime; }
	//unsigned GetNumberOfHaves() { return this->have.size(); }
	unsigned GetNumberOfItems() { return this->items.Size(); }

	/* mutators */
	void SetSourceInfo(CopNodeInfo& si) { sourceInfo=si; }
	void SetSentTime(CopTimestamp_t t) { this->sentTime = t; }
	void IncrementSequence() { this->sequence += 1; }
	bool AddItem(CopNodeInfo& i){
		items.InsertBack(i);
		wireLength+=sizeof(CopNodeInfo);
		return true;
	}
	//bool AddHave(NodeState& s){return false;}
	void Reset() {
		//this->have.clear();
		this->items.DeleteAll();
		this->wireLength = 0;
		//this->wireLengthValid = false;
	}

	uint8_t* ToWire (uint8_t *buffer, int16_t bufsize) {
		uint8_t *ret=buffer;
		uint16_t itemSize = sizeof(CopNodeInfo);
		
		if (bufsize < itemSize) return ret;
		
		//first copy my information.
		ret = sourceInfo.ToWire(buffer,bufsize);
		bufsize-= itemSize;
		
		//now copy others info from the itemslist
		uint i=0;
		while (i < items.Size() && bufsize >= itemSize){
			ret = items[i].ToWire(ret,bufsize);
			bufsize-= sizeof(CopNodeInfo);
			i++;
		}
		return ret;
	}

	static bool FromWire(uint8_t *bufp, uint8_t *buflimit, CopMessage& cm){
		//First extract the sourceInfo
		CopNodeInfo *si = (CopNodeInfo*) bufp;
		cm.SetSourceInfo(*si);

		bufp+=sizeof(CopNodeInfo);
		int items=0;
		while (bufp+sizeof(CopNodeInfo) <= buflimit){
			CopNodeInfo *item = new CopNodeInfo();
			bufp=item->FromWire(bufp, sizeof(CopNodeInfo));
			cm.AddItem(*item);
			items++;
			//Debug_Printf(DBG_PATTERN, "CopMessage::FromWire:: Extracted : %i items from message\n", items);
		}
		return true;
	}

	//Implementation is incomple
	std::string ToString(){
		//std::string ret;
		stringstream ss;
		ss<< "CopNodeInfo: "<< sourceInfo.ToString() ;
		ss<<", Sequence: "<< sequence << ", Sent Timestamp: "<< sentTime << ", Wirelength: "<< wireLength ;
		return ss.str();
	}
	
}; // class CopMessage


} // namespace Cop
} // namespace Patterns


#endif //_COP_TYPES_H_