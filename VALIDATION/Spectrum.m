[xn fs]=audioread('15233129.WAV');
nf=1024; %number of point in DTFT
Y = fft(xn,nf);
f = fs/2*linspace(0,1,nf/2+1);
plot(f,abs(Y(1:nf/2+1)));
xlim([0,20000]);
ylim([0,10])

figure(2)
xn= xn(:,1);
dt = 1/fs;
t = 0:dt:(length(xn)*dt)-dt;
plot(t,xn); xlabel('Seconds'); ylabel('Amplitude');
    