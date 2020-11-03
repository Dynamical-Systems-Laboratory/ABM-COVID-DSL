%% Run a number of COVID simulations and store results

clear
close all

num_sim = 100;
num_steps = 600;
num_pop = 79205;
dt = 0.25;
time=0:dt:dt*num_steps;

% Number of agents to vaccinate
to_vaccinate = load('input_data/random_v_numbers.txt');

% All the collected data
inf_data = zeros(num_sim, num_steps+1);
new_pos_data = zeros(num_sim, num_steps+1);
new_neg_data = zeros(num_sim, num_steps+1);
new_fpos_data = zeros(num_sim, num_steps+1);
new_fneg_data = zeros(num_sim, num_steps+1);
new_tested = zeros(num_sim, num_steps+1);
%
new_infected = zeros(num_sim, num_steps+1);
tot_pos = zeros(num_sim, num_steps+1);
tot_neg = zeros(num_sim, num_steps+1);
tot_fpos = zeros(num_sim, num_steps+1);
tot_fneg = zeros(num_sim, num_steps+1);
tot_tested = zeros(num_sim, num_steps+1);
tot_active = zeros(num_sim, num_steps+1);
tot_deaths = zeros(num_sim, num_steps+1);
tested_deaths = zeros(num_sim, num_steps+1);
not_tested_deaths = zeros(num_sim, num_steps+1);
%
tot_ih = zeros(num_sim, num_steps+1);
tot_hn = zeros(num_sim, num_steps+1);
tot_icu = zeros(num_sim, num_steps+1);

for i=1:num_sim
   i
   system(sprintf('./rvac_covid_exe %.0f >> output/simulation.log', 10*to_vaccinate(i)));
   % Collect 
   inf_data(i,:) = load('output/infected_with_time.txt');
   new_pos_data(i,:) = load('output/new_tested_pos_step.txt');
   new_neg_data(i,:) = load('output/new_tested_neg_step.txt');
   new_fpos_data(i,:) = load('output/new_tested_false_pos_step.txt');
   new_fneg_data(i,:) = load('output/new_tested_false_neg_step.txt');
   %
   new_tested(i,:) = load('output/new_tested_step.txt');
   new_infected(i,:) = load('output/new_infected_step.txt');
   %
   tot_pos(i,:) = load('output/total_tested_pos.txt');
   tot_tested(i,:) = load('output/total_tested.txt');
   tot_active(i,:) = load('output/active_with_time.txt');
   tot_deaths(i,:) = load('output/dead_with_time.txt');
   tested_deaths(i,:) = load('output/tested_dead_with_time.txt');
   not_tested_deaths(i,:) = load('output/not_tested_dead_with_time.txt');
   tot_fneg(i,:) = load('output/total_tested_false_neg.txt');
   tot_fpos(i,:) = load('output/total_tested_false_pos.txt');
   tot_neg(i,:) = load('output/total_tested_neg.txt');
   %
   tot_ih(i,:) = load('output/home_isolated_with_time.txt');
   tot_hn(i,:) = load('output/hospitalized_with_time.txt');
   tot_icu(i,:) = load('output/icu_with_time.txt');
end

save('random_vaccination_ten_times')
