%% Description:
% This script calculates the time of link establishment and breakages based
% on the matrix LocationDatas
%
% Input: 
% LocationDatas(2,N,NumberofEntriesforEachNode)
% R : Communication Radius
% 
% Output: 
% NL_Cells4EaBTime{N,N} cell array that stores link establish and breakage times in the upper right half 
% NL_Results_el(3,:) Stores time of link establishments in the [node1;node2;time] format
% NL_Results_bl(3,:) Stores time of link breakegaes in the [node1;node2;time] format

% Major Intermediate Variables: 
% Neighbors_Matrix(NxN): Binary matrix representing neighborhood info in
% the upper half of the matrix
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
%% Initialize
clear all
clc
close all
SimulationInfo
Neighbors_Matrix = zeros(N,N);

NL_Cells4EaBTime = cell(N,N); %% Concotanate time into the cells of this cell matrix
NL_Results_el = nan(3,0); %Add results to the end of this matrix
NL_Results_bl = nan(3,0); %Add results to the end of this matrix
load(nameofthememorymapmat,'LocationDatas');
%% Execute
for t = 1:size(LocationDatas,3)
    curtime = t *T;
    xdifmat = repmat(LocationDatas(1,:,t),[N 1]) - repmat((LocationDatas(1,:,t))',[1 N]);
    ydifmat = repmat(LocationDatas(2,:,t),[N 1]) - repmat((LocationDatas(2,:,t))',[1 N]);
    distmat = sqrt ( xdifmat.^2 + ydifmat.^2 );
    prevNeighbors_Matrix = Neighbors_Matrix;
    Neighbors_Matrix = (distmat < R);
    Neighbordiff_Matrix = Neighbors_Matrix - prevNeighbors_Matrix;
    [r,c] = find(( Neighbors_Matrix - prevNeighbors_Matrix ) == 1 );
    % Add link establishments
    NL_Results_el = [NL_Results_el [r' ; c'; repmat(curtime,[1 numel(r)])]] ;
    % Add link breakages
    [r,c] = find(( Neighbors_Matrix - prevNeighbors_Matrix ) == -1 );
    NL_Results_bl = [NL_Results_bl [r' ; c'; repmat(curtime,[1 numel(r)])]] ;
    
    % Add to NL_Cells4EaBTime 
    ind = find(( Neighbors_Matrix - prevNeighbors_Matrix ) ~= 0 );
    for i = 1:numel(ind)
        NL_Cells4EaBTime{ind(i)} = [NL_Cells4EaBTime{ind(i)} curtime];
    end
    %This does not work bit I will work on this wirthout the loop
%     ind = find(( Neighbors_Matrix - prevNeighbors_Matrix ) ~= 0 );
%         NL_Cells4EaBTime{ind} = {NL_Cells4EaBTime{ind} {curtime}}; 
    curtime
%     if nnz (ind==80)
%         a = 2;
%         emphasizednodelist = [1 80]
%         
%         fig_handle = figure;
%         axes_handle = gca;
%         title(['t = ' num2str(t *T)]);
%         xlabel('x-position');
%         ylabel('y-position');
%         axis square;
%         grid on;
%         hold on;
%         X = LocationDatas(1,:,t);
%         Y = LocationDatas(2,:,t);
%         scatter(axes_handle,X, Y, pi*(R/100)^2, 0:(numel(X)-1));
%         circle(X(emphasizednodelist),Y(emphasizednodelist),repmat(R,size(X(emphasizednodelist))),axes_handle) ;
%         text(X,Y, num2str((0:(numel(X)-1))') );
%         hold off;
%     end
end

save(nameofthememorymapmat,'NL_Cells4EaBTime','-append');