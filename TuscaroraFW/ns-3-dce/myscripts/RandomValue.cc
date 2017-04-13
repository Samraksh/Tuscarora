////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");
int 
main (int argc, char *argv[])
{
  bool verbose = true;
  // uint32_t nCsma = 3;
  RngSeedManager::SetSeed(1);
  RngSeedManager::SetRun(1);
  uint64_t base = ((1ULL)<<63);
  for (uint64_t j=0; j<10; j++)
    {
      Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
      //Ptr<UniformRandomVariable> y = CreateObject<UniformRandomVariable> ();
      //Ptr<UniformRandomVariable> z = CreateObject<UniformRandomVariable> ();
      //Ptr<UniformRandomVariable> a = CreateObject<UniformRandomVariable> ();
      //printf("Setting stream value to 1\n");
      x->SetStream(j*10000);
      //y->SetStream(15000);
      //z->SetStream(25000);
      //printf("Get Random Values and its mean\n");
 
      //  printf("Address is %p\n",&x);
      double mean =0;
      printf("Printing 10000 random numbers for Seed 1, Stream %lu\n",base+x->GetStream());
      for(int i=0; i<10000; i++)
	{
	  double value = x->GetValue();
	  mean += value;
	  printf("%f ",value);
	}

      printf("\nMean1: %f\n",mean /10000);
    }  
  /*double mean1 = 0;
       printf("Stream Value  is %lld\n",y->GetStream());
       for(int j=0; j<1000; j++)
       {
       double value = y->GetValue();
       mean1 += value;
       // printf("%f\n",value);
       }
       printf("Stream 15000 mean: %f\n", mean1/1000);
       double mean2 = 0;
       printf("Stream Value is %lld\n",z->GetStream());
       for(int k=0; k<1000; k++)
       {
       double value = z->GetValue();
       mean2 += value;
       // printf("%f\n",value);
       }
       printf("Stream 25000 mean: %f\n", mean2 /1000);*/
  /*
       double mean3 =0;
       printf("Stream Value is %lld\n",a->GetStream());
       for(int i=0; i<1000; i++)
       {
       double value = a->GetValue();
       mean3 += value;
       printf("%f\n",value);
       }
       printf("Stream Random mean: %f\n",mean3 /1000);
  */
  CommandLine cmd;
  //  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  return 0;
}
