%% Post-processing 
clear
close all

% Data directory and the mat file
mfile = 'base_case';

% Plot settings
clrf = [0.7, 0.7, 0.7];
clrm = [21/255, 23/255, 150/255];

% County size
n_county = 967506;
% New Rochelle size
n_new_rochelle = 79205;

% Real data
% For 03/03 or later
real_active = load('real_data/real_active_w_time.txt');
real_tot = load('real_data/real_tot_cases_w_time.txt');
real_deaths = load('real_data/real_tot_deaths_county_w_time.txt');
% Total tested since 03/02
real_tested = load('real_data/real_tested.txt');

% Plot all realizations and the mean
load(mfile)
load('agregated_data')

% Collection start in steps
cst = 41;

% Adjust
time = time - 10.0;

% Active cases
ylab = 'Number of active cases';
plot_title = '$\mathrm{T_p + T_p^f}$';
temp = tot_active(:,cst:end);
plot_all_and_mean(time(cst:end), temp, real_active(:,1), real_active(:,2), 1, clrm, clrf, plot_title, ylab, false)

% Active vs. weekly average
ylab = 'Number of active cases';
plot_title = 'Weekly average, $\mathrm{T_p + T_p^f}$';
temp = tot_active(:,cst:end);
rtemp = agregated_real(:,3);
rtemp = rtemp(~isnan(rtemp));
agregated_time = [66, 73, 80, 87, 94, 101, 107, 115, 121, 129, 136, 140];
plot_all_and_mean(time(cst:end), temp, agregated_time, rtemp, 2, clrm, clrf, plot_title, ylab, false)

% Total
ylab = 'Total number of cases';
plot_title = '$\mathrm{T_p+ T_p^f}$';
temp = tot_pos(:,cst:end) + tot_fpos(:,cst:end) + not_tested_deaths(:,cst:end);
plot_all_and_mean(time(cst:end), temp, real_tot(1:end,1), real_tot(1:end,2), 3, clrm, clrf, plot_title, ylab, false)

% Total increase per week
ylab = 'Total number of new cases in a week';
plot_title = '$\mathrm{T_p+ T_p^f}$';

agregated_time = [17, 24, 32, 37, 45, 51, 59, 66, 73, 80, 87, 94, 101, 107, 115, 121, 129, 136, 140];
rtemp = agregated_real(:,end);
rtemp = rtemp(~isnan(rtemp));

temp = tot_pos(:,cst:4:end) + tot_fpos(:,cst:4:end) + not_tested_deaths(:,cst:4:end);
week_sim = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
plot_all_and_mean(agregated_time, week_sim, agregated_time, rtemp, 4, clrm, clrf, plot_title, ylab, false)

% Total deaths
ylab = 'Total number of deaths';
plot_title = '$\mathrm{R_D}$';
temp = tot_deaths(:,cst:end);
plot_all_and_mean(time(cst:end), temp, real_deaths(:,1), real_deaths(:,2)/n_county*n_new_rochelle, 5, clrm, clrf, plot_title, ylab, false)

% New weekly deaths
ylab = 'Number of deaths in a week';
plot_title = '$\mathrm{R_D}$';

weekly_deaths = [0, 10, 201, 148, 309, 195, 186, 142, 78, 50, 40]/n_county*n_new_rochelle;
agregated_time = [17, 24, 32, 37, 44, 51, 59, 66, 73, 80, 87];

temp = tot_deaths(:,cst:4:end);
week_sim = [temp(:,2), temp(:,agregated_time(2:end))-temp(:,agregated_time(1:end-1))];
plot_all_and_mean(agregated_time, week_sim, agregated_time, weekly_deaths, 6, clrm, clrf, plot_title, ylab, false)

% Total tested
% All tested
ylab = 'Total number of tests';
plot_title = '$\mathrm{T_H + T_c}$';
temp = tot_tested(:,cst:end);
plot_all_and_mean(time(cst:end), temp, real_tested(2:end,1), real_tested(2:end,3)/n_county*n_new_rochelle, 7, clrm, clrf, plot_title, ylab, true)

% Confirmed positive vs. all tested
ylab = 'Fraction of positive tests';
plot_title = '$\mathrm{(T_p+T_p^f)/(T_H + T_c)}$';
temp = (tot_pos(:,cst:end) + tot_fpos(:,cst:end))./tot_tested(:,cst:end);
plot_all_and_mean(time(cst:end), temp, real_tested(2:end,1), real_tested(2:end,2)./real_tested(2:end,3), 8, clrm, clrf, plot_title, ylab, true)

% Underdetection - ratio of tests vs. ratio of weekly totals
weekly_tests = load('real_data/weekly_tests.txt');
ylab = 'Ratio of simulated and real number of weekly tests';
plot_title = '$\mathrm{(T_H + T_c)/T_{real}}$';
ntime = cst+weekly_tests(1:end,1)*4;

temp=zeros(size(tot_tested,1), size(ntime,1));
for i =1:size(tot_tested,1)
    sim_test = [tot_tested(i,ntime(1)),tot_tested(i,ntime(2:end))-tot_tested(i,ntime(1:end-1))];
    temp(i,:) = sim_test./(weekly_tests(1:end,end)/n_county*n_new_rochelle)';
end
plot_one(weekly_tests(:,1), temp, 9, clrm, clrf, plot_title, ylab, true)

% Total ratio per week
ylab = 'Ratio of simulated and real new weekly cases';
plot_title = '$\mathrm{(T_p+ T_p^f)/N_{real}}$';

agregated_time = [17, 24, 32, 37, 45, 51, 59, 66, 73, 80, 87, 94, 101, 107, 115, 121, 129, 136, 140];
rtemp = agregated_real(:,end);
rtemp = rtemp(~isnan(rtemp));

ntime = cst+agregated_time*4;

temp=zeros(size(tot_pos,1), size(ntime,2));
for i =1:size(tot_pos,1)
    sim_tpos = [tot_pos(i,ntime(1))+tot_fpos(i,ntime(1)),(tot_pos(i,ntime(2:end))+tot_fpos(i,ntime(2:end)))-(tot_pos(i,ntime(1:end-1))+tot_fpos(i,ntime(1:end-1)))];
    temp(i,:) = sim_tpos./rtemp';
end
plot_one(agregated_time, temp, 10, clrm, clrf, plot_title, ylab, false)

% Total deaths
ylab = 'Total number of deaths if treated';
plot_title = 'Treated $\mathrm{R_D}$';
temp = tested_deaths(:,cst:end);
plot_all_and_mean(time(cst:end), temp, real_deaths(:,1), real_deaths(:,2)/n_county*n_new_rochelle, 11, clrm, clrf, plot_title, ylab, false)

ylab = 'Total number of deaths if untreated';
plot_title = 'Untreated $\mathrm{R_D}$';
temp = not_tested_deaths(:,cst:end);
plot_one(time(cst:end), temp, 12, clrm, clrf, plot_title, ylab, false)
% plot_all_and_mean(time(cst:end), temp, real_deaths(:,1), real_deaths(:,2)/n_county*n_new_rochelle, 12, clrm, clrf, plot_title, ylab, false)

function plot_all_and_mean(time, y, t_real, y_real, i, clrm, clrf, plot_title, ylab, noMarkers)

    % Create figure
    figure1 = figure(i);

    % Create axes
    axes1 = axes('Parent',figure1);

    for i=1:size(y,1)
        plot(time, y(i,:), 'LineWidth', 2, 'Color', clrf)
        hold on
    end
    plot(time, mean(y,1), 'LineWidth', 2, 'Color', clrm)
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
    set(axes1,'FontSize',24,'TickLabelInterpreter','latex','XGrid','on','YGrid',...
        'on'); 
    
    % Ticks
    xlim([1,140])
    xticks([1 50 100 140])
    xticklabels({'March 3','April 22','June 11','July 21'})
        
    % Add events
    plot([10,10],ylim, '--', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
    plot([19,19],ylim, '-.', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
    plot([84,84],ylim, '--', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    plot([98, 98],ylim, '-.', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    plot([112, 112],ylim, ':', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])        
    
end

function plot_one(time, y, i, clrm, clrf, plot_title, ylab, noMarkers)

    % Create figure
    figure1 = figure(i);

    % Create axes
    axes1 = axes('Parent',figure1);

    for i=1:size(y,1)
        plot(time, y(i,:), 'LineWidth', 2, 'Color', clrf)
        hold on
    end
    plot(time, mean(y,1), 'v-', 'LineWidth', 2, 'Color', clrm)
 
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
    set(axes1,'FontSize',24,'TickLabelInterpreter','latex','XGrid','on','YGrid',...
        'on'); 
    
    % Ticks
    xticks([1 50 100 140])
    xticklabels({'March 3','April 22','June 11','July 21'})
    
    % Add events
    plot([10,10],ylim, '--', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
    plot([19,19],ylim, '-.', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
    plot([84,84],ylim, '--', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    plot([98, 98],ylim, '-.', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    plot([112, 112],ylim, ':', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])    
    
end