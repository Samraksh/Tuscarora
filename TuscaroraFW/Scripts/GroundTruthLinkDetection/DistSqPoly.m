function dist2pol = Dist2Poly(State1, State2)
%timeInstancesofNeighborhood: This function finds the time instances where
%the distances between two nodes with initial State arrays of State1 and
%State2. State = [UpdateTime, PositionX, PositionY, Vx, Vy]
%I_time = 1; I_position = [2,3]; I_V=[4,5];  
%
%
% Author            : Bora Karaoglu, bora.karaoglu@samraksh.com, 2014

Load_STATE_MATRIX_Indicies;


%Check Input
if nnz(isnan(State1)) || nnz(isnan(State2)), r =[]; end


%%%%% Beginning of function
V1x = State1(I_V(1));
V1y = State1(I_V(2));
Ut1 = State1(I_time(1));
x1 = State1(I_position(1));
y1 = State1(I_position(2));


V2x = State2(I_V(1));
V2y = State2(I_V(2));
Ut2 = State2(I_time(1));
x2 = State2(I_position(1));
y2 = State2(I_position(2));



pos1xpol = [ V1x x1-Ut1*V1x ];
pos1ypol = [ V1y y1-Ut1*V1y ];
%plot(pos1xpol(1).*t+pos1xpol(2),pos1ypol(1).*t+pos1ypol(2),'color','g')

pos2xpol = [ V2x x2-Ut2*V2x ];
pos2ypol = [ V2y y2-Ut2*V2y ];
%plot(pos2xpol(1).*t+pos2xpol(2),pos2ypol(1).*t+pos2ypol(2),'color','m')


posdifxpol = (pos2xpol - pos1xpol);
posdifypol = (pos2ypol - pos1ypol);

dist2pol = conv(posdifxpol,posdifxpol)+conv(posdifypol,posdifypol);

% A = (posdifxpol(1))^2 + (posdifypol(1))^2;
% B = (2*prod(posdifxpol) + 2*prod(posdifypol));
% C = (posdifxpol(2))^2 + (posdifypol(2))^2 ;
% %% eND OF FUNCTION
end

