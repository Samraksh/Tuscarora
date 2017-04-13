%This script test the output of Neighborhood Calculator
%
% Input:
% N:  Number of nodes
% nameofCourseChangeDataFile : Name of input file
% R: communication radius for links
% simulationEndtime = Simulation end time
%
% Output:
% Cells4EaBTime{N,N} cell array that stores link establish and breakage times in the upper right half
% Results_el(3,:) Stores time of link establishments in the [node1;node2;time] format
% Results_bl(3,:) Stores time of link breakegaes in the [node1;node2;time] format
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
clc;
clear all;
close all;
%%
clear lastreadlinenumber
clear fileh
%% INPUT
load('SimulationInfo.mat');
load(nameofCourseChangemat, 'Results_el', 'Results_bl');
%% Options
plotStateMatricies = 0; %%This many plots will be created
plotonPreviousFigure = 0;
%% Initialization
ParallelNodes = 0;
%STATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy]
Load_STATE_MATRIX_Indicies; %I_time = 1; I_position = [2,3]; I_V=[4,5];
STATE_MATRIX = NaN(N,5);
STATE_MATRIX(:,1) = 0;
curtime = 0;

statistics_el = [];
statistics_bl = [];
%% Read Initial Node Locations
for i=1:N
    %readdata = ReadLineFromCourseChangeDataInText( fileh );
    ReadLineFromCourseChangeData;
    if numel(readdata)<6, break; end
    if readdata(1)>0, error('Ut>0 in the initialization part'); end
    curnode = readdata(2)+1;
    STATE_MATRIX(curnode , :) = [0, readdata(5), readdata(6), readdata(3), readdata(4)];
end
if (plotStateMatricies)
    figureh = findobj('type','figure','name','STATE_Figure');
    close(figureh)
    if isempty(findobj('type','figure','name','STATE_Figure')), figureh = figure('name','STATE_Figure');
    else clf(figureh); end
    PlotStateMatrix(STATE_MATRIX,0,figureh);
    plotStateMatricies = plotStateMatricies-1;
    pause(2);
end
%% Process Results_el
rind_el = 1;
while Results_el(3,rind_el) == 0
    rind_el = rind_el +1;
end
rind_bl = 1;
while Results_el(3,rind_bl) == 0
    rind_bl = rind_bl +1;
end
linenumber = 0;
while 1
    ReadLineFromCourseChangeData;
    linenumber = linenumber + 1;
    if numel(readdata)<6 % End of the file is reached or cannot read data
        break;
    end
    nextroutechangetime = readdata(1);
    % Results_el
    while rind_el < size(Results_el,2) && Results_el(3,rind_el)<nextroutechangetime
        desiredtime = Results_el(3,rind_el);  
        
        if (plotStateMatricies) %DispInformation
            if plotonPreviousFigure
                figureh = findobj('type','figure','name','STATE_Figure');
                if isempty(findobj('type','figure','name','STATE_Figure')), figureh = figure('name','STATE_Figure');
                else clf(figureh); end
            else
                figureh = figure('name','STATE_Figure');
            end
            PlotStateMatrix(STATE_MATRIX,curtime,figureh);
            plotStateMatricies = plotStateMatricies-1;
            pause(2);
        end
        
        [X,Y] = CalculatePositionsAtGivenTime( STATE_MATRIX([Results_el(1,rind_el),Results_el(2,rind_el)],:), desiredtime );
        statistics_el(end+1) = sqrt( (X(2)-X(1)).^2 + (Y(2)-Y(1)).^2 ) - R;
        if(statistics_el(end)>0.1)
            disp('Warning large value in statistics_el');
            a = 1;
        end
        rind_el = rind_el+1;
    end
    % Results_bl
    while rind_bl < size(Results_bl,2) && Results_bl(3,rind_bl)<nextroutechangetime
        desiredtime = Results_bl(3,rind_bl);
        %DispInformation
        if (plotStateMatricies)
            if plotonPreviousFigure
                figureh = findobj('type','figure','name','STATE_Figure');
                if isempty(findobj('type','figure','name','STATE_Figure')), figureh = figure('name','STATE_Figure');
                else clf(figureh); end
            else
                figureh = figure('name','STATE_Figure');
            end
            PlotStateMatrix(STATE_MATRIX,curtime,figureh);
            plotStateMatricies = plotStateMatricies-1;
            pause(2);
        end
        [X,Y] = CalculatePositionsAtGivenTime( STATE_MATRIX([Results_bl(1,rind_bl),Results_bl(2,rind_bl)],:), desiredtime );
        
        statistics_bl(end+1) = sqrt( (X(2)-X(1)).^2 + (Y(2)-Y(1)).^2 ) - R;
        if(statistics_bl(end)>0.1)
            disp('Warning large value in statistics_bl');
            a = 1;
        end
        rind_bl = rind_bl+1;
    end
    
    %Update the STATE_MATRIX based on the input data
    curtime = readdata(1);
    %Update Node Position
    curnode = readdata(2)+1;
    %STATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy]
    STATE_MATRIX(curnode , :) = [nextroutechangetime, readdata(5), readdata(6), readdata(3), readdata(4)];
end
%% Calculate statistics
mean_el = mean(abs(statistics_el)) ;
median_el = median(abs(statistics_el)) ;
max_statistic_el = max(abs(statistics_el)) ;

mean_bl = mean(abs(statistics_bl)) ;
median_bl = median(abs(statistics_bl)) ;
max_statistic_bl = max(abs(statistics_bl)) ;
%% Save Workspace
%save('testNeighborhoodCalculator.mat');


