clc;
clear all;
load EmuSamples.mat;
%load Samples.mat;
%Train=Train1;
%Test=Test1;
Ngauss=2;
[Pw1,mu1,sigma1]=GMM_Learning(Train1,Ngauss);
[Pw2,mu2,sigma2]=GMM_Learning(Train2,Ngauss);
for i=1:Ngauss
    TestP1_1(i,:)=Gaussian(Test1,mu1(i,:),sigma1(:,:,i));
    TestP2_1(i,:)=Gaussian(Test1,mu2(i,:),sigma2(:,:,i));
    TestP1_2(i,:)=Gaussian(Test2,mu1(i,:),sigma1(:,:,i));
    TestP2_2(i,:)=Gaussian(Test2,mu2(i,:),sigma2(:,:,i));
end
tmp1_1=sum(repmat(Pw1,1,size(Test1,1)).*TestP1_1);
tmp2_1=sum(repmat(Pw2,1,size(Test1,1)).*TestP2_1);
tmp1_2=sum(repmat(Pw1,1,size(Test2,1)).*TestP1_2);
tmp2_2=sum(repmat(Pw2,1,size(Test2,1)).*TestP2_2);
T1=length(find(tmp1_1>tmp2_1));
acc1=100*T1/length(Test1);
T2=length(find(tmp1_2<tmp2_2));
acc2=100*T2/length(Test2);