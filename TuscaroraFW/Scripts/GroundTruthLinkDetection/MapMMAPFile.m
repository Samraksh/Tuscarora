%% Description:
%This script MapsMMAPFile
%speeds.

% Output: --
% Cells4EaBTime{N,N} cell array that stores link establish and breakage times in the upper right half


% Input:
% N:  Number of nodes nameoffile : Name of input file
% R: communication radius for links simulationEndtime = Simulation end time




%% Define Inputs
InputFileName = 'output.mmap';
HeaderSize = 32; %Header size in bytes 
SizeofEntry = 8; %Size of each entry in size(uint32)
%% Initialization
fileID = fopen(InputFileName);

%% Execution
% Read Header
% numbytesinframes    = fread(fileID, 1, 'uint32' );
% numframes   = fread(fileID, 1, 'uint32' );
% nodeID      = fread(fileID, 1, 'uint32' );
% BeaconPeriod = fread(fileID, 1, 'uint32' );
% InactPeriod = fread(fileID, 1, 'uint8' );
% CommRadius = fread(fileID, 1, 'uint16' );


%% Read Header
sizeOfStruct =  fread(fileID, 1, 'uint32' )      ; %uint32_t sizeOfStruct       ;
numRecords =  fread(fileID, 1, 'uint32' )        ; % uint32_t numRecords       ; %
nodeid =  fread(fileID, 1, 'uint32' )       ; % uint32_t nodeid       ; %
NodeID = double(nodeid) + 1;
beaconPeriod = fread(fileID, 1, 'uint32' )       ; % uint32_t beaconPeriod       ; %
%------------First 16 bytes
inactivityPeriod =  fread(fileID, 1, 'uint8', 1 )       ; % uint8_t inactivityPeriod       ; %
range =  fread(fileID, 1, 'uint16' )       ; %uint16_t range       ; %
type =  fread(fileID, 1, 'uint8', 1 )       ; %uint8_t type       ; %
numNodes =  fread(fileID, 1, 'uint16' )       ; % uint16_t numNodes       ; %
runTime =  fread(fileID, 1, 'uint16' )       ; %uint16_t runTime       ; %
density =  fread(fileID, 1, 'uint8', 1 )       ; % uint8_t density       ; %
mobilityModel =  fread(fileID, 1, 'uint8', 1 )       ; %  uint8_t mobilityModel       ; %
speed =  fread(fileID, 1, 'uint16' )       ; %uint16_t speed       ; %
%----------------------Second 16 bytes
% if (((MAX_NBR / 32) +2) > 8)
% unused  =  fread(fileID, 1, 'uint32' )       ; % uint32_t unused[MAX_NBR / 32-7]       ; %
% endif
% }       ;
fclose(fileID);
%% Map MemoryMapFile
LDMemMap = memmapfile(InputFileName        ...               ...
    ,'Format', { 'uint8'  [HeaderSize 1 ] 'Header'  ...
                ;'uint32' [SizeofEntry numRecords] 'nodesdata'     ...
               } ...
    ,'Repeat',numNodes);
