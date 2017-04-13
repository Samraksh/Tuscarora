%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
N = 100; %Number of nodes in the network
IsInputDataInTextFormat = 0; % 1 if the input data is in text format 0 if the input data is in MATLAB workspace format
nameofCourseChangeDataFile='CourseChangeData.mat';
R= 120; % Neighborhood distance
simulationEndtime = 120;

maxx = 750;
maxy = 750;
speedmax = 10;
speedmin = 0;

nameofAllNodesDatafile = 'allnodes.txt';
T = 0.01;
TotalTime = simulationEndtime;



nameofCourseChangemat = 'linksCourseChangeNotifier.mat'; % Mat file storing the course establishment and breakage times
nameofthememorymapmat = 'BryanDataRead.mat'; % Name mat file
nameofthememorymapmatintermediate = 'BryanDataRead_intermediate.mat';



save('SimulationInfo.mat');