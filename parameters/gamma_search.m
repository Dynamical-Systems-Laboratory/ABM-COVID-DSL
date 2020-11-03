function f_err = gamma_search(params)
    % Function for fitting a gamma distribution
    data = load('onset_to_hosp_linton.csv');
    gpdf = gampdf(data(:,1), params(1), params(2));
    
    f_err = max(abs(data(:,2)-gpdf));
    
end