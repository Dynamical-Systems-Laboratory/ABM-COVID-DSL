function f_err = weibull_search(params)
  
    data = load('hospitalization_to_death_linton.csv');
    wpdf = wblpdf(data(:,1), params(1), params(2));
    
    f_err = max(abs(data(:,2)-wpdf));
    
end