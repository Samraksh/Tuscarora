%This script calculates the link durations based on CN
% Output:
% LinkDuration: (3XL) [Node i; Node j; Link Duration];

%% Input
clear all;
clc;
close all;
load('SimulationInfo.mat');
t_init = 10;
threshold = 10; % the maximum difference between the LD reported times and the CN reported times



%% Initialization
S = load(nameofCourseChangemat, 'Cells4EaBTime');
CN_Cells4EaBTime = S.Cells4EaBTime; %Link Establishments and Breakages course change notifier data

LinkReestablishmentDelay =[];
%% Execute
for i = 1:N
    for j = i:N
        CN_EaBTime = CN_Cells4EaBTime{min(i,j),max(i,j)};
        m2add = diff(CN_EaBTime);
        m2add = m2add(2:2:end);
        LinkReestablishmentDelay = [LinkReestablishmentDelay [repmat(i,size(m2add));repmat(j,size(m2add));m2add]];
    end
end

%% Save Workspace
save('LinkReestablishmentDelay','LinkReestablishmentDelay');

%% Plot node ID vs. LinkDuration
figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'LinkReestablishmentDelayvsNodeIDs';

y1 = LinkReestablishmentDelay(1,:);
y2 = LinkReestablishmentDelay(2,:);
x = LinkReestablishmentDelay(3,:);

scatter(x,y1,5,'MarkerEdgeColor', [1 0 0], 'MarkerFaceColor','auto', 'DisplayName', 'LinkDuration');
scatter(x,y2,10,'MarkerEdgeColor', [1 0 0], 'MarkerFaceColor','auto', 'DisplayName', 'LinkDuration');

xlabel('time');
ylabel('Node ID');
%legend(axeshandle,'show');
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

%% LinkDuration 
rawdata = LinkReestablishmentDelay(3,:);
nameofrawdata = 'LinkReestablishmentDelay';
figh = figure();
axeshandle = gca;
hist(axeshandle,rawdata,20 );
nameoffig = ['hist__' nameofrawdata];
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

figh = figure();
axeshandle = gca;
[curvehandle,stats] = cdfplot(rawdata);
nameoffig = ['cdf__' nameofrawdata];
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

truncatednormalfit_left(rawdata,min(rawdata));


