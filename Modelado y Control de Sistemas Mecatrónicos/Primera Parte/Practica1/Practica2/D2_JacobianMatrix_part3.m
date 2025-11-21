%% Subject: Modelado y Control de Sistemas Mecatrónicos y Robots. 2025-26.
%% Deliverable 2: Jacobian Matrix

clearvars;
close all;

%% IRB120 DH1 parameters
syms alpha a theta d real
T=despZ(d)*rotZ(theta)*despX(a)*rotX(alpha);

% Standard Denavit-Hartemberg parameters (DH1)
di=[0.290 0 0 0.302 0 0.072];
ai=[0 0.270 0.070 0 0 0];
alphai=[-pi/2 0 -pi/2 pi/2 -pi/2 0];
syms q1 q2 q3 q4 q5 q6 real
qi=[q1 q2 q3 q4 q5 q6];

T01=subs(T,{a,alpha,d,theta},{ai(1),alphai(1),di(1),qi(1)});
T12=subs(T,{a,alpha,d,theta},{ai(2),alphai(2),di(2),qi(2)});
T23=subs(T,{a,alpha,d,theta},{ai(3),alphai(3),di(3),qi(3)});
T34=subs(T,{a,alpha,d,theta},{ai(4),alphai(4),di(4),qi(4)});
T45=subs(T,{a,alpha,d,theta},{ai(5),alphai(5),di(5),qi(5)});
T56=subs(T,{a,alpha,d,theta},{ai(6),alphai(6),di(6),qi(6)});

%T01 computed before.
T02=simplify(T01*T12);
T03=simplify(T02*T23);
T04=simplify(T03*T34);
T05=simplify(T04*T45);
T06=simplify(T05*T56);

% IRB120 symbolic Jacobian
%joint vectors (zi)
zL=[0 0 1]';
z0=zL;
%%%%% PUT YOUR CODE HERE %%%%%%
z1 = T01(1:3,1:3) * zL;
z2 = T02(1:3,1:3) * zL;
z3 = T03(1:3,1:3) * zL;
z4 = T04(1:3,1:3) * zL;
z5 = T05(1:3,1:3) * zL;
%position vectors (pi); they are the position part of the homogeneus
%transformation matrices (position of the origin of the reference frame)
p0=[0 0 0]';
%%%%% PUT YOUR CODE HERE %%%%%%
p1 = T01(1:3, 4);
p2 = T02(1:3, 4);
p3 = T03(1:3, 4);
p4 = T04(1:3, 4);
p5 = T05(1:3, 4);
p6 = T06(1:3, 4);

%Columns according to the algorithm (J=[JP;JO]); we are left with the three
%first elements of each pi vector
J1=simplify([cross(z0,(p6-p0));z0]);
%%%%% PUT YOUR CODE HERE %%%%%%
J2=simplify([cross(z1,(p6-p1));z1]);
J3=simplify([cross(z2,(p6-p2));z2]);
J4=simplify([cross(z3,(p6-p3));z3]);
J5=simplify([cross(z4,(p6-p4));z4]);
J6=simplify([cross(z5,(p6-p5));z5]);

JPO=simplify([J1 J2 J3 J4 J5 J6]);

%% Part 3: trajectory between A and B
% Point A
Ta=[1 0 0 -0.3; 0 -1 0 0.302; 0 0 -1 0.558; 0 0 0 1];
% Point B
Tb=[1 0 0 0; 0 -1 0 0.302; 0 0 -1 0.358; 0 0 0 1];

% Generate trajectory between Ta and Tb
initTime = 0;
% End-effector velocity
toolSpeed=0.1; % m/s
distance = norm(tform2trvec(Ta)-tform2trvec(Tb));
finalTime = (distance/toolSpeed)-initTime;
timeStep=0.1; %s
trajTimePoints = initTime:timeStep:finalTime;
trajtInterval=[trajTimePoints(1); trajTimePoints(end)];
[T, Vel] = transformtraj(Ta,Tb,trajtInterval,trajTimePoints); 
% tool speed in cartesian coordiantes
ve=[Vel(4:6,:); Vel(1:3,:)]; 

% Trajectory in joint space
out_ws=0; % out of workspace
for k=1:length(trajTimePoints)
    q(k,:)=IRB120_IK(T(:,:,k),ai,di,T03);
    if isinf(q(k,:)) % if a position out of range
        out_ws=1; 
    end
end

qd=[];
dj=[];
if out_ws==1
    disp('There is a position out of workspace');
else
    %Singularities?: computing determinant of the Jacobian Matrix for every pose
    for k=1:length(trajTimePoints)
        %%%%% PUT YOUR CODE HERE %%%%%%
        Jq=subs(JPO,{q1,q2,q3,q4,q5,q6},{q(k,1),q(k,2),q(k,3),q(k,4),q(k,5),q(k,6)});
        qd(:,k) = Jq\ve(:,k);
        dj(:,k) = det(Jq);
        % Joint velocities: qd=J^(-1)*v

    end
    
    % Representation
    % Load robot ABB IRB120 model
    [IRB120, IRB120Data] = loadrobot('abbIrb120','DataFormat','row','Gravity',[0 0 -9.81]);
    q_off=[0 -pi/2 0 0 0 0]; % offset for the joint values in IRB120 representation model
    % Robot representation figure
    f1=figure(1);
    set(f1,'Name','IRB120 Manipulator');
    InitialConfig=q(1,:)-q_off;
    show(IRB120, InitialConfig, 'PreservePlot', false, 'Frames', 'off'); 
    hold on
    axis([-0.5,0.7,-0.5,0.7,0,1]); % set appriate axis dimensions
    
    % Represent manipulator configurations
    for k=1:length(trajTimePoints)
        % Robot poses representation
        qm=q(k,:)-q_off; % computing current joint values
        show(IRB120, qm, 'PreservePlot', false, 'Frames', 'off'); 
        title(['q= [', num2str(qm(1)), ' ', num2str(qm(2)), ' ', num2str(qm(3)), ' ', num2str(qm(4)), ' ', num2str(qm(5)), ' ', ...
            num2str(qm(6)), '];     x= ', num2str(T(1,4)), ' y= ', num2str(T(2,4)), ' z= ', num2str(T(3,4))]);  
        plot3(T(1,4,k), T(2,4,k), T(3,4,k), 'b*', 'MarkerSize', 20)  % task space marker  
        drawnow
    end
    
    % Representation of joint positions and joint velocities over time
    figure;
    rg=180/pi; 
    subplot(2,1,2);
    plot(trajTimePoints,q*rg); 
    xlabel('TRAJECTORY'); ylabel('degrees');
    legend('q_1','q_2','q_3','q_4','q_5','q_6'); title('Joint positions'); grid;
    
    subplot(2,1,1);
    plot(trajTimePoints,qd*rg); 
    xlabel('TRAJECTORY'); ylabel('degrees/s');
    legend('dq_1','dq_2','dq_3','dq_4','dq_5','dq_6'); title('Joint velocities'); grid;
    
    %% Part 4: singularities?
    figure;
    plot(trajTimePoints,dj); grid;
    title('Jacobian determinant');
end
