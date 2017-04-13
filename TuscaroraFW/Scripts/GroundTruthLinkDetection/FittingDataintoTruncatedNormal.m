%% Definition
%% Inputs
%rawdata;
rawdata = LinkDuration(3,:);

xTrunc = 0 ;
%% Left Truncation
rawdata = normrnd(2,1,[1 1000]);
rawdata = rawdata(rawdata>xTrunc);

figure();
hist(rawdata)

pdf_truncnorm = @(x,mu,sigma) normpdf(x,mu,sigma) ./ normcdf(xTrunc,mu,sigma);
start = [mean(rawdata),std(rawdata)];

[paramEsts,paramCIs] = mle(rawdata, 'pdf',pdf_truncnorm, 'start',start , 'lower', [-500 0.001]);
