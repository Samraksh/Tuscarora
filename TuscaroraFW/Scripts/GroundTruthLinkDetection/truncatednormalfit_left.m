function [paramEsts,paramCIs] = truncatednormalfit_left(x,xTrunc_left,start)

%%Input
if nargin<3 || ~exist('start','var'), start = [mean(x),std(x)]; end
if nargin<2 || ~exist('xTrunc_left','var'), xTrunc_left = min(x); end

x = x(x > xTrunc_left);
n = numel(x);
%% Execute
fighandle = figure();
axeshandle = gca;
grid on;
hold on;
[nelements, xout] = hist(x,max(n/100,10));
bar(xout,nelements/sum(nelements)./(xout(2)-xout(1)));
pdf_truncnorm = @(x,mu,sigma) normpdf(x,mu,sigma) .* (1+normcdf(xTrunc_left,mu,sigma));



[paramEsts,paramCIs] = mle(x, 'pdf',pdf_truncnorm, 'start',start, 'lowerbound',[xTrunc_left 0],'upperbound',[Inf Inf]);

plot(xout, normpdf(xout,mean(x),std(x)),'b','DisplayName','Normal')
plot(xout, feval(pdf_truncnorm,xout,mean(x),std(x)),'g','DisplayName','Truncated Unfitted')
plot(xout, feval(pdf_truncnorm,xout,paramEsts(1),paramEsts(2)),'r','DisplayName','Truncated fit')

legend(axeshandle,'show');

