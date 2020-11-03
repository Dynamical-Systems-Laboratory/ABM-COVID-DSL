%% Optimization driver for finding best testing fraction distribution

% Optimization settings
options = optimset('Display','iter', 'MaxFunEvals', 100);

% Initial values - [shape, scale]
x0 = load('opt_inputs/every_10.txt');
x0 = x0(:,2:3);

% Optimization
[x,fval]=fminsearch(@testing_distribution, x0, options);
