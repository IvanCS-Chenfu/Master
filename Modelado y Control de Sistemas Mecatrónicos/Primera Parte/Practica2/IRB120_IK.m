function [Q]=IRB120_IK(T,a,d,T03s)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Geometric Inverse Kinematics method for IRB120 manipulator.
%
% function [Q]=IRB120_IK(p)
%
% params:
%               T  - Homogeneus transformation matrix of the target pose
%               a   - a distance in DH1
%               d   - d distance in DH1
%
%  output:
%               Q  - joint configuration in the target pose
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

syms q1 q2 q3

Q=zeros(1,6);

d_1=d(1);
a_2=a(2);
a2_2=a_2^2;
a_3=a(3);
d_4=d(4);

% Kinematic decoupling
Z6=[0 0 1]';
R06=T(1:3,1:3);
P04=T(1:3,4)-d(6)*R06*Z6;

x=P04(1);
y=P04(2);
z=P04(3);

Lxy=sqrt(x^2+y^2);

if Lxy<(a_2+d_4) 
    L2=(Lxy)^2+(z-d_1)^2;
    L=sqrt(L2);
    d2=a_3^2+d_4^2;
    d=sqrt(d2);
    alpha=atan2(z-d_1,Lxy);
    beta=acos((d2-a2_2-L2)/(-2*a_2*L));
    Q(2)=-beta-alpha;

    phi=atan2(d_4,a_3);
    gamma=acos((L2-a2_2-d2)/(-2*a_2*d));
    Q(3)=pi-phi-gamma;
    
    %gamma=acos((L2-a2_2-d2)/(-2*a_2*d));
    %Q(3)=pi-gamma-phi;
    
    Q(1)=atan2(y,x);
    
    % Orientation
    T03=double(subs(T03s,{q1,q2,q3},{Q(1),Q(2),Q(3)}));
    R36=(T03(1:3,1:3))'*R06;
    s5=sqrt(R36(1,3)^2+R36(2,3)^2);
    if s5 < 1e-10
        disp('Degenerate wrist configuration');
        Q(4)=0;
        Q(5)=0;
        Q(6)=0;
    else
        c5=R36(3,3);
        Q(5)=atan2(s5,c5);
        Q(4)=atan2(-R36(2,3)/s5,-R36(1,3)/s5);
        Q(6)=atan2(-R36(3,2)/s5,R36(3,1)/s5);
    end
else
    disp('Out of workspace')
    Q=[inf, inf, inf, inf, inf, inf];
end