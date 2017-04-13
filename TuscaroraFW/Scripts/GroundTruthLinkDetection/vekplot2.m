function [pilx,pily]=vekplot2(x,y,u,v,scale,line,mark,fillcolor)
% [pilx,pily]=vekplot2(x,y,u,v,scale,line,mark,fillcolor)
% Function essentially equivalent to quiver, but
% this function does NOT preform automatic scaling of vectors.
% SCALE: lentgh in figure when u^2+v^2=1
% PILX,PILY: plot(pilx.pily) will return a vector plot.

% Rewritten by Per-Olav Rusaas, peolav@math.uio.no, 1997

x=x(:).'; y=y(:).'; u=u(:).'; v=v(:).';

alfa=pi/6; % half the "tip-angle"

x1=x;
x2=x+u*scale;
y1=y;
y2=y+v*scale;
r=sqrt((x2-x1).^2 + (y2-y1).^2);

retcos=[cos(alfa), -sin(alfa) ; sin(alfa), cos(alfa)];

spisslengde=min(0.5*ones(size(r)), 0.25*r);
spisslengde(2,:)=spisslengde(1,:);

lvek1=retcos*[(x1-x2) ; (y1-y2)];
lvek2=retcos'*[(x1-x2) ; (y1-y2)];
lengde=sqrt(lvek1(1,:).^2+lvek1(2,:).^2);
lengde=max(lengde,ones(size(lengde))*1.0e-200);
lengde(2,:)=lengde(1,:);
lvek1=lvek1./lengde .* spisslengde;
lvek2=lvek2./lengde .* spisslengde;

pilx=[x1; x2; x2+lvek1(1,:); x2; x2+lvek2(1,:)];
pily=[y1; y2; y2+lvek1(2,:); y2; y2+lvek2(2,:)];



