
n = 10000;
mu = 10;
xTrunc = 6;
sigma = 3;
x = normrnd(mu,sigma,[1 n]);


x = x(x > xTrunc);
length(x)

fighandle = figure();
axeshandle = gca;
grid on;
hold on;
[nelements xout] = hist(x,n/100);
bar(xout,nelements/sum(nelements)./(xout(2)-xout(1)));
pdf_truncnorm = @(x,mu,sigma) normpdf(x,mu,sigma) .* (1+normcdf(xTrunc,mu,sigma));


start = [mean(x),std(x)];
[paramEsts,paramCIs] = mle(x, 'pdf',pdf_truncnorm, 'start',start, 'lowerbound',[xTrunc 0],'upperbound',[Inf Inf])

plot(xout, feval(pdf_truncnorm,xout,mean(x),std(x)),'g','DisplayName','Unfitted')
plot(xout, feval(pdf_truncnorm,xout,paramEsts(1),paramEsts(2)),'r','DisplayName','fit')

legend(axeshandle,'show');

