%% Post-processing 
clear
close all

% Data directory and the mat file
data_dir = 'output/base_case/';
mfile = 'set_27';
load(mfile)

% Plot settings
clrf = [0.7, 0.7, 0.7];
clrm = [21/255, 23/255, 150/255];

% County size
n_county = 967506;
% New Rochelle size
n_new_rochelle = 79205;

% Collection start in steps
cst = 41;

% Total
ylab = 'Probability density';
plot_title = 'Final total cases: $\mathrm{T_p+ T_p^f}$';
temp = tot_pos(:,cst:end)+tot_fpos(:,cst:end);
plot_distribution(temp, 1, clrm, plot_title, ylab)

% Total deaths
ylab = 'Probability density';
plot_title = 'Final total deaths: $\mathrm{R_D}$';
temp = tot_deaths(:,cst:end);
plot_distribution(temp, 2, clrm, plot_title, ylab)

% Total tested
ylab = 'Probability density';
plot_title = 'Final total tested: $\mathrm{T_H + T_c}$';
temp = tot_tested(:,cst:end);
plot_distribution(temp, 3, clrm, plot_title, ylab)

function plot_distribution(y, i, clrm, plot_title, ylab)

    % Create figure
    figure1 = figure(i);

    % Create axes
    axes1 = axes('Parent',figure1);

    final_values = zeros(size(y,1),1);
    for i=1:size(y,1)
        final_values(i) = y(i,end);
    end
    
    histogram(final_values,20,'Normalization','pdf','FaceColor',[.7 .7 .7]);
    hold on
    
    pd = fitdist(final_values, 'Normal');
    % Confidence intervals
    ci = paramci(pd,0.25)
    xgrid = linspace(min(final_values), max(final_values), 100);
    pdf_final = pdf(pd, xgrid);
    plot(xgrid, pdf_final, 'LineWidth', 2, 'Color', clrm)
           
    hold off
    
    % Create ylabel
    ylabel(ylab,'Interpreter','latex');

    % Create xlabel
    xlabel('Cumulative value','Interpreter','latex');

    % Create title
    title(plot_title,'Interpreter','latex');

    % Uncomment the following line to preserve the Y-limits of the axes
    % ylim(axes1,[0 5]);
    box(axes1,'on');
    % Set the remaining axes properties
    set(axes1,'FontSize',20,'TickLabelInterpreter','latex','XGrid','on','YGrid',...
        'on'); 
         
end