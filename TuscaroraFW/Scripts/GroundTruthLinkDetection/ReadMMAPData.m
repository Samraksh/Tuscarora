%% Description:
%This script reads the output of snapshot methods of  node position  and calculates
%the time of link establishments and link breakages between nodes.
% 
% Output: 
% LD_Cells4EaBTime{N,N} cell array that stores link establish and breakage times in the upper right half 
% LocationDatas(2,N,NumberofEntriesforEachNode)
% 
% Input: 
% N:  Number of nodes nameoffile 
% nameofAllNodesDatafile: Name of input file
% T: TimeIntervalbetween reads
%
% Author            : Bora Karaoglu
% Last Update by    : Bora Karaoglu
% Last Update date  : 08/14/2014 12:31PM
%% Define Inputs
clear all;
clc;
close all;
%SimulationInfo
load('SimulationInfo.mat');
MapMMAPFile; %MAP MMAP file
T = 0.01; %Snapshot period
%% Initialization
LD_Cells4EaBTime = cell(N,N);
LocationDatas = nan(2,N,TotalTime/T) ; % N*2

%% Execution
tic
for cur_node = 1 : numNodes
    disp(['Processing node:' num2str(cur_node)]);       
% Record location data
    LocationDatas (1,cur_node,:) = double(LDMemMap.Data(cur_node).nodesdata(1,:)) /10000;
    LocationDatas (2,cur_node,:) = double(LDMemMap.Data(cur_node).nodesdata(2,:)) /10000; 
% Record neighbor data
    neighbormaps = (reshape( (de2bi(LDMemMap.Data(cur_node).nodesdata(3:end,:),32))',(SizeofEntry-2)*32,[]))';
    [timeinstanceindexes,neigbourMIDs] = find(neighbormaps);
    uneigbourMIDs = unique(neigbourMIDs);
    for neigh = uneigbourMIDs'
        tis = (timeinstanceindexes(neigbourMIDs == neigh))';
        if (numel(tis)==1) 
             tind2add = [];
        elseif(numel(tis)==1)
            tind2add = tis(1);
        elseif numel(tis)>=2
            tchanges = diff(tis);
            t2 = find(tchanges~=1);            
            if tis(end)~= size(neighbormaps,1) ;% or number of records
                tind2add = [tis(1) tis(t2) tis(end)];
            else
                tind2add = [tis(1) tis(t2)];
            end
        end
        LD_Cells4EaBTime{cur_node, neigh} = tind2add.*T;
    end
end
processingtime = toc;
disp(['Total Processing Time is ' num2str(processingtime) 'seconds']);
ISCOMPLETED = 1;
%save(nameofthememorymapmat, 'LocationDatas', 'LD_Cells4EaBTime', 'LD_Results_el', 'LD_Results_bl', 'linenumber', 'ISCOMPLETED');
save(nameofthememorymapmat);
%CalculateLinksFromLocationDatas







