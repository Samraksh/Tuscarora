function [X Y] = CalculatePositionsAtGivenTime( STATE_MATRIX, desiredtime )
%CalculateStateMatrixAtGivenTime This function Calculates the positions of
%the nodes at a later time based on the STATE_MATRIX
%  % INPUT:
% STATE_MATRIX Nx6 : [LastUpdateTime, Positionx,Positiony, Vx, Vy]
% desired_time: time of desired output
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014
Load_STATE_MATRIX_Indicies;

ElapsedTimes =  repmat(desiredtime,[size(STATE_MATRIX,I_time),1]) -  STATE_MATRIX(:,I_time) ;
    X =   STATE_MATRIX(:,I_position(1)) ...
        + STATE_MATRIX(:,I_V(1)) .* ElapsedTimes ; 
    Y =   STATE_MATRIX(:,I_position(2)) ...
        + STATE_MATRIX(:,I_V(2)) .* ElapsedTimes;

end

