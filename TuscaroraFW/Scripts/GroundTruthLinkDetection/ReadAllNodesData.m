%% Description:
%This script reads the output of snapshot methods of  node position  and calculates
%the time of link establishments and link breakages between nodes.
% The output.mmap file should be converted to the ASCII format before being
% read into this. MPP script can do this. 
% Usage: "MPP path/to/mmap/file allnodes > mmaap_text_file"

% Output: 
% LD_Cells4EaBTime{N,N} cell array that stores link establish and breakage times in the upper right half 
% LD_Results_el(3,:) Stores time of link establishments in the [node1;node2;time] format
% LD_Results_bl(3,:) Stores time of link breakegaes in the [node1;node2;time] format
% LocationDatas(2,N,NumberofEntriesforEachNode)
% 
% Input: 
% N:  Number of nodes nameoffile 
% nameofAllNodesDatafile: Name of input file
% T: TimeIntervalbetween reads
%
%
% Author            : Bora Karaoglu
% Last Update by    : Bora Karaoglu
% Last Update date  : 07/31/2014 12:31PM

%% Define Inputs
clear all;
clc;
close all;
%SimulationInfo
load('SimulationInfo.mat');

%% Initialization

LocationDatas = nan(2,N,TotalTime/T) ; % N*2


LD_Cells4EaBTime = cell(N,N);
%fileID = fopen(nameofAllNodesDatafile);
prev_node = 1;
prev_neighborlist = [];
neighborlist = [];
endoffile = false;
cur_time = 0;
tindex = 0;

LD_Results_el = nan(3,0); %Add results to the end of this matrix
LD_Results_bl = nan(3,0); %Add results to the end of this matrix

fileID = fopen(nameofAllNodesDatafile);
fseek(fileID, 0,'bof');



%% Execution

% Read and procell file
linenumber = 0;
ISCOMPLETED = 0;
while ~endoffile
    if mod(linenumber,1000) == 0
        linenumber
        save(nameofthememorymapmatintermediate, 'LD_Cells4EaBTime', 'LD_Results_el', 'LD_Results_bl', 'linenumber', 'ISCOMPLETED');
    end
% Read Data    
    linenumber = linenumber + 1;
    tline = fgetl(fileID);
    if ~ischar(tline), endoffile = true; 
    else
        C = strsplit(tline, ', ')  ;
        cur_node = str2double(C{1})+1;
        xpos = str2double(C{2});
        ypos = str2double(C{3});
        neighborlist = str2double(strsplit(C{4}));
        neighborlist = neighborlist(1:end-1) + 1;
        cur_time = cur_time + T;
        tindex = tindex + 1;
        if ~isequal(prev_node, cur_node) 
            prev_node = cur_node; 
            cur_time = T;
            tindex = 1;
            prev_neighborlist = [];
        end
        
        if cur_node == 10
            a = 1
        end
   % Record location data
LocationDatas (1,cur_node,tindex) = xpos;
LocationDatas (2,cur_node,tindex) = ypos; 
% Find Neighbors
        if ~isequal(prev_neighborlist , neighborlist)
            lostneighbors = setdiff(prev_neighborlist,neighborlist);
            gainedneighbors = setdiff(neighborlist,prev_neighborlist);
            for i = 1:numel(gainedneighbors)
                LD_Cells4EaBTime{cur_node, gainedneighbors(i)} = [LD_Cells4EaBTime{cur_node, gainedneighbors(i)} cur_time];
                LD_Results_el = [LD_Results_el [cur_node;gainedneighbors(i);cur_time]];
            end
            for i = 1:numel(lostneighbors)
                if isempty( LD_Cells4EaBTime{cur_node, lostneighbors(i)}),  LD_Cells4EaBTime{cur_node, lostneighbors(i)} = 0; end
                LD_Cells4EaBTime{cur_node, lostneighbors(i)} = [  LD_Cells4EaBTime{cur_node, lostneighbors(i)} cur_time ];
                LD_Results_bl =  [LD_Results_bl [cur_node;lostneighbors(i);cur_time]];
            end
            prev_neighborlist = neighborlist;
        end
    end
end
ISCOMPLETED = 1;
%save(nameofthememorymapmat, 'LocationDatas', 'LD_Cells4EaBTime', 'LD_Results_el', 'LD_Results_bl', 'linenumber', 'ISCOMPLETED');
CalculateLinksFromLocationDatas







