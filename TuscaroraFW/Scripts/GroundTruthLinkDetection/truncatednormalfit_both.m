function [paramEsts,paramCIs] = truncatednormalfit_both(x,xTruncarray,start)

%%Input
if nargin<3 || ~exist('start',var), start = [mean(x),std(x)]; end

xTrunc_left = xTruncarray(1);
xTrunc_right = xTruncarray(2);

x = x(x > xTrunc_left);
x = x(x< xTrunc_right);
n = numel(x);

pdf_truncnorm = @(x,mu,sigma) normpdf(x,mu,sigma) .* (1+normcdf(xTrunc_left,mu,sigma)) .* (1+(1-normcdf(xTrunc_right,mu,sigma)));

%% Execute
fighandle = figure();
axeshandle = gca;
grid on;
hold on;
[nelements, xout] = hist(x,max(n/100,10));
bar(xout,nelements/sum(nelements)./(xout(2)-xout(1)));





[paramEsts,paramCIs] = mle(x, 'pdf',pdf_truncnorm, 'start',start, 'lowerbound',[xTrunc_left 0],'upperbound',[xTrunc_right Inf]);

plot(xout, normpdf(xout,mean(x),std(x)),'b','DisplayName','Normal')
plot(xout, feval(pdf_truncnorm,xout,mean(x),std(x)),'g','DisplayName','Truncated Unfitted')
plot(xout, feval(pdf_truncnorm,xout,paramEsts(1),paramEsts(2)),'r','DisplayName','Truncated fit')

legend(axeshandle,'show');

