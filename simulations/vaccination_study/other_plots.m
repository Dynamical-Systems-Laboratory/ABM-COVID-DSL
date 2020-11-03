%% Post-processing 
clear
close all

% Data directory and the mat file
mfile = 'base_case';

% Plot settings
clrf = [0.7, 0.7, 0.7];
clrm = [0.64, 0.08, 0.18];

% County size
n_county = 967506;
% New Rochelle size
n_new_rochelle = 79205;

% Plot all realizations and the mean
load(mfile)

% Treatment 
% Home isolated
ylab = 'Home isolated';
plot_title = '$\mathrm{I_H}$';
plot_all_and_mean(time, tot_ih, 1, clrm, clrf, plot_title, ylab)

% Hospitalized
ylab = 'Hospitalized';
plot_title = '$\mathrm{H_N}$';
plot_all_and_mean(time, tot_hn, 2, clrm, clrf, plot_title, ylab)

% Hospitalized in an ICU
ylab = 'Hospitalized in ICU';
plot_title = '$\mathrm{H_{ICU}}$';
plot_all_and_mean(time, tot_icu, 3, clrm, clrf, plot_title, ylab)

% Testing
clrm = [0.0, 0.00, 0.0];
% All tested
ylab = 'Number of tests';
plot_title = '$\mathrm{T_H + T_c}$';
plot_all_and_mean(time, tot_tested, 4, clrm, clrf, plot_title, ylab)

% False positive
ylab = 'Number of false positive results';
plot_title = '$\mathrm{T_p^f}$';
plot_all_and_mean(time, tot_fpos, 5, clrm, clrf, plot_title, ylab)

% False negative
ylab = 'Number of false negative results';
plot_title = '$\mathrm{T_n^f}$';
plot_all_and_mean(time, tot_fneg, 6, clrm, clrf, plot_title, ylab)

% False positive
ylab = 'Number of negative results';
plot_title = '$\mathrm{T_n}$';
plot_all_and_mean(time, tot_neg, 7, clrm, clrf, plot_title, ylab)

function plot_all_and_mean(time, y, i, clrm, clrf, plot_title, ylab)

    % Create figure
    figure1 = figure(i);

    % Create axes
    axes1 = axes('Parent',figure1);

    for i=1:size(y,1)
        plot(time, y(i,:), 'LineWidth', 2, 'Color', clrf)
        hold on
    end
    plot(time, mean(y,1), 'LineWidth', 2, 'Color', clrm)
   
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
    set(axes1,'FontSize',20,'TickLabelInterpreter','latex','XGrid','on','YGrid',...
        'on'); 
    
    % Ticks
    xticks([1 50 100 140])
    xticklabels({'March 3','April 22','June 11','July 21'})
    xlim([1,140])
    
    % Add events
    plot([10,10],ylim, '--', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
    plot([19,19],ylim, '-.', 'LineWidth', 2, 'Color', [188/255, 19/255, 30/255])
    plot([84,84],ylim, '--', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    plot([98, 98],ylim, '-.', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])
    plot([112, 112],ylim, ':', 'LineWidth', 2, 'Color', [123/255, 33/255, 157/255])    
   
end