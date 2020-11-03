function error = testing_distribution(test_fractions)
    % Main function for finding testing distribution with time
    %
    % test_fractions is a matrix, each row is a pair of testing
    % probabilities at thtat time step; first one is for exposed and second
    % is for symptomatic

    % Apply constraints (all values should be in the [0,1] interval)
    test_fractions = constrain_values(test_fractions);
    
    % Save new input
    generate_test_input(test_fractions, 'input_data/tests_with_time.txt');
   
    % Compute the time average of symptomatic testing and substitute in
    % input parameters
    compute_average_testing('input_data/tests_with_time.txt', 'input_data/ingection_parameters.txt');
    
    % Run the simulation
    parametric_driver
    
    % Postprocess weekly total and compute the error
    load('optimization_set')
    load('agregated_data')
    real_tot = load('real_data/real_tot_cases_w_time.txt');
    
    % Collection start in steps
    cst = 41;
    % Adjust
    time = time - 10.0;
    % Total increase per week
    ylab = 'Total number of new cases in a week';

    agregated_time = [17, 24, 32, 37, 45, 51, 59, 66, 73, 80, 87, 94, 101, 107, 115, 121, 129, 136, 140];
    
    % Real
    rtemp = agregated_real(:,end);
    rtemp = rtemp(~isnan(rtemp));

    % Simulation
    temp = tot_pos(:,cst:4:end) + tot_fpos(:,cst:4:end)+ not_tested_deaths(:,cst:4:end);
    week_sim = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
     
    % Difference
    error =  norm(rtemp-reshape(mean(week_sim),size(rtemp)));   
end

function generate_test_input(test_fractions, fname)
    % Assumes time intervals/points are the same

    old_set = load(fname);
    new_set(:,1) = old_set(:,1);
    new_set(:,2:3) = test_fractions;
           
    save(fname, 'new_set', '-ascii')
end

function compute_average_testing(ftests_time, f_input_params)
    % Calculate time-average of symptomatic tested fractions and updated
    % the input parameter list
    
    % This is all very hardcoded!
    
    % Load testing distribution from previous iteration which is day 150
    % (151st day in the simulation, and 601st step - think of it as 
    % evening of February 22nd) 
    data = load(ftests_time);
    
    % Create arrays with known values (at step 9 all new symptomatic are
    % tested, before that it is all zero, and last recorded value stays
    % until the end of the simulation which is 
    % Length of the interval for each testing fraction - days
    n_int = 10;
    mid_y = zeros(length(data(:,3))-1,n_int);
    mid_x = mid_y;
    % This can be used for visualization - it generates even day
    % intervals
    for i=1:length(data(:,3))-1
        mid_y(i,1:end) = data(i,3);
        %mid_x(i,:) = linspace(data(i,1),data(i+1,1)-1,n_int);
    end
    y=[zeros(1,9),1,reshape(mid_y.',1,[]),ones(1,51)*data(end,3)];
    mean_testing = mean(y);
    
    % Substitute in Python
    system(sprintf('./sub_average_testing.py %f', mean_testing));
end

function temp = constrain_values(test_fractions)
    % Move negative and large values to [0,1] interval
    test_fractions=[arrayfun(@(x) max(x,0.0), test_fractions)];
    temp=[arrayfun(@(x) min(x,1.0), test_fractions)];
end
