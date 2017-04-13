////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <Lib/PAL/PseudoRandom/rng-seed-manager.h>


bool  RngSeedManager::initialized =false;
bool  RngSeedManager::seedInitialized =false;
bool  RngSeedManager::runInitialized =false;
static uint64_t g_nextStreamIndex = 0;
static uint64_t g_baseStreamIndex = 0;
static uint64_t g_rngSeed = 1;
static uint64_t g_rngRun = 1;


uint32_t RngSeedManager::GetSeed (void)
{
   return g_rngSeed;
}

void 
RngSeedManager::SetSeed (uint64_t seed)
{
  g_rngSeed = seed;
  seedInitialized=true;
  initialized = seedInitialized & runInitialized;
}

void RngSeedManager::SetRun (uint64_t run)
{
  g_rngRun = run;
  runInitialized=true;
  initialized = seedInitialized & runInitialized;
}

uint64_t RngSeedManager::GetRun ()
{
  uint64_t c_run = g_rngRun;
  return c_run;
}

//Streams from 0 to STREAM_OFFSET are to be explicitly specified. These are used for node schedules that are to be reproducible.

void 
RngSeedManager::SetBaseStreamIndex (uint64_t node_id)
{
  g_nextStreamIndex = node_id*STREAMS_PER_NODE + STREAM_OFFSET;
  g_baseStreamIndex = node_id*STREAMS_PER_NODE + STREAM_OFFSET;
}

uint64_t RngSeedManager::GetNextStreamIndex (void)
{
  uint64_t next = g_nextStreamIndex;
  g_nextStreamIndex++;
  return next;
}

