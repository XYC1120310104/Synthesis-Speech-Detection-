function [Pw,mu,sigma]=GMM_Learning(Train,Ngauss)
max_iter=20;
diff=10e-4;
[N, dim]=size(Train);
Pw=zeros(Ngauss,1);%����Ȩ��  
mu= zeros(Ngauss,dim);%����ÿ����˹����ľ�ֵ,ÿһ��Ϊһ����˹����  
sigma= zeros(dim,dim,Ngauss);%�����˹�����Э�������  
[cm,map]=KMInital(Train,Ngauss);
%k-means��ʼ��EM�㷨
mu=cm;
for i=1:Ngauss
    Pw(i)=length(find(map==mod(i,Ngauss)))/length(map);
    %sigma(:,:,i)=diag(std(Train(map==mod(i,Ngauss),:),0,1).^2);
    sigma(:,:,i)=cov(Train(map==mod(i,Ngauss),:))+eps*eye(dim);
end
P=zeros(Ngauss,N);
for j=1:max_iter
    %if Ngauss==1
     %   break;
    %end
    sigma_old=sigma;
    for i=1:Ngauss
         P(i,:)=Pw(i)*Gaussian(Train,mu(i,:),sigma(:,:,i));
    end
    s=sum(P,1);
    for k=1:N
        P(:,k)=P(:,k)/s(k);
    end
    %Ȩ���ع�
    Pw=sum(P,2)/N;
    %��ֵ�ع�
    for i=1:Ngauss
        sumi=0;
        for k=1:N
            sumi=sumi+P(i,k).*Train(k,:);
        end
        mu(i,:)=sumi./sum(P(i,:));
    end
    %Э�����ع�
    for i=1:Ngauss
        sumi=0;
        for k=1:N
            sumi=sumi+P(i,k).*(Train(k,:)-mu(i,:))'*(Train(k,:)-mu(i,:));
        end
        sigma(:,:,i)=sumi./sum(P(i,:))+eps*eye(dim);
    end
    
    if sum(sum(sum(abs(sigma_old-sigma))))<diff
        break;
    end
end