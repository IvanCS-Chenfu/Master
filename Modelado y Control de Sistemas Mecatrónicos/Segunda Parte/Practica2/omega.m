function D = omega(U)
% omega: función que calcula U por su pseudo inversa. 
%
D=U*inv(U'*U)*U'-eye(max(size(U)));
end

