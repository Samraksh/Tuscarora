%This script reads the output of ns3 CourseChange callbacks and calculates
%the time of link establishments and link breakages between nodes
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
% Author            : Bora Karaoglu
% Last Update by    : Bora Karaoglu
% Last Update date  : 07/31/2014 12:31PM

clc;
clear all;
close all;
%% INPUT
load('SimulationInfo.mat');
%% Options
plotStateMatricies = 1; %%This many plots will be created
plotonPreviousFigure = 0;
%% Initialization
ParallelNodes = 0;
%STATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy]
Load_STATE_MATRIX_Indicies; %I_time = 1; I_position = [2,3]; I_V=[4,5];
STATE_MATRIX = NaN( N, sum([numel(I_time) numel(I_position)  numel(I_V)]) );
STATE_MATRIX(:,1) = 0;
curtime = 0;
Cells4EaBTime = cell(N,N);

c = zeros(1,N);

TempResults_el = nan(N,N); %StoreTempResults of Time instances where two nodes becomes neighbors
TempResults_bl = nan(N,N); %StoreTempResults of where two nodes seize to become neighbors

Results_el = nan(3,0); %Add results to the end of this matrix
Results_bl = nan(3,0); %Add results to the end of this matrix
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
%% Find Initial Neighbors
%BK: This is not required at this stage since I assume link is formed at
%t=0 with the first link breakage
for i = 1:N-1
    for curnode = i+1:N
        %STATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy]
        %I_time = 1; I_position = [2,3]; I_V=[4,5];
        xdif = STATE_MATRIX(i,I_position(1))-STATE_MATRIX(curnode,I_position(1));
        ydif = STATE_MATRIX(i,I_position(2))-STATE_MATRIX(curnode,I_position(2));
        dist = sqrt(xdif^2+ydif^2);
        if dist < R
            if i == 80 && curnode == 81
                a = 1;
            end
            Results_el = [Results_el [curnode;i;0]];
            Cells4EaBTime{min([curnode i]),max([curnode i])} = ...
                [Cells4EaBTime{min([curnode i]),max([curnode i])} 0];
        end
    end
end
%% Find Initial Intersection times based on the initial data
for curnode = 1:N
    for i = curnode+1:N
        if curnode ==1 && i==10
            a = 1;
        end
        %[ A, B, C ] = DistancePolynomial(STATE_MATRIX(min(i,curnode),:), STATE_MATRIX(max(i,curnode),:));
        dist2pol = DistSqPoly(STATE_MATRIX(min(i,curnode),:), STATE_MATRIX(max(i,curnode),:));
        %The polynomial for the 0 = d^2 - R^2
        polynomial_t = dist2pol - [0 0 R^2];
        D = polynomial_t(2)^2 - 4*polynomial_t(1)*polynomial_t(3);
        A = polynomial_t(1);
        
        if ( D==0 )% There will be less than 2 roots
            %Only possible when they have zero relative speed
            %No link breakages or establishing
            ParallelNodes= ParallelNodes + 1;
        elseif (D > 0)
            r = roots(polynomial_t);
            r = sort(r);
            %fileh = PlotNodeStates(STATE_MATRIX(min(i,curnode),:), STATE_MATRIX(max(i,curnode),:));
            %close all;
            if A>0 && isreal(r(1))
                if(curtime<r(1))
                    TempResults_el(curnode,i) = r(1);
                    TempResults_el(i,curnode) = r(1);
                end
                if(curtime<r(2))
                    TempResults_bl(curnode,i) = r(2);
                    TempResults_bl(i,curnode) = r(2);
                end
            elseif A<0 && isreal(r(1))
                if(curtime<r(2))
                    TempResults_el(curnode,i) = r(2);
                    TempResults_el(i,curnode) = r(2);
                end
                if(curtime<r(1))
                    TempResults_bl(curnode,i) = r(1);
                    TempResults_bl(i,curnode) = r(1);
                end
            end
        end
        % iteration_time = toc
    end
end
%% Process Movement Data line by line
linenumber = 0;
while 1
    ReadLineFromCourseChangeData;
    linenumber = linenumber + 1;
    if numel(readdata)<6 % End of the file is reached or cannot read data
        break;
    end
    curtime = readdata(1);
    %Update Node Position
    curnode = readdata(2)+1;
    %STATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy]
    STATE_MATRIX(curnode , :) = [curtime, readdata(5), readdata(6), readdata(3), readdata(4)];
    
    if curnode ==10
        a = 1;
    end
    
    %DispInformation
    if (plotStateMatricies)
        if plotonPreviousFigure
            figureh = findobj('type','figure','name','STATE_Figure');
            if isempty(findobj('type','figure','name','STATE_Figure')), figureh = figure('name','STATE_Figure');
            else clf(figureh); end
        else
            figureh = figure('name','STATE_Figure');
        end
        PlotStateMatrix(STATE_MATRIX,curtime,figureh,120,50,[1 10]);
        plotStateMatricies = plotStateMatricies-1;
        pause(2);
    end
    
    %Check if the previous neighborhood informaton is confirmed
    for i=1:N
        if i == curnode , continue; end
        if ~isnan(TempResults_el(curnode,i))
            if TempResults_el(curnode,i) < curtime
                Results_el = [Results_el [curnode;i;TempResults_el(curnode,i)]];
                Cells4EaBTime{min([curnode i]),max([curnode i])} = ...
                    [Cells4EaBTime{min([curnode i]),max([curnode i])} TempResults_el(curnode,i) ];
            end
        end
        if ~isnan(TempResults_bl(curnode,i))
            if TempResults_bl(curnode,i) < curtime
                Results_bl = [Results_bl [curnode;i;TempResults_bl(curnode,i)]];
                if numel(Cells4EaBTime{min([curnode i]),max([curnode i])})==0,  Cells4EaBTime{min([curnode i]),max([curnode i])} = 0; end
                Cells4EaBTime{min([curnode i]),max([curnode i])} = ...
                    [Cells4EaBTime{min([curnode i]),max([curnode i])} TempResults_bl(curnode,i) ];
            end
        end
    end
    % Erase previous temp results since route is changed
    TempResults_el(curnode,:) = NaN;
    TempResults_el(:,curnode) = NaN;
    TempResults_bl(curnode,:) = NaN;
    TempResults_bl(:,curnode) = NaN;
    % Calculate New temp results since
    for i = 1:N
        if i == curnode , continue; end
        %         if (curnode == 19 && i == 42) || (curnode == 42 && i == 19)
        %             a = 2
        %         end
        %[ A, B, C ] = DistancePolynomial(STATE_MATRIX(min(i,curnode),:), STATE_MATRIX(max(i,curnode),:));
        dist2pol = DistSqPoly(STATE_MATRIX(min(i,curnode),:), STATE_MATRIX(max(i,curnode),:));
        %The polynomial for the 0 = d^2 - R^2
        polynomial_t = dist2pol - [0 0 R^2];
        D = polynomial_t(2)^2 - 4*polynomial_t(1)*polynomial_t(3);
        A = polynomial_t(1);
        B = polynomial_t(2);
        if ( A == 0) % The polynomial is first order
                %Nodes have zero relative velocity
                ParallelNodes= ParallelNodes + 1;
                if polynomial_t(2) ~= 0 
                    disp('Possible problem ahead!!!');
                    error('Possible problem ahead!!!');
                end
        else
            if ( D < 0 )% The roots are not real
                %Nodes never intersect
                ParallelNodes= ParallelNodes + 1;
            else %(D >= 0)
                %r = roots(polynomial_t);
                r = ([-1*sqrt(D) sqrt(D)] - polynomial_t(2))/(2*polynomial_t(1));
                
%                 [X,Y] = CalculatePositionsAtGivenTime( STATE_MATRIX([curnode i],:), r(1) );
%                 if( sqrt( (X(2)-X(1)).^2 + (Y(2)-Y(1)).^2 ) - R ) > 0.01
%                     a = 1;
%                     disp('Possible problem ahead!!!');
%                     %r = sort(r);
%                     %fileh = PlotNodeStates(STATE_MATRIX(min(i,curnode),:), STATE_MATRIX(max(i,curnode),:));
%                     %close all;
%                 end
                if A<0, r = [r(2) r(1)]; end
                    if(curtime<r(1))
                        TempResults_el(curnode,i) = r(1);
                        TempResults_el(i,curnode) = r(1);
                    end
                    if(curtime<r(2))
                        TempResults_bl(curnode,i) = r(2);
                        TempResults_bl(i,curnode) = r(2);
                    end
            end
        end
        % iteration_time = toc
    end
end
%% Add final neighbors and link breakages
for i=1:N
    for j=i:N
        if ~isnan(TempResults_el(j,i)) && TempResults_el(j,i)< simulationEndtime
            Results_el = [Results_el [i;j;TempResults_el(i,j)]];
            Cells4EaBTime{min([j i]),max([j i])} = ...
                [Cells4EaBTime{min([j i]),max([j i])} TempResults_el(j,i) ];
            
            TempResults_el(i,j) = NaN;
            TempResults_el(j,i) = NaN;
        end
        if ~isnan(TempResults_bl(j,i)) && TempResults_bl(j,i)< simulationEndtime
            Results_bl = [Results_bl [i;j;TempResults_bl(i,j)]];
            
            if numel(Cells4EaBTime{min([j i]),max([j i])})==0,  Cells4EaBTime{min([j i]),max([j i])} = 0; end
            Cells4EaBTime{min([j i]),max([j i])} = ...
                [Cells4EaBTime{min([j i]),max([j i])} TempResults_bl(j,i) ];
            
            TempResults_bl(i,j) = NaN;
            TempResults_bl(j,i) = NaN;
        end
    end
end
%% Save Workspace
save(nameofCourseChangemat,'Cells4EaBTime','Results_el','Results_bl');

