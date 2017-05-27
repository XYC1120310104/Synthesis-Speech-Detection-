clc;
clear all;
%load EmuSamples.mat;
load Samples.mat;
Ngauss=2;
N=10;
ex=0;
%%training
for i=1:N
    [Pw(:,i),mu(:,:,i),sigma(:,:,:,i)]=GMM_Learning(TrainSamples(TrainLabels==mod(i,N),:),Ngauss);
end
%%testing TestP[Ngauss,length(TestSamples),N]
for i=1:N
    for j=1:Ngauss
        TestP(j,:,i)=Gaussian(TestSamples,mu(j,:,i),sigma(:,:,j,i));
    end
    tmp(i,:)=sum(repmat(Pw(:,i),1,size(TestSamples,1)).*TestP(:,:,i),1);%Ngauss,length(TestSamples)
end
%%evaluating
for k=1:size(TestSamples,1)
    if (find(tmp(:,k)==max(tmp(:,k)))==TestLabels(k))%tmp(:,k)
        ex=ex+1;
    end
end
acc=ex*100/length(TestLabels);