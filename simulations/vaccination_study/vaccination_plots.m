%% Data comparison - different datasets and real data

clear
close all

load('real_data/agregated_data')

% Plot settings for each considered case
% Realizations 
% clrf = [0.2, 0.2, 0.2; 0.4, 0.4, 0.4; 0.6, 0.6, 0.6; 0.8, 0.8, 0.8; 0.9, 0.9, 0.9];
clrf = [0.4, 0.4, 0.4; 178/255, 198/255, 238/255; 175/255, 154/255, 210/255; 228/255, 181/255, 181/255; 205/255, 197/255, 181/255];
% Mean
% clrm = [164/255, 164/255, 230/255; 68/255, 68/255, 213/255; 21/255, 23/255, 150/255; 12/255, 12/255, 128/255; 3/255, 3/255, 129/255;];
clrm = [0.14, 0.14, 0.14; 7/255, 70/255, 198/255; 77/255, 7/255, 198/255; 185/255, 46/255, 46/255; 91/255, 79/255, 54/255;];

% Number of datasets to consider in one plot
nsets = 3;
% Number of trials in each set
ntrials = 100;

% County size
n_county = 967506;
% New Rochelle size
n_new_rochelle = 79205;

% Collection start in steps
cst = 41;
% Adjust - just to collect the time
time_data = load('base_case');
time = time_data.time - 10.0;

% Real data
% For 03/03 or later
real_active = load('real_data/real_active_w_time.txt');
real_tot = load('real_data/real_tot_cases_w_time.txt');
real_deaths = load('real_data/real_tot_deaths_county_w_time.txt');
% Total tested since 03/02
real_tested = load('real_data/real_tested.txt');

%% ---> Active cases
ylab = 'Number of active cases';
plot_title = 'Weekly average, $\mathrm{T_p + T_p^f}$';
rtemp = agregated_real(:,3);
rtemp = rtemp(~isnan(rtemp));
agregated_time = [66, 73, 80, 87, 94, 101, 107, 115, 121, 129, 136, 140];

% Agregate all datasets into one multidimensional array
combined_res = zeros(ntrials, size(time(cst:end), 2), nsets);
% 1) No restrictions
sim_res = load('base_case');
combined_res(:,:,1) = sim_res.tot_active(:,cst:end);
% 2) No restrictions, vaccination of hospital employees
sim_res = load('hsp_employee_vaccination');
combined_res(:,:,2) = sim_res.tot_active(:,cst:end);
% 3) No restrictions, vaccination of random population, equivalent to hospital employees
sim_res = load('random_vaccination');
combined_res(:,:,3) = sim_res.tot_active(:,cst:end);
% 4) No restrictions, vaccination of random, 10x hospital employees
sim_res = load('random_vaccination_10_times');
combined_res(:,:,4) = sim_res.tot_active(:,cst:end);

plot_all_and_mean(time(cst:end), combined_res, agregated_time, rtemp, 1, clrm, clrf, plot_title, ylab, false)

%% ---> Total cases

combined_res = zeros(ntrials, size(time(cst:end), 2), nsets);
ylab = 'Total number of cases';
plot_title = '$\mathrm{T_p+ T_p^f}$';

% 1) No restrictions
sim_res = load('base_case');
combined_res(:,:,1) = sim_res.tot_pos(:,cst:end) + sim_res.tot_fpos(:,cst:end) + sim_res.not_tested_deaths(:,cst:end);
% 2) No restrictions, vaccination of hospital employees
sim_res = load('hsp_employee_vaccination');
combined_res(:,:,2) = sim_res.tot_pos(:,cst:end)+ sim_res.tot_fpos(:,cst:end) + sim_res.not_tested_deaths(:,cst:end);
% 3) No restrictions, vaccination of random population, equivalent to hospital employees
sim_res = load('random_vaccination');
combined_res(:,:,3) = sim_res.tot_pos(:,cst:end)+ sim_res.tot_fpos(:,cst:end) + sim_res.not_tested_deaths(:,cst:end);
% 4) No restrictions, vaccination of random, 10x hospital employees
sim_res = load('random_vaccination_10_times');
combined_res(:,:,4) = sim_res.tot_pos(:,cst:end)+ sim_res.tot_fpos(:,cst:end) + sim_res.not_tested_deaths(:,cst:end);

plot_all_and_mean(time(cst:end), combined_res, real_tot(1:end,1), real_tot(1:end,2), 2, clrm, clrf, plot_title, ylab, false)

%% ---> Total weekly cases

ylab = 'Total number of new cases in a week';
plot_title = '$\mathrm{T_p+ T_p^f}$';

agregated_time = [17, 24, 32, 37, 45, 51, 59, 66, 73, 80, 87, 94, 101, 107, 115, 121, 129, 136, 140];
rtemp = agregated_real(:,end);
rtemp = rtemp(~isnan(rtemp));
combined_res = zeros(ntrials, size(agregated_time, 2), nsets);

% 1) No restrictions
sim_res = load('base_case');
temp = sim_res.tot_pos(:,cst:4:end)+ sim_res.tot_fpos(:,cst:4:end) + sim_res.not_tested_deaths(:,cst:4:end);
combined_res(:,:,1) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
% 2) No restrictions, vaccination of hospital employees
sim_res = load('hsp_employee_vaccination');
temp = sim_res.tot_pos(:,cst:4:end)+ sim_res.tot_fpos(:,cst:4:end) + sim_res.not_tested_deaths(:,cst:4:end);
combined_res(:,:,2) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
% 3) No restrictions, vaccination of random population, equivalent to hospital employees
sim_res = load('random_vaccination');
temp = sim_res.tot_pos(:,cst:4:end)+ sim_res.tot_fpos(:,cst:4:end) + sim_res.not_tested_deaths(:,cst:4:end);
combined_res(:,:,3) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
% 4) No restrictions, vaccination of random, 10x hospital employees
sim_res = load('random_vaccination_10_times');
temp = sim_res.tot_pos(:,cst:4:end)+ sim_res.tot_fpos(:,cst:4:end) + sim_res.not_tested_deaths(:,cst:4:end);
combined_res(:,:,4) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];

plot_all_and_mean(agregated_time, combined_res, agregated_time, rtemp, 3, clrm, clrf, plot_title, ylab, false)

%% ---> Total number of deaths
combined_res = [];
ylab = 'Total number of deaths';
plot_title = '$\mathrm{R_D}$';

% 1) No restrictions
sim_res = load('base_case');
combined_res(:,:,1) = sim_res.tot_deaths(:,cst:end);
% 2) No restrictions, vaccination of hospital employees
sim_res = load('hsp_employee_vaccination');
combined_res(:,:,2) = sim_res.tot_deaths(:,cst:end);
% 3) No restrictions, vaccination of random population, equivalent to hospital employees
sim_res = load('random_vaccination');
combined_res(:,:,3) = sim_res.tot_deaths(:,cst:end);
% 4) No restrictions, vaccination of random, 10x hospital employees
sim_res = load('random_vaccination_10_times');
combined_res(:,:,4) = sim_res.tot_deaths(:,cst:end);

plot_all_and_mean(time(cst:end), combined_res, real_deaths(:,1), real_deaths(:,2)/n_county*n_new_rochelle, 4, clrm, clrf, plot_title, ylab, false)

%% ---> Weekly deaths
combined_res = [];
ylab = 'Number of deaths in a week';
plot_title = '$\mathrm{R_D}$';

weekly_deaths = [0, 10, 201, 148, 309, 195, 186, 142, 78, 50, 40]/n_county*n_new_rochelle;
agregated_time = [17, 24, 32, 37, 44, 51, 59, 66, 73, 80, 87];

% 1) No restrictions
sim_res = load('base_case');
temp = sim_res.tot_deaths(:,cst:4:end);
combined_res(:,:,1) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
% 2) No restrictions, vaccination of hospital employees
sim_res = load('hsp_employee_vaccination');
temp = sim_res.tot_deaths(:,cst:4:end);
combined_res(:,:,2) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
% 3) No restrictions, vaccination of random population, equivalent to hospital employees
sim_res = load('random_vaccination');
temp = sim_res.tot_deaths(:,cst:4:end);
combined_res(:,:,3) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
% 4) No restrictions, vaccination of random, 10x hospital employees
sim_res = load('random_vaccination_10_times');
temp = sim_res.tot_deaths(:,cst:4:end);
combined_res(:,:,4) = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];

plot_all_and_mean(agregated_time, combined_res, agregated_time, weekly_deaths, 5, clrm, clrf, plot_title, ylab, false)


function plot_all_and_mean(time, y, t_real, y_real, i, clrm, clrf, plot_title, ylab, noMarkers)
   
    % Create figure
    figure1 = figure(i);

    % Create axes
    axes1 = axes('Parent',figure1);

    % Process each dataset separately
%     for dset=1:size(y,3)
%         for i=1:size(y,1)
%                 % To plot all realizations
% %             plot(time, y(i,:,dset), 'LineWidth', 2, 'Color', [clrf(dset,:),0.5])
%             hold on
%         end
%         % and the mean
% %         plot(time, mean(y(:,:,dset),1), 'LineWidth', 2, 'Color', clrm(dset,:))
%     end

    % To plot just the mean
    for dset=1:size(y,3)
         plot(time, mean(y(:,:,dset),1), 'LineWidth', 2, 'Color', clrm(dset,:))
         hold on
    end
     
    if noMarkers == false
        plot(t_real, y_real, 'ko', 'LineWidth', 2, 'MarkerSize', 10)
    else
        plot(t_real, y_real, 'k-.', 'LineWidth', 2)
    end
  
    % Create ylabel
    ylabel(ylab,'Interpreter','latex');

    % Create xlabel
    xlabel('Time (days)','Interpreter','latex');

    % Create title
    title(plot_title,'Interpreter','latex');

    % Uncomment the following line to preserve the Y-limits of the axes
    % ylim(axes1,[0 5]);
    box(axes1,'on');
    % Set the remaining axes properties
    set(axes1,'FontSize',28,'TickLabelInterpreter','latex','XGrid','on','YGrid',...
        'on'); 
    
    % Ticks
    xlim([1,140])
    xticks([1 50 100 140])
    xticklabels({'March 3','April 22','June 11','July 21'})
    
    % Add events
%     plot([10,10],ylim, '--', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
%     plot([19,19],ylim, '-.', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
%     plot([84,84],ylim, '--', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
%     plot([98, 98],ylim, '-.', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
%     plot([112, 112],ylim, ':', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    
    
end