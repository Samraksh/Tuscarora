linkestimation="1 2 3 4 5 6" #1-3 is periodic with the number of missed messages, 4 is schedule aware, 5-6 is avoidance with global and oracle knowlege
discovery="1"
speeds="0.1 1 10 50 100"
densities="4 5 6 8 10 20 40"
sizes="30 50 100"
beacons="1000000 200000 100000"
interference="RangePropagationLossModel FriisPropagationLossModel"

if [ -n "$(git status --porcelain)" ]; then 
  echo "there are changes pending. Please commit your changes before continuing"; 
else 
  echo "Repository Up to Date";
fi

git push
echo "Pushed to remote"
#parallel --env _ --tag --nonall -S : -S l1 -S l2 -S l3 -S wsn-02 'export DCE_DIR=${DCE_DIR//wsn/$USER}; export LD_LIBRARY_PATH=${LD_LIBRARY_PATH//wsn/$USER}; export BAKE_HOME=${BAKE_HOME//wsn/$USER}; export DCE_PATH=${DCE_PATH//wsn/$USER};  export NS3_DCE=${NS3_DCE//wsn/$USER}; export DCE_ROOT=${DCE_ROOT//wsn/$USER}; export TUS=${TUS//wsn/$USER}; export DCE_RAW_DIR=${DCE_RAW_DIR//wsn/$USER}; export PYTHONPATH=${PYTHONPATH//wsn/$USER}; cd ~/FixedWireless; git pull; cd Tuscarora; make clean > /dev/null; ./runOrDebug.sh PI;'
echo "Servers pulled and up to date"

parallel --resume-failed --retries 3 --resume --joblog ./log --env _ --delay 5 -S wsn-02 -S : --cleanup --workdir ./FixedWireless/Tuscarora/Scripts/Bryan/trials/massrun --eta  'export DCE_DIR=${DCE_DIR//wsn/$USER}; export LD_LIBRARY_PATH=${LD_LIBRARY_PATH//wsn/$USER}; export BAKE_HOME=${BAKE_HOME//wsn/$USER}; export NS3_DCE=${NS3_DCE//wsn/$USER}; export DCE_ROOT=${DCE_ROOT//wsn/$USER}; export DCE_PATH=${DCE_PATH//wsn/$USER}; export TUS=${TUS//wsn/$USER}; export DCE_RAW_DIR=${DCE_RAW_DIR//wsn/$USER}; export PYTHONPATH=${PYTHONPATH//wsn/$USER}; ./job.sh {1} {2} {3} {4} {5} {6} {#} {7}' ::: $linkestimation ::: $discovery ::: $speeds ::: $densities ::: $beacons ::: $sizes ::: $interference
