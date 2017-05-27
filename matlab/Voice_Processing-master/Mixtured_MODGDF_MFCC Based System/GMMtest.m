clc;
clear all;
dbstop if error;
load 'D:\liblinear-2.1\matlab\Mixtured_feature\trainData.mat';
load 'D:\liblinear-2.1\matlab\Mixtured_feature\trainLabel.mat';
Ngauss=2;
[~,dim]=size(MSPC);
N=2;%category
do_training = true;
%% training
if (do_training)
Pw=zeros(Ngauss,N);
mu=zeros(Ngauss,dim,N);
sigma=zeros(dim,dim,Ngauss,N);
for i=1:N
    [Pw(:,i),mu(:,:,i),sigma(:,:,:,i)]=GMM_Learning(MSPC(label==mod(i,N),:),Ngauss);
end
save('D:\liblinear-2.1\matlab\Mixtured_feature\Mixtured_feature_GMMmodel.mat','Pw','mu','sigma');
end
%% testing
load('D:\liblinear-2.1\matlab\Mixtured_feature\Mixtured_feature_GMMmodel.mat');
load 'D:\liblinear-2.1\matlab\Mixtured_feature\testData.mat';
load 'D:\liblinear-2.1\matlab\Mixtured_feature\testLabel.mat';
[TestSamples,dim]=size(MSPC);
TestP=zeros(Ngauss,TestSamples,N);
tmp=zeros(N,TestSamples);
for i=1:N
    for j=1:Ngauss
        TestP(j,:,i)=Gaussian(MSPC,mu(j,:,i),sigma(:,:,j,i));
    end
    tmp(i,:)=sum(repmat(Pw(:,i),1,size(MSPC,1)).*TestP(:,:,i),1);%Ngauss,length(TestSamples)
end
%% evaluating
predict_label=zeros(size(label));
for k=1:size(MSPC,1)
    predict_label(k)=mod(find(tmp(:,k)==max(tmp(:,k))),2);
end

flex_predict_label = predict_label;
for i=2:length(label)-1
    flex_predict_label(i) =( (predict_label(i-1)+predict_label(i)+predict_label(i+1))/3 > 1/2); 
end

predict_label = flex_predict_label;

T_NUM=length(find(label==1));
F_NUM=length(label)-T_NUM;

TP=length(find(predict_label(1:T_NUM)==label(1:T_NUM)));
FP=length(find(predict_label(T_NUM+1:end)~=label(T_NUM+1:end)));

Precision=TP/(TP+FP)*100;
Recall=TP/T_NUM*100;
F_Measure=2/(100/Precision+100/Recall);

FAR = FP/F_NUM*100;
FRR = (T_NUM - TP)/T_NUM*100;

ex=length(find(predict_label==label));%tmp(:,k)
acc=ex*100/length(label);