function RX=rotX(theta)

s=sin(theta); c=cos(theta);
if isnumeric(theta)
    if abs(c)<1e-10
        c=0;
    end
    if abs(s)<1e-10
        s=0;
    end
end 
RX=[1,0,0,0;0,c,-s,0;0,s,c,0;0,0,0,1];