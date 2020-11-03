%% Process and fit the distributions
clear

% % ------ Gamma 
% 
% % Optimization settings
% options=optimset('Display','iter','TolX', 1e-16, 'TolFun', 1e-16, 'MaxFunEvals', 500);
% % Initial values - [shape, scale]
% x0=[1e-2, 10.0];
% % Optimization
% [x,fval]=fminsearch(@gamma_search, x0, options);
% 
% data = load('onset_to_hosp_linton.csv');
% gpdf = gampdf(data(:,1), x(1), x(2));
% plot(data(:,1), data(:,2),'ro')
% hold on
% plot(data(:,1), gpdf,'k')

% ------ Gamma - flu testing

% Optimization settings
options=optimset('Display','iter','TolX', 1e-16, 'TolFun', 1e-16, 'MaxFunEvals', 500);
% Initial values - [shape, scale]
x0=[1.5, 5.0];
% Optimization
[x,fval]=fminsearch(@gamma_flu_search, x0, options);

data = load('hosp_data.txt');
gpdf = gampdf(data(1,:), x(1), x(2));
plot(data(1,:), data(2,:),'ro')
hold on
plot(data(1,:), gpdf,'k')

% ------ Lognormal

% Optimization settings
% options=optimset('Display','iter','TolX', 1e-16, 'TolFun', 1e-16, 'MaxFunEvals', 1500);
% % Initial values - [shape, scale]
% x0=[2.6612    0.4819];
% % Optimization
% [x,fval]=fminsearch(@lognormal_search, x0, options);
% 
% data = load('onset_to_death_linton.csv');
% gpdf = lognpdf(data(:,1), x(1), x(2));
% plot(data(:,1), data(:,2),'ro')
% hold on
% plot(data(:,1), gpdf,'k')

% ------ Weibull

% Optimization settings
% options=optimset('Display','iter','TolX', 1e-16, 'TolFun', 1e-16, 'MaxFunEvals', 1500, 'MaxIter', 1500);
% % Initial values - [shape, scale]
% x0=[1.0 1.0];
% % Optimization
% [x,fval]=fminsearch(@weibull_search, x0, options);
% 
% data = load('hospitalization_to_death_linton.csv');
% gpdf = wblpdf(data(:,1), x(1), x(2));
% plot(data(:,1), data(:,2),'ro')
% hold on
% plot(data(:,1), gpdf,'k')
% x