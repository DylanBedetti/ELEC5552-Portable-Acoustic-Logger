[xn fs]=audioread('A2.wav');
nf=5*4096; %number of point in DTFT
Y = fft(xn,nf);
f = fs/2*linspace(0,1,nf/2+1);
plot(f,abs(Y(1:nf/2+1)));
xlim([0,800])

figure(2);
f='A2.wav';              
[x,sr]=audioread(f) ;    
Ts=1/sr;                         
N=2^15;
x=x(1:N)';                   
time=Ts*(0:length(x)-1);       
figure(1)
magx=abs(fft(x));               
ssf=(0:N/2-1)/(Ts*N);             
plot(ssf,magx(1:N/2))


yhp = falter('hp',x,'fs',sr,'fc',750)
[b1,b2]=butter(4,[0,0.12],'stop');
    
clean=filter(b1,b2,clean);
adiowrite("cleaned",clean,fs);