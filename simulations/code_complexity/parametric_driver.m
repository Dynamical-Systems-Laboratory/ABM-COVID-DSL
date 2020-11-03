%% Run a number of COVID simulations for complexity study

clear
close all

num_sim = 100;
num_steps = 100;

for i=1:num_sim
   !./covid_exe >> output/simulation.log
end

