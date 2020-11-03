function plot_mean_and_var(x, y, i, clrB, clrF, plot_title, ylab)
   
    % Create figure
    figure1 = figure(i);

    % Create axes
    axes1 = axes('Parent',figure1);

    % Final
    dy = std(y)';
    y = mean(y)';
    x = x';
    
    fill([x;flipud(x)],[y-dy;flipud(y+dy)], clrB,'linestyle','none');
    hold on
    plot(x,y, 'LineWidth',2, 'Color', clrF, 'MarkerSize',  10)

    hold off
    
    % Create ylabel
    ylabel(ylab,'Interpreter','latex');

    % Create xlabel
    xlabel('Time, days','Interpreter','latex');

    % Create title
    title(plot_title,'Interpreter','latex');

    % Uncomment the following line to preserve the Y-limits of the axes
    % ylim(axes1,[0 5]);
    box(axes1,'on');
    % Set the remaining axes properties
    set(axes1,'FontSize',20,'TickLabelInterpreter','latex','XGrid','on','YGrid',...
        'on');  
end