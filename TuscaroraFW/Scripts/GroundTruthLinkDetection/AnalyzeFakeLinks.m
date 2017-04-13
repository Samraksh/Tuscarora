%% Input
clear all;
clc;
close all;
load('CourseChangevsMemMap.mat'); 

%% Check existance of LETs
if numel(fake_LET)==0, disp('No fake LETs'); 
else disp('fake LETs');  end

%% Check existance of  LBTs
if numel(fake_LBT)==0, disp('No fake LBTs exists'); 
else disp('fake LBTs exists');  end

%% Plot node ID vs. fake LET and LBTs
figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'LETs';

y1 = fake_LET(1,:);
y2 = fake_LET(2,:);
x = fake_LET(3,:);

y1_2 = fake_LBT(1,:);
y2_2 = fake_LBT(2,:);
x_2 = fake_LBT(3,:);

scatter(x,y1,5,'MarkerEdgeColor', [1 0 0], 'MarkerFaceColor','auto', 'DisplayName', 'LETAnnouncingNode');
scatter(x,y2,10,'MarkerEdgeColor', [0 1 0], 'MarkerFaceColor','auto', 'DisplayName', 'LETAnnouncedNode');


xlabel('time');
ylabel('Node ID');
legend(axeshandle,'show');
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');


figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'LBTs';
scatter(x_2,y1_2,15,'MarkerEdgeColor', [0 0 1], 'MarkerFaceColor','auto', 'DisplayName', 'LBTAnnouncingNode');
scatter(x_2,y2_2,20,'MarkerEdgeColor', [1 0 1], 'MarkerFaceColor','auto', 'DisplayName', 'LBTAnnouncedNode');
xlabel('time');
ylabel('Node ID');
legend(axeshandle,'show');
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'LETandLBTs';
scatter(x_2,y1_2,40,'MarkerEdgeColor', [0 0 1], 'MarkerFaceColor','auto', 'DisplayName', 'LBTAnnouncingNode');
scatter(x_2,y2_2,40,'MarkerEdgeColor', [1 0 1], 'MarkerFaceColor','auto', 'DisplayName', 'LBTAnnouncedNode');
scatter(x,y1,5,'MarkerEdgeColor', [1 0 0], 'MarkerFaceColor','auto', 'DisplayName', 'LETAnnouncingNode');
scatter(x,y2,5,'MarkerEdgeColor', [0 1 0], 'MarkerFaceColor','auto', 'DisplayName', 'LETAnnouncedNode');
xlabel('time');
ylabel('Node ID');
legend(axeshandle,'show');
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

%% Num LETs
figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'numLETs';

binsize = 0.01;

y1 = fake_LET(1,:);
y2 = fake_LET(2,:);
x = fake_LET(3,:);

bincenters = 0:binsize:max(x);

[nelements,centers] = hist(x,bincenters);
scatter(bincenters,nelements,5,'MarkerEdgeColor', [1 0 0], 'MarkerFaceColor','auto', 'DisplayName', 'Total False Links');

xlabel('time');
ylabel('Number of Nodes');
legend(axeshandle,'show');
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

%% Fake Duration
rawdata = fake_duration(3,:);
nameofrawdata = 'Duration__of__Fake__Link_Breakages';
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

