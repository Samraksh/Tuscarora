function STATE_MATRIX = CalculateSTATE_MATRIX_BasedOnCN( varargin )
%CalculateSTATE_MATRIX_BasedOnCN This function calculates the
% STATE_MATRIX at a desired time
%  CalculateSTATE_MATRIX_BasedOnCN( siminfofile, desiredtime  )
% Input 
%    siminfofile: This is the file that stores defining the simulation
%                   parameters
%    desiredtime : Desired network status time in seconds
% 
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
%% Input
if nargin >= 1, siminfofile = varargin{1}; else siminfofile = 'SimulationInfo.mat'; end
if nargin >= 2, desiredtime = varargin{2}; else desiredtime = 0; end
%% Initialization
load(siminfofile);
Load_STATE_MATRIX_Indicies;
STATE_MATRIX = nan( N, sum([numel(I_time) numel(I_position)  numel(I_V)]) );
%% Execution
linenumber = 0;
while 1 % Update the STATE_MATRIX up until the desiredtime 
    ReadLineFromCourseChangeData;
    linenumber = linenumber + 1;
    if numel(readdata)<6 % End of the file is reached or cannot read data
        break;
    end
    nextroutechangetime = readdata(1);
    if nextroutechangetime > desiredtime % Desired Time Has been reached
        break;
    end
    if 1 %Update the STATE_MATRIX based on the input data
        %Update Node Position
        curnode = readdata(2)+1;
        %STATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy]
        STATE_MATRIX(curnode , :) = [nextroutechangetime, readdata(5), readdata(6), readdata(3), readdata(4)];
    end   
end
% [X,Y] = CalculatePositionsAtGivenTime( STATE_MATRIX, desiredtime );
% STATE_MATRIX(:,I_position) = [X,Y];
end

