function CourseChangeDataMatrix = GenerateRandomWayPointModel(N, maxx, maxy, speedmax, speedmin, simulationEndtime)
%This function generates the course change notifier file inputs following
%a random waypoint model with zero pause duration
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
%% Inputs: 
% N:  Number of nodes
% maxx :
% maxy :
% speedmaxx :
% speedmin :
%% Outputs
% CourseChangeDataMatrix: (Lx6) : [UpdateTime, NodeID, Vx, Vy, Positionx,Positiony]

% GENSTATE_MATRIX Nx6 [LastUpdateTime, Positionx,Positiony, Vx, Vy, Duration]
%% Initialization
%GENSTATE_MATRIX = nan(N,6);
speeds = speedmin+(speedmax-speedmin).*rand(N,1);
init_locs = [maxx*rand(N,1) maxy*rand(N,1)];
dest_locs = [maxx*rand(N,1) maxy*rand(N,1)];
diff_locs = dest_locs - init_locs;
indicies = diff_locs(:,1)~=0;
angles = nan(N,1);
angles(indicies) = atan(diff_locs(indicies,2)./diff_locs(indicies,1));
angles(~indicies) = (diff_locs(~indicies,2)>0)*pi - pi/2;
durations = sqrt(sum( ((dest_locs - init_locs).^2) ,2 ))./speeds; 

GENSTATE_MATRIX =     [zeros(N,1) maxx*rand(N,1) maxy*rand(N,1) speeds.*cos(angles) speeds.*sin(angles) durations];
CourseChangeDataMatrix = [GENSTATE_MATRIX(:,1) (1:N)' GENSTATE_MATRIX(:,4) GENSTATE_MATRIX(:,5) GENSTATE_MATRIX(:,2) GENSTATE_MATRIX(:,3)];
%% Process Movement
%Find next time update
curtime = 0;
while curtime < simulationEndtime
    [curtime,I] = min(GENSTATE_MATRIX(:,1)+GENSTATE_MATRIX(:,6));
    speed = speedmin+(speedmax-speedmin).*rand(1,1);
    init_loc = [ GENSTATE_MATRIX(I,2) + GENSTATE_MATRIX(I,6)*GENSTATE_MATRIX(I,4) ...
                  GENSTATE_MATRIX(I,3) + GENSTATE_MATRIX(I,6)*GENSTATE_MATRIX(I,5) ...
                ];
    dest_loc = [ maxx*rand(1,1) maxy*rand(1,1)];
    diff_loc = dest_loc - init_loc;
    if diff_loc(1) == 0, angle = (diff_locs(1)>0)*pi - pi/2;
    else angle = atan(diff_loc(2)./diff_loc(1));
    end
    duration = sqrt(sum( ((dest_loc - init_loc).^2)  ))./speed;
    GENSTATE_MATRIX(I,:)= [ curtime init_loc(1) init_loc(2) speed*cos(angle) speed*sin(angle) duration];
    CourseChangeDataMatrix = [CourseChangeDataMatrix; ...
        [GENSTATE_MATRIX(I,1) I GENSTATE_MATRIX(I,4) GENSTATE_MATRIX(I,5) GENSTATE_MATRIX(I,2) GENSTATE_MATRIX(I,3)] ];
end
save('GeneratedMobility.mat', 'CourseChangeDataMatrix');
end