Synopsis
======

CopAux.{h,cpp} - common data structures used in both:
   CopTimestamp_t
   CopItem - info transmitted in messages
   CopMessage - container class for messages.
     - static method fromWire() parses buffer, returns instance.
     - instance method toWire() encodes into buffer.  Various accessor/mutator methods also included.

Cop.{h,cpp} - base class for BCop (broadcast) and RCop (individual message send)

BCop.{h,cpp} - the original Cop, refactored to use the above data structures.  I think it should work the same.

RCop.{h,cpp} - new version that chooses a best neighbor to send to (based on potential staleness removal) and sends items that will do the most good.


Todo for Daniel:
===========

 [x] Merge updated main branch with our (rcop) branch, so we can use
   more up-to-date features of the framework i/f.

 [ ] Put in logging code to record staleness every second and also the info about each received message that was being recorded in the original Cop.  This will include putting in the timer event delegate.

 [ ] Set up relevant tests/drivers in Tests directory.


Todo for Ken:
=========

 [x] Add code to handle acks, including updating neighbor state.

 [x] Add code to BCop to deal with failed xmissions.  Save a copy of
  the xmitted message.

 [ ] Add code to RCop to deal with unacked messages.  Handle retransmissions
  as part of the regular ShareData method.

 [ ] Check memory management to make sure we're not doing something stupid.
