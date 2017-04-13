#!/bin/bash

g++ -I ${TUS}/Include ${TUS}/bin/SimulationOpts.cpp -o ${TUS}/bin/checkArgs

chmod +x ${TUS}/bin/checkArgs

${TUS}/bin/checkArgs $@
