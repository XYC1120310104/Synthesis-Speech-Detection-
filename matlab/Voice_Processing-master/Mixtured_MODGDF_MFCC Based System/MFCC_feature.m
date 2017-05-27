function feature = MFCC_feature(x,fs,segment)

do_diff  = false;
do_seg   = true;
FrameLen = 50;%ms
FrameInc = 20;%ms

FrameLen = round((FrameLen/1000)*fs);
FrameInc = round((FrameInc/1000)*fs);

bank     = melbankm(24,FrameLen,fs,0,0.5,'t');%Mel滤波器的阶数为24，fft变换的长度为400，采样频率为8000Hz

% 归一化mel滤波器组系数
bank     = full(bank);
bank     = bank/max(bank(:));

% DCT系数,13*24
dctnum   = 13;
for k=1:dctnum
    n = 0:23;
    dctcoef(k,:) = cos((2*n+1)*k*pi/(2*24));
end
% 归一化倒谱提升窗口
w = 1 + 6 * sin(pi * [1:dctnum] ./ dctnum);
w = w/max(w);

% 预加重滤波器，预加重系数0.9375
xx = double(x);
xx = filter([1 -0.9375],1,xx);
% 语音信号分帧，分帧后每一行为一帧
xx = enframe(xx,FrameLen,FrameInc);%对xx，256点分为一帧，帧移为80/256
% % 计算每帧的MFCC参数
m=[];
for i = 1:size(xx,1)
    y = xx(i,:);
    s = y' .* hamming(FrameLen);
    t = abs(fft(s));
    t = t.^2;
    t=t(1:FrameLen/2+1);
    
    c1 = dctcoef * log(bank * t + eps);
    c2 = c1.*w';
    c2 = c2(2:dctnum,:);
    m(i,:) = c2';
end

if (do_diff)
    %求取一阶差分系数
    dtm = zeros(size(m));
    for l=3:size(m,1)-2
        dtm(l,:)=-2*m(l-2,:)-m(l-1,:)+m(l+1,:)+2*m(l+2,:);
    end
    dtm=dtm/3;
    %求取二阶差分系数
    dtmm=zeros(size(dtm));
    for p=3:size(dtm,1)-2
        dtmm(p,:)=-2*dtm(p-2,:)-dtm(p-1,:)+dtm(p+1,:)+2*dtm(p+2,:);
    end
    dtmm=dtmm/3;
    %合并mfcc参数和一阶差分mfcc参数
    m=[m dtm dtmm];
end
feature=m;
% %% 计算短时能量
% amp = sum(xx.^2, 2);
% 
% %求取一阶差分系数
% dta=zeros(size(amp));
% for l=3:size(amp,1)-2
%     dta(l,:)=-2*amp(l-2,:)-amp(l-1,:)+amp(l+1,:)+2*amp(l+2,:);
% end
% dta=dta/3;
% %求取二阶差分系数
% dtaa=zeros(size(dta));
% for p=3:size(dta,1)-2
%     dtaa(p,:)=-2*dta(p-2,:)-dta(p-1,:)+dta(p+1,:)+2*dta(p+2,:);
% end
% dtaa=dtaa/3;
% dta=[amp dta dtaa];
% 
% %% 过零率
% tmp1 = enframe(x(1:end-1), FrameLen, FrameInc);
% tmp2 = enframe(x(2:end) , FrameLen, FrameInc);
% signs = (tmp1.*tmp2)<0;
% diffs = (tmp1 -tmp2)>0.02;
% zcr = sum(signs.*diffs, 2);
% % root mean quare
% rms=sqrt(sum(xx.^2,2));
%%
% feature =[m,dta,zcr,rms];
% feature=mapminmax(feature,0,1);
if (do_seg)
    seg_inc = segment/2;
    [n1,d] = size(feature);
    nsize = fix((n1-segment)/seg_inc);
    final_F=zeros(nsize,d);
% HZCRR=[];
% LSTER=[];
% tempFREn = sum( abs(xx),2 )/FrameLen;
% LEnFR=[];
    for i=1:1:nsize
      final_F(i,:)=mean(feature((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:),1);
%     avgzcr(i)=mean(zcr((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:));
%     avSTE=final_F(i,1);
%     HZCRR=[HZCRR,sum( sign( zcr((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:)-1.5*avgzcr(i) )+1 )/2/segment];
%     LSTER=[LSTER,1/(2*segment)*(sum(sign(avSTE-amp((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:)))+1)];  
%     avgEn=mean(tempFREn((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:),1);
%     LEnFR =[LEnFR, sum(find(tempFREn((i-1)*seg_inc+1:(i-1)*seg_inc+segment,:) < avgEn * 0.3))/nsize];
    end
    feature = final_F;
end
% 
% feature=[final_F,HZCRR',LSTER',LEnFR'];
% feature=feature(5:size(feature,1)-4,:);
% [x_1,y_1]=find(isnan(feature));
% feature(x_1,y_1)=0;

% feature=[amp zcr rms LSTER HZCRR LEnFR]
%% 画图
% feature=feature(:,end);
%  xi=[0:0.05:1];
% f=ksdensity(feature,xi);
% plot(xi,f*0.05,'.-');



