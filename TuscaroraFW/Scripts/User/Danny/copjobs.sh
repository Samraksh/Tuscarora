#!/usr/bin/env bash
# ./runSim.sh -q --force --no-compile Cop -- -- \
#       Size '3 4 5' \
#       RunTime '3 3 4' \
#       Mobility 'ConstantPositionMobilityModel GaussMarkovMobilityModel RandomWalk2dMobilityModel RandomWaypointMobilityModel' \
#       DistanceSensitivity '1'

./gitupdateservers.sh && \
./runSim.sh -q --force --no-compile CopB -- \
      Size '30 100 300 1000' \
      RunTime 360 \
      Mobility 'ConstantPositionMobilityModel GaussMarkovMobilityModel RandomWalk2dMobilityModel RandomWaypointMobilityModel' \
      -- DistanceSensitivity '0 0.5 1 1.5'
