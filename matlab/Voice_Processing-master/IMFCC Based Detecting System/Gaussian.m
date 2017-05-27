function p=Gaussian(x,mu,sigma)
[N,dim]=size(x);
p=zeros(1,N);
% p=mvnpdf(x,mu,sigma+eps*eye(dim));
% p=p';
 for i=1:N  
     %p(i)= 1/(2*pi*abs(det(sigma)))^(length(mu)/2)*exp(-0.5*(x(:,i)-mu)'*inv(sigma)*(x(:,i)-mu));  
     %p(i) = exp(-0.5*(x(i,:)-mu)*(sigma\(x(i,:)-mu)'))/((2*pi)^(dim/2)*sqrt(abs(det(sigma))));
     p(i) = exp(-0.5*((x(i,:)-mu)/sigma)*(x(i,:)-mu)')/((2*pi)^(dim/2)*sqrt(abs(det(sigma))));
 end  