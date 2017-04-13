////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "standard-includes.h"
#include "physical-world-hybrid.h"

#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/node-container.h"


int main (int argc, char *argv[])
{
  SimulationOpts args ( argc - 1, argv+1 );
  physicalWorld_hybrid_t physicalWorld ( &args );
  physicalWorld.setMobility();
  physicalWorld.setDce();
  
  unsigned numNodes = args.nodes;
  double startx[numNodes];
  double starty[numNodes];
  double stopx[numNodes];
  double stopy[numNodes];

  //store start positiion of nodes
  NodeContainer glob = NodeContainer::GetGlobal();
    
  for(int id = 0; id < glob.GetN(); id++) {
    Ptr<ns3::Node> nl = glob.Get(id);
    startx[id] = nl->GetObject<MobilityModel>()->GetPosition().x;
    starty[id] = nl->GetObject<MobilityModel>()->GetPosition().y;
  }

  //Start simulation
  Simulator::Stop ( Seconds ( args.runTime ));
  Simulator::Run ();

  if ( args.verbose ) printf ( "Simulator done\n" );

  //store stop position of nodes
  glob = NodeContainer::GetGlobal();
  for(int id = 0; id < numNodes; id++) {
    Ptr<ns3::Node> nl = glob.Get(id);
    stopx[id] = nl->GetObject<MobilityModel>()->GetPosition().x;
    stopy[id] = nl->GetObject<MobilityModel>()->GetPosition().y;
  }
  //print position deltas
  for(int id = 0; id < numNodes; id++) {
    printf("%f\n", sqrt((startx[id] - stopx[id]) * (startx[id] - stopx[id]) + (starty[id] - stopy[id]) * (starty[id] - stopy[id])));
  }

  Simulator::Destroy ();
}
