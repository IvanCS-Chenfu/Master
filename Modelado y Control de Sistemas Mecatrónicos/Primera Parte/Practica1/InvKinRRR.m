function [Q] = InvKinRRR(P,m)

a1 = 20;
a2 = 15;
a3 = 10;

X = P(1);
Y = P(2);
phi = P(3);

PH = [X,Y];
R03 = rotZ(phi);
R03 = R03(1:3, 1:3);

PW = [PH(1) ; PH(2) ; 0] - a3*R03*[1 0 0]';

Xw = PW(1);
Yw = PW(2);



alpha = atan2(Yw,Xw);

R = sqrt(Yw^2 + Xw^2);

beta = acos((a1^2+a2^2-R^2)/(2*a1*a2));

gamma = acos((R^2+a1^2-a2^2)/(2*R*a1));



syms theta1 theta2 real
R01 = rotZ(theta1);
R01 = R01(1:3, 1:3);
R12 = rotZ(theta2);
R12 = R12(1:3, 1:3);

R02 = R01 * R12;

if m > 0
    
    theta1 = alpha - gamma;
    Q(1) = theta1;
    theta2 = pi - beta;
    Q(2) = theta2;

    R23 = eval(R02)'*R03;
    Q(3) = atan2(R23(2,1),R23(1,1));
    

else

    theta1 = alpha + gamma;
    Q(1) = theta1;
    theta2 = -(pi - beta);
    Q(2) = theta2;

    R23 = eval(R02)'*R03;
    Q(3) = atan2(R23(2,1),R23(1,1));

end


end


function RZ=rotZ(theta)

    s=sin(theta); c=cos(theta);
    if isnumeric(theta)
        if abs(c)<1e-10
            c=0;
        end
        if abs(s)<1e-10
             s=0;
        end
    end 
    
    RZ=[c,-s,0,0;s,c,0,0;0,0,1,0;0,0,0,1];
end
