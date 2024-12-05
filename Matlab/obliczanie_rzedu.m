load matlab.mat;
z = iddata(output_data-35.54,input_data*99.99,0.01,'Tstart',0);
na = 1:5;
nc = 1:5;
nk = 0:3;
models = cell(1,18);
ct = 1;
for i = 1:3
    na_ = na(i);
    nb_ = na_;
    for j = 1:2
        nc_ = nc(j);
        for k = 1:3
            nk_ = nk(k); 
            models{ct} = armax(z,[na_ nb_ nc_ nk_]);
            ct = ct+1;
        end
    end
end
models = stack(1,models{:});
time = (0:1340-1) * 0.01;
compare(z,models)
figure;
subplot(2,1,1);
plot(time,input_data*99.99);
title('Dane wejscowe');
xlabel('czas [s]');
ylabel('Wartosc zadanego sygnalu');
subplot(2,1,2);
plot(time,output_data-35.54);
title('Dane wyjsciowe');
xlabel('czas [s]');
ylabel('Temperatura [°C]');