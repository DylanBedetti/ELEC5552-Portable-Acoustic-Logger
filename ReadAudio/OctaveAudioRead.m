pkg load signal

figure(1)

f='A.wav';              
[x,sr]=audioread(f) ;    
Ts=1/sr;                         
N=2^15;
x=x(1:N)';                   
time=Ts*(0:length(x)-1);       
figure(1)
magx=abs(fft(x));               
ssf=(0:N/2-1)/(Ts*N);             
plot(ssf,magx(1:N/2))
ylim([0,400])

Fn  = sr/2;                                 % Nyquist Frequency
Fco =   500;                                 % Passband (Cutoff) Frequency
Fsb =   0; 
[b1,b2]=butter(4,0.02,'high');
    
clean=filter(b1,b2,x);
audiowrite ("clean.wav", clean, sr);
Fn