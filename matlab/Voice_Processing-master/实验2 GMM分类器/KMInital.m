function  [C, PL]=KMInital(X,N)
k=1;
diff=ones(N,1)*inf;
C=X(1:N,:);
C2=zeros(N,size(X,2));
PL=zeros(size(X,1),1);
while (max(diff)>eps)
    for j=1:size(X,1)
        for i=1:N
            diff(i,1)=sqrt((C(i,:)-X(j,:))*(C(i,:)-X(j,:))');
        end
        PL(j,1)=mod(find(diff==min(diff),1),N);
    end
    for i=1:N
        C2(i,:)=mean(X(find(PL==mod(i,N)),:));
        diff(i,1)=sqrt((C(i,:)-C2(i,:))*(C(i,:)-C2(i,:))');
    end
    C=C2;
    k=k+1;
    if k>100
        break
    end
end