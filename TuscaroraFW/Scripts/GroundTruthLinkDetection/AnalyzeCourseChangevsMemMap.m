%This script plots figures related to the comparison between the outputs 
% CourseChangeNotifier and link detection algorithm. This script
% complements the CheckCourseChangevsMemMap.m script.
%
% Author            : Bora Karaoglu
% Last Update by    : Bora Karaoglu
% Last Update date  : 08/04/2014 12:31PM

%% Input:
% CourseChangevsMemMap.mat
%
%% Output:

%% Notes

%% Input
clear all;
clc;
close all;
load('CourseChangevsMemMap.mat'); 

rawdata = LDvsCN_link_LET;
%% Plot cdfs for LETs and LBTs
figh = figure();
axeshandle = gca;
hist(axeshandle,LDvsCN_link_LET,20 );
nameoffig = 'Hist__LDvsCN__LET';
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

figh = figure();
axeshandle = gca;
[curvehandle,stats] = cdfplot(LDvsCN_link_LET);
nameoffig = 'cdf__LDvsCN_LET';
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

figh = figure();
axeshandle = gca;
hist(axeshandle,LDvsCN_link_LBT,20 );
nameoffig = 'Hist__LDvsCN__LBT';
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

figh = figure();
axeshandle = gca;
[curvehandle,stats] = cdfplot(LDvsCN_link_LBT);
nameoffig = 'cdf__LDvsCN_LBT';
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

%% Fit Distributions to LET
figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = '1-FxforLDvsCN__LET';
[Fx, xval ] = ecdf(rawdata);
plot(xval,1-Fx, 'DisplayName','empirical');
[p,ci] = wblfit(rawdata);
Fwbl = wblcdf(xval, p(1), p(2));
plot(xval,1-Fwbl,'DisplayName','weibulfit','color','r');
pexp = expfit(rawdata);
Fexp = expcdf(xval, pexp);
plot(xval,1-Fexp,'DisplayName','expfit','color','g');
% set(axeshandle,'xscale','log');
% set(axeshandle,'yscale','log');
legend(axeshandle,'show');
xlabel('Data');
ylabel('1-F(x)')
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

pd = fitdist(rawdata','Weibull');
[h,pvalue,stats] = chi2gof(rawdata,'CDF',pd) ;
[h,p,ksstat,cv] = kstest(rawdata, 'CDF',pd) ;

pd = fitdist(rawdata','Lognormal');
[h,pvalue,stats] = chi2gof(rawdata,'CDF',pd) ;
[h,p,ksstat,cv] = kstest(rawdata, 'CDF',pd)  ;

figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'pdf__estimates__LET';
[nelements, xval ] = hist(axeshandle,rawdata,20 );
fx = (nelements/numel(rawdata))/(xval(2)-xval(1));
bar(xval,fx, 'DisplayName','empirical');
[p,ci] = wblfit(rawdata);
fwbl = wblpdf(xval, p(1), p(2));
plot(xval,fwbl,'DisplayName','weibulfit','color','r');
pexp = expfit(rawdata);
fexp = exppdf(xval, p(1));
plot(xval,fexp,'DisplayName','expfit','color','g');
legend(axeshandle,'show');
xlabel('Data');
ylabel('f(x)')
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');

%% Plot w= ln((Fx-1)/(1-Fx))
% figh = figure();
% axeshandle = gca;
% hold on;
% grid on;
% nameoffig = 'Fxloglog__LET';
% xlabel('ln(x)');
% ylabel('ln((Fx-1)./(1-Fx))')
% 
% [Fx, xval ] = ecdf(rawdata);
% y = Fx(1:end-1);
% x = xval(1:end-1);
% plot(x,log((y-1)./(1-y)),'b','DisplayName','Empirical');
% 
% [p,ci] = wblfit(rawdata);
% Fwbl = wblcdf(xval, p(1), p(2));
% y = Fwbl(1:end-1);
% x = xval(1:end-1);
% plot(x,log((y-1)/(1-y)),'r','DisplayName','Weibull fit');
% 
% [muhat,sigma] = normfit(rawdata);
% F = normcdf(xval, muhat, sigma);
% y = F(1:end-1);
% x = xval(1:end-1);
% plot(x,log((y-1)/(1-y)),'r','DisplayName','Normal fit');
% 
% pexp = expfit(rawdata);
% F = expcdf(xval, pexp);
% y = F(1:end-1);
% x = xval(1:end-1);
% plot(x,log((y-1)/(1-y)),'r','DisplayName','Normal fit');
% 
% legend(axeshandle,'show');
% title(nameoffig);
% saveas(figh,nameoffig,'fig');
% saveas(figh,nameoffig,'jpg');

%% Check cumulative averages
% 
rawdata2 = normrnd(0,100,[1 500000]);
%rawdata2 = rawdata2(rawdata2>10);
rawdata3 = exprnd(1,[1 5000]);
rawdata4 = wblrnd(1,2,[1 500000]);
rawdata5 = [2.426	1.953	1.418	1.08	3.735	2.307	1.876	1.11	3.131	1.134	1.171	1.141	2.181	1.007	1.076	1.131	1.156	2.264	2.535	1.001	1.099	1.149	1.225	1.099	1.279	1.052	1.051	9.421	1.346	1.532	1	1.106	1.126	1.293	1.13	1.043	1.254	1.118	1.027	1.383	1.288	1.988	1.561	1.106	1.256	1.187	1.084	1.968	1.045	1.155];
K = 2;
sigma = 1;
theta = sigma/K ; 
rawdata6 = gprnd(K,sigma,theta,[1 5000]);
rawdata7 = 1./unifrnd(0,1,[1 5000]);

rawdata_used = rawdata2;


x = 1:numel(rawdata_used);
%fcoeff = ones(1,movavsize)/movavsize;
%y = filter(fcoeff,1,rawdata7);
%y = smooth(rawdata7,movavsize);
y = cumsum(rawdata_used)./ x;


figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'qq';
xlabel('Number of Observations');
ylabel('Cumulative Mean');
plot(x,y,'r', 'DisplayName','MovAverage');

%% Calculate tail index which is the slope
% % % % Plot moving mean
rawdata_used = LDvsCN_link_LBT;

figh = figure();
axeshandle = gca;
hold on;
grid on;
nameoffig = 'tailindex';
[Fx, xval ] = ecdf(rawdata_used);
% xval = 0.1:0.001:10;
% Fx = 1 - (xval.^-0.2);

y = -1*log(1-Fx(1:end-1));
x = log(xval(1:end-1));
ylabel('-ln(1-Fx)');
xlabel('ln(x)')
scatter(x,y,1,[0 1 0],'DisplayName',rawdata2name);
p = polyfit(x,y, 1);
plot(x,polyval(p,x),'r', 'DisplayName','fittedline');
xselect = (max(x)-min(x))/2 + min(x);
text(xselect,polyval(p,xselect), ['\leftarrow y = ' num2str(p(1)) 'x + '  num2str(p(2))])
tailindex = p(1);
legend(axeshandle,'show');
title(nameoffig);
saveas(figh,nameoffig,'fig');
saveas(figh,nameoffig,'jpg');


%% Discarded code
% % % %% Check moving averages
% % % % Plot moving mean
% % % rawdata2 = normrnd(0,100,[1 5000]);
% % % rawdata2 = rawdata2(rawdata2>10)
% % % % rawdata3 = exprnd(1,[1 5000]);
% % % % rawdata4 = wblrnd(0.06,1.56,[1 5000]);
% % % % rawdata5 = [2.426	1.953	1.418	1.08	3.735	2.307	1.876	1.11	3.131	1.134	1.171	1.141	2.181	1.007	1.076	1.131	1.156	2.264	2.535	1.001	1.099	1.149	1.225	1.099	1.279	1.052	1.051	9.421	1.346	1.532	1	1.106	1.126	1.293	1.13	1.043	1.254	1.118	1.027	1.383	1.288	1.988	1.561	1.106	1.256	1.187	1.084	1.968	1.045	1.155];
% % % % 
% % % % K = 2;
% % % % sigma = 1;
% % % % theta = sigma/K ; 
% % % % rawdata6 = gprnd(K,sigma,theta,[1 5000]);
% % % % 
% % % % rawdata7 = 1./unifrnd(0,1,[1 5000]);
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % hold on;
% % % grid on;
% % % nameoffig = 'qq';
% % % % [Fx, xval ] = ecdf(rawdata2);
% % % xval = 0.1:0.001:10;
% % % 
% % % Fx = 1 - (xval.^-0.2);
% % % y = -1*log(1-Fx(1:end-1));
% % % x = log(xval(1:end-1));
% % % ylabel('-ln(1-Fx)');
% % % xlabel('ln(x)')
% % % scatter(x,y,1,[0 1 0]);
% % % p = polyfit(x,y, 1);
% % % plot(x,polyval(p,x),'r', 'DisplayName','fittedline');
% % % xselect = (max(x)-min(x))/2 + min(x);
% % % text(xselect,polyval(p,xselect), ['\leftarrow y = ' num2str(p(1)) 'x + '  num2str(p(2))])
% % % tailindex = p(1);

% % %% Calculate tail index which is the slope
% % % 
% % 
% % figh = figure();
% % axeshandle = gca;
% % hold on;
% % grid on;
% % nameoffig = 'Kenneth''s graph';
% % 
% % 
% % 
% % rawdata2 = normrnd(0,100,[1 5000]);
% % rawdata2 = rawdata2(rawdata2>10)
% % % rawdata3 = exprnd(1,[1 5000]);
% % % rawdata4 = wblrnd(0.06,1.56,[1 5000]);
% % % rawdata5 = [2.426	1.953	1.418	1.08	3.735	2.307	1.876	1.11	3.131	1.134	1.171	1.141	2.181	1.007	1.076	1.131	1.156	2.264	2.535	1.001	1.099	1.149	1.225	1.099	1.279	1.052	1.051	9.421	1.346	1.532	1	1.106	1.126	1.293	1.13	1.043	1.254	1.118	1.027	1.383	1.288	1.988	1.561	1.106	1.256	1.187	1.084	1.968	1.045	1.155];
% % % 
% % % K = 2;
% % % sigma = 1;
% % % theta = sigma/K ; 
% % % rawdata6 = gprnd(K,sigma,theta,[1 5000]);
% % % 
% % rawdata7 = 1./unifrnd(0,1,[1 1000]);
% % rawdata8 = unifrnd(0,1,[1 1000]);
% % 
% % [Fx, xval ] = ecdf(rawdata8);
% % x = asinh(xval(1:end-1));
% % y = -1*log(1-Fx(1:end-1));
% % ylabel('ln(1-F_x)');
% % xlabel('asinh(x)')
% % plot(x,y,'r', 'DisplayName','MovAverage');


% % %% Draw a QQ plot assuming a pareto distribution
% % t = trnd(5,5000,1);
% % y = t(t>2) - 2;
% % pd2 = fitdist(y,'GeneralizedPareto');
% % pd = fitdist(rawdata','GeneralizedPareto');
% % figh = figure();
% % qqplot(rawdata,pd)
% % 
% % figh = figure();
% % qqplot(y,pd2)
% % 
% % x=0.1:0.001:10;
% % 
% % figh = figure();
% % axeshandle = gca;
% % [Fx, xval ] = ecdf(rawdata);
% % y1 = log(1-Fx);
% % plot(xval,y1, 'Color', 'b', 'DisplayName', 'rawdata')
% % hold on;
% % [Fx, xval ] = ecdf(y);
% % y2 = log(1-Fx);
% % plot(xval,y2, 'Color', 'r','DisplayName', 'Truncated Student t')
% % Fx = normcdf(xval,0,1);
% % y3 = log(1-Fx);
% % plot(xval,y3, 'Color', 'g','DisplayName', 'normal')
% % 
% % plot(axeshandle,'show');
% % xlabel('data');
% % ylabel('1-Fx');
% % 
% % 
% % figh = figure();
% % axeshandle = gca;
% % x = (1-F)
% % plot(xval,y1, 'Color', 'b', 'DisplayName', 'rawdata')
% % hold on;
% % [Fx, xval ] = ecdf(y);
% % y2 = log(1-Fx);
% % plot(xval,y2, 'Color', 'r','DisplayName', 'Truncated Student t')
% % Fx = normcdf(xval,0,1);
% % y3 = log(1-Fx);
% % plot(xval,y3, 'Color', 'g','DisplayName', 'normal')
% % 
% % plot(axeshandle,'show');
% % xlabel('data');
% % ylabel('1-Fx');
% % log(z)./log(x)
% % 
% % 
% % 
% % figh = figure();
% % [Fx, xval ] = ecdf(rawdata);
% % loglog(xval,1-Fx)
% % 
% % F_pareto = gppdf(x);
% % polyfit(x,y,n)

% % % LDvsCN_linkbre = CNvsLD_linkbre * -1;
% % % figh = figure();
% % % axeshandle = gca;
% % % hist(axeshandle,LDvsCN_linkbre,20 );
% % % nameoffig = 'Breakage__Hist__LDvsCN';
% % % title(nameoffig);
% % % saveas(figh,nameoffig,'fig');
% % % saveas(figh,nameoffig,'jpg');
% % % 
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % [curvehandle,stats] = cdfplot(-1*CNvsLD_linkbre);
% % % nameoffig = 'Breakage__cdf__LDvsCN';
% % % title(nameoffig);
% % % saveas(figh,nameoffig,'fig');
% % % saveas(figh,nameoffig,'jpg');
% % % %%
% % % % figh = figure();
% % % % axeshandle = gca;
% % % % [nelements, centers]= hist(axeshandle,-1*CNvsLD_LET(CNvsLD_LET>-0.4),40 );
% % % % bar(axeshandle, centers, nelements/(sum(nelements)),1)
% % % % title('Histogram CN vs LD Link Establishments');
% % % % 
% % % % rawdata = -1*CNvsLD_LET(CNvsLD_LET>-0.4);
% % % % 
% % % % x = 0:0.001:10;
% % % % y1 = -1*CNvsLD_LET(CNvsLD_LET>-0.4);
% % % % a = repmat(y1,[numel(x) 1]) ;
% % % % b = repmat(x',[1 numel(y1)]) ;
% % % % c = a<b;
% % % % cumprob = sum( c ,2) / numel(y1) ; 
% % % % figh = figure();
% % % % axeshandle = gca;
% % % % scatter(x, cumprob)
% % % 
% % % % figh = figure();
% % % % axeshandle = gca;
% % % % [nelements, centers] = hist(axeshandle,log(-1*CNvsLD_LET(CNvsLD_LET>-0.4)),40 );
% % % % bar(axeshandle, centers, nelements/(sum(nelements)),1)
% % % % title('Histogram CN vs LD Link Establishments');
% % % % nameoffig = 'Est_Hist_LDvsCN';
% % % % saveas(figh,nameoffig,'fig');
% % % % saveas(figh,nameoffig,'jpg');
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % [curvehandle,stats] = cdfplot(-1*CNvsLD_LET);
% % % title('CDF CN vs LD Link Establishments');
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % [curvehandle,stats] = cdfplot(-1*CNvsLD_LET(CNvsLD_LET>-0.4));
% % % title('CDF CN vs LD Link Establishments');
% % % 
% % % 
% % % %% Plot hist and cdfs for the link durations
% % % figh = figure();
% % % axeshandle = gca;
% % % hist(axeshandle,CN_LinkDurations,20 );
% % % nameoffig='Histogram CN Link Durations';
% % % title(nameoffig);
% % % saveas(figh,nameoffig,'fig');
% % % saveas(figh,nameoffig,'jpg');
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % [curvehandle,stats] = cdfplot(CN_LinkDurations);
% % % nameoffig = 'CDF CN Link Durations';
% % % title(nameoffig);
% % % saveas(figh,nameoffig,'fig');
% % % saveas(figh,nameoffig,'jpg');
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % hist(axeshandle,LD_LinkDurations,20 );
% % % nameoffig = 'Histogram  LD Link Durations';
% % % title(nameoffig);
% % % saveas(figh,nameoffig,'fig');
% % % saveas(figh,nameoffig,'jpg');
% % % 
% % % figh = figure();
% % % axeshandle = gca;
% % % [curvehandle,stats] = cdfplot(LD_LinkDurations);
% % % nameoffig = 'CDF LD Link Durations';
% % % title(nameoffig);
% % % saveas(figh,nameoffig,'fig');
% % % saveas(figh,nameoffig,'jpg');
% % % 
% % % %% Estimate Link Breakages with Weibull
% % % % [parmhat parmci] = wblfit(-1*CNvsLD_linkbre);
% % % % a = parmhat(1);
% % % % b = parmhat(2);
% % % %
% % % % figh = figure();
% % % % axeshandle = gca;
% % % % hist(axeshandle,-1*CNvsLD_linkbre,20 );
% % % % hold on;
% % % % x = 0.1:0.001:0.4;
% % % % plot(x,wblpdf(x,a,b),'.-r');
% % % % title('Histogram CN vs LD Link Breakages');
% % % % legend('Empirical','Theoretical','Location','NW')
% % % %
% % % % figh = figure();
% % % % axeshandle = gca;
% % % % [curvehandle,stats] = cdfplot(-1*CNvsLD_linkbre);
% % % % hold on;
% % % % x = 0.1:0.001:0.4;
% % % % plot(x,wblcdf(x,a,b),'.-r');
% % % % title('CDF CN vs LD Link Breakages');
% % % % legend('Empirical','Theoretical','Location','NW')
% % % 
% % % %% Plot link duration
% % % 
% % % 
% % % 
