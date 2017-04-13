function [ ] = PlotStateMatrix( STATE_MATRIX, desiredtime, fh, R, extraspace,emphasizednodelist )
%PlotStateMatrix( STATE_MATRIX, desiredtime, fh )
% THIS function plots the node locations at the desired time according to
% the STATE_MATRIX
% 
% INPUT:
% STATE_MATRIX Nx6 : [LastUpdateTime, Positionx,Positiony, Vx, Vy]
% desired_time: time of desired output
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014

%% Initialize
if nargin<3 || isempty(fh), fh = figure('name','STATE_Figure'); end
if nargin<4 || isempty(R), R = 120; end
if nargin<5 || ~exist('extraspace','var') || isempty(extraspace), extraspace = 50; end
if nargin<6 || ~exist('emphasizednodelist','var') || isempty(extraspace), emphasizednodelist = [1 10]; end
rectanglesize = 1000;


figure(fh);
axes_handle = gca;

xlabel('x-position');
ylabel('y-position');
title(['t = ' num2str(desiredtime)]);
axis square;
grid on;

Load_STATE_MATRIX_Indicies;
%% execute

% ElapsedTimes =  repmat(desiredtime,[size(STATE_MATRIX,I_time),1]) -  STATE_MATRIX(:,I_time) ;
%     X =   STATE_MATRIX(:,I_position(1)) ...
%         + STATE_MATRIX(:,I_V(1)) .* ElapsedTimes ; 
%     Y =   STATE_MATRIX(:,I_position(2)) ...
%         + STATE_MATRIX(:,I_V(2)) .* ElapsedTimes;
    [X,Y] = CalculatePositionsAtGivenTime( STATE_MATRIX, desiredtime );

     hold on;
    %circle(X, Y, repmat(120, size(X)), fh);
    colors_used = hsv(numel(X));
    scatter(X, Y, 20, colors_used);
    %scatter(X(1), Y(1), R^2);
    text(X,Y, num2str((0:(numel(X)-1))') );
    
    %quiver( X, Y, STATE_MATRIX(:,I_V(1)), STATE_MATRIX(:,I_V(2)), 0.5 );
    %colquiver(quiver( X, Y, STATE_MATRIX(:,I_V(1)), STATE_MATRIX(:,I_V(2))), colors_used)
    [pilx, pily] = vekplot2( X, Y, STATE_MATRIX(:,I_V(1)), STATE_MATRIX(:,I_V(2)), 10);
    for i=1:numel(X)
        plot(pilx(:,i),pily(:,i),'color',colors_used(i,:));
    end
    
    for i = emphasizednodelist
        circle(X(i),Y(i),repmat(R,size(X(i))),axes_handle, colors_used(i,:) ) ;
    end

    axis([0-extraspace rectanglesize+extraspace 0-extraspace rectanglesize+extraspace])
rectangle('Position',[0 0 rectanglesize rectanglesize],...
         'LineWidth',2,'LineStyle','--');
end

