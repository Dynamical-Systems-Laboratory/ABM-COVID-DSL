function f_err = lognormal_search(params)
 
    data = load('onset_to_death_linton.csv');
    npdf = lognpdf(data(:,1), params(1), params(2));
    
    f_err = norm(abs(data(:,2)-npdf));
    
end