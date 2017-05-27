function feature = MFCC_feature(x,fs,segment)

do_diff  = false;
do_seg   = true;
FrameLen = 50;%ms
FrameInc = 20;%ms

FrameLen = round((FrameLen/1000)*fs);
FrameInc = round((FrameInc/1000)*fs);

bank     = melbankm(24,FrameLen,fs,0,0.5,'t');%Mel�˲����Ľ���Ϊ24��fft�任�ĳ���Ϊ400������Ƶ��Ϊ8000Hz

% ��һ��mel�˲�����ϵ��
bank     = full(bank);
bank     = bank/max(bank(:));

% DCTϵ��,13*24
dctnum   = 13;
for k=1:dctnum
    n = 0:23;
    dctcoef(k,:) = cos((2*n+1)*k*pi/(2*24));
end
% ��һ��������������
w = 1 + 6 * sin(pi * [1:dctnum] ./ dctnum);
w = w/max(w);

% Ԥ�����˲�����Ԥ����ϵ��0.9375
xx = double(x);
xx = filter([1 -0.9375],1,xx);
% �����źŷ�֡����֡��ÿһ��Ϊһ֡
xx = enframe(xx,FrameLen,FrameInc);%��xx��256���Ϊһ֡��֡��Ϊ80/256
% % ����ÿ֡��MFCC����
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
    %��ȡһ�ײ��ϵ��
    dtm = zeros(size(m));
    for l=3:size(m,1)-2
        dtm(l,:)=-2*m(l-2,:)-m(l-1,:)+m(l+1,:)+2*m(l+2,:);
    end
    dtm=dtm/3;
    %��ȡ���ײ��ϵ��
    dtmm=zeros(size(dtm));
    for p=3:size(dtm,1)-2
        dtmm(p,:)=-2*dtm(p-2,:)-dtm(p-1,:)+dtm(p+1,:)+2*dtm(p+2,:);
    end
    dtmm=dtmm/3;
    %�ϲ�mfcc������һ�ײ��mfcc����
    m=[m dtm dtmm];
end
feature=m;
% %% �����ʱ����
% amp = sum(xx.^2, 2);
% 
% %��ȡһ�ײ��ϵ��
% dta=zeros(size(amp));
% for l=3:size(amp,1)-2
%     dta(l,:)=-2*amp(l-2,:)-amp(l-1,:)+amp(l+1,:)+2*amp(l+2,:);
% end
% dta=dta/3;
% %��ȡ���ײ��ϵ��
% dtaa=zeros(size(dta));
% for p=3:size(dta,1)-2
%     dtaa(p,:)=-2*dta(p-2,:)-dta(p-1,:)+dta(p+1,:)+2*dta(p+2,:);
% end
% dtaa=dtaa/3;
% dta=[amp dta dtaa];
% 
% %% ������
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
%% ��ͼ
% feature=feature(:,end);
%  xi=[0:0.05:1];
% f=ksdensity(feature,xi);
% plot(xi,f*0.05,'.-');



