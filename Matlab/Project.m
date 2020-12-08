clc
clear all
close all
%% Initial setting
% Empty all comunication
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end
% Comunication setting
COM_X = serial('com14');
COM_Y=serial('com9');
COM_ZC=serial('com13');
set(COM_X, 'BaudRate', 115200);
set(COM_Y, 'BaudRate', 115200);
set(COM_ZC, 'BaudRate', 115200);
% pause(3); %wait for COM opened
fopen(COM_X);
fopen(COM_Y);
fopen(COM_ZC);
% Camera setting
playercam=webcam(3);
playercam.Resolution='1280x960';
pause(2);
robotcam=webcam(1);
robotcam.Resolution='1280x960';
% Game setting
score_condition=20; %pixel,if centroid difference less than this condition then score 1 point
ratio=2.75; %55cm/20cm
sizewidth=2000; %pixel
widthlength=55; %cm x434y102, 109
heightlength=widthlength/ratio; %cm
sizeheight=round(sizewidth/ratio);
backgroundcolor=1; %white background
linewidth=5; %odd number
outlinewidth=11; %odd number
linecolor=0.5;
countdown=5;
% Real length(cm) per pixel
a=17.5;
b=12.5;
c=2.6;
cmperpixel=0.02392;
angleperstep=0.1125;
cmperstepX=0.0005;
cmperstepY=0.0025;
% Setting common window
beginwindow=resizewindow(imread('beginwindow.jpg'),sizewidth,sizeheight);
calculatingwindow=resizewindow(imread('calculatingwindow.jpg'),sizewidth,sizeheight);
sizewindow=size(calculatingwindow);
for i=1:sizewindow(1)
    for j=1:sizewindow(2)
        if calculatingwindow(i,j,1)>0.9
            calculatingwindow(i,j,1)=0;
            calculatingwindow(i,j,2)=1;
            calculatingwindow(i,j,3)=0;
        end
    end
end
wingamewindow=resizewindow(imread('wingame.png'),sizewidth,sizeheight);
losegamewindow=resizewindow(imread('losegame.png'),sizewidth,sizeheight);
tiegamewindow=resizewindow(imread('tiegame.png'),sizewidth,sizeheight);
resetwindow=resizewindow(imread('resetgame.png'),sizewidth,sizeheight);
% Import object jpg
im_object=struct();
im_object(1).I=imread('hex.png');
im_object(2).I=imread('circle.png');
im_object(3).I=imread('heart.png');
im_object(4).I=imread('arrow.png');
im_object(5).I=imread('rectangle.png');
im_object(6).I=imread('ten.png');
loop=1;
%% Maincode
while loop==1
    % Get setting data
    % Set window to fullscreen
    figure('units','normalized','outerposition',[0 0 1 1])
    set(gcf,"WindowState",'maximized');
    set(gcf,'MenuBar','none')
    set(gca,'Position',[0 0 1 1 ])
    set(gcf,'NumberTitle','off');
    imshow(beginwindow);
    pause(2);
    while (get(COM_X,'BytesAvailable')==0)
    end
    settingdata=fscanf(COM_X);
    data=split(settingdata);
    gridnum=str2double(data(2));
    gridsize=round(sizeheight/gridnum);
    level=str2double(data(1)); %set robot arm speed
    numobject=str2double(data(3));
    typerobot=str2double(data(4));
    typeplayer=str2double(data(5));
    robotobject=im_object(typerobot).I;
    playerobject=im_object(typeplayer).I;
    speed=num2str(level);
    fprintf(COM_Y,speed);
    pause(0.2);
    fprintf(COM_ZC,speed);
    %% Create window
    % Setting display window
    window=creategridwindow(ratio,sizewidth,backgroundcolor,linewidth,outlinewidth,linecolor,gridnum,level);
    % Create random object position
    array=(1:gridnum^2);
    randomarray(1:numobject)=0;
    for i=1:numobject
        randomarray(i)=array(randi(numel(array)));
        array=setdiff(array,randomarray(i));
    end
    % Struct with robot arm position and player position
    objectposition=position(randomarray,numobject,gridnum,gridsize,sizewidth,sizeheight);
    % Create arm play region
    window=plotobject(window,"robot",objectposition,playerobject,gridsize,gridnum);
    % Create player play region
    window=plotobject(window,"player",objectposition,robotobject,gridsize,gridnum);
    newwindow=zeros(1100,2000);
    sizewindow=size(newwindow);
    newwindow(1:sizewindow(1)-726,:)=1;
    newwindow(sizewindow(1)-726:sizewindow(1),:)=window;
    % Create ready window with countdown
    readywindow=createwindowstruct(newwindow,countdown);
    %% Start game
    imshow(beginwindow);
    while (get(COM_X,'BytesAvailable')==0)
    end
    datascan=fscanf(COM_X);
    if datascan=="Hard reset"
        imshow(resetwindow);
        pause(2);
        continue
    else %datascan=="Start"
        fprintf(COM_X,"Open flash 1");
        pause(1);
        %% Calculate moving track process
        % Find robot object
        Irobotobject=snapshot(robotcam);
        Irobotobject=imrotate(Irobotobject,270);
        Irobotobject=imcrop(Irobotobject,[110 450 490 780]);
        robotobject=findrobotobject(Irobotobject); % Return struct with theta, centroid
        % Real catch position
        size_robot_real=size(robotobject);
        catch_xy=zeros(size_robot_real(2),2);
        for i=1:size_robot_real(2)
            catch_xy(i,:)=[robotobject(i).centroid];
        end
        real_catch_xy=round(catch_xy.*cmperpixel,2);
        % Absolute position of object relative to origin of arm robot
        abs_catch_xy=[real_catch_xy(:,1),-real_catch_xy(:,2)-c];
        % Real window object position
        projection_xy=zeros(numobject,2);
        for i=1:numobject
            projection_xy(i,:)=[objectposition(i).robotarm];
        end
        real_projection_xy=round(projection_xy.*cmperpixel,2);
        % Absolute position of window object relative to origin of arm robot
        abs_projection_xy=[a-real_projection_xy(:,2),-real_projection_xy(:,1)-b-c];
        % Relative position of object and window object
        movetimes=size_robot_real(2)*2+1;
        movingCentimeter=zeros(movetimes,2);
        for i=1:movetimes
            if i==1
                movingCentimeter(i,:)=abs_catch_xy(i,:);
            elseif mod(i,2)==0
                movingCentimeter(i,:)=abs_projection_xy(i/2,:)-abs_catch_xy(i/2,:);
            elseif mod(i,2)==1&&i~=1&&i~=movetimes
                movingCentimeter(i,:)=abs_catch_xy((i+1)/2,:)-abs_projection_xy((i+1)/2-1,:);
            elseif i==movetimes
                movingCentimeter(i,:)=-abs_projection_xy((i+1)/2-1,:);
            end
        end
        movingSteps=movingCentimeter;
        turnangle=zeros(1,size_robot_real(2));
        for i=1:size_robot_real(2)
            turnangle(i)=robotobject(i).theta;
        end
        turnstep=round(turnangle./angleperstep);
        %% Sending moving track and turning angle data
        dataX="";
        dataY="";
        dataC="";
        for i=1:movetimes
            if i==movetimes
                dataX=dataX+num2str(movingSteps(i,1)/cmperstepX);
                dataY=dataY+num2str(movingSteps(i,2)/cmperstepY);
            else
                dataX=dataX+num2str(movingSteps(i,1)/cmperstepX)+" ";
                dataY=dataY+num2str(movingSteps(i,2)/cmperstepY)+" ";
            end
        end
        for i=1:numobject
            if i==numobject
                dataC=dataC+num2str(turnstep(i));
            else
                dataC=dataC+num2str(turnstep(i))+" ";
            end
        end
        fprintf(COM_X,dataX);
        pause(0.1);
        fprintf(COM_Y,dataY);
        pause(0.1);
        fprintf(COM_ZC,dataC);
        %% Player playing game
        % Show ready window
        for i=5:-1:1
            imshow(readywindow(i).window);
            pause(1);
            if i==3
                Player_projection_Object=snapshot(playercam);
            end
        end
        % Show game screen
        imshow(newwindow);
        pause(1);
        % Calculate player projection object centroid
        Player_projection_Object=imcrop(Player_projection_Object,[284 209 832-284 736-209]);
        playerobject_1=findplayerobject(Player_projection_Object,gridnum); % Return struct with centroid
        while(get(COM_ZC,'BytesAvailable')==0)
        end
        %% Complete matching
        dataZC=fscanf(COM_ZC);
        check=str2double(dataZC);
        disp(dataZC);
        if check==1
            robot_score=numobject;
        elseif check==2
            robot_score=str2double(fscanf(COM_ZC));
        else
            imshow(resetwindow);
            pause(2);
            continue
        end
        %% Scoring process
        % Show calculating projection screen
        imshow(calculatingwindow);
        pause(1);
        % Find player object
        Iplayerobject=snapshot(playercam);
        Iplayerobject=imcrop(Iplayerobject,[284 209 832-284 736-209]);
        playerobject_2=findplayerobject2(Iplayerobject); % Return struct with centroid
        % Real object position
        size_player_projection=size(playerobject_1);
        size_player_real=size(playerobject_2);
        real_projection_xy=zeros(size_player_projection(2),2);
        real_player_xy=zeros(size_player_projection(2),2);
        for i=1:size_player_projection(2)
            real_projection_xy(i,:)=playerobject_1(i).centroid;
            if i>size_player_real(2)
                real_player_xy(i,:)=[0 0];
            else
                real_player_xy(i,:)=playerobject_2(i).centroid;
            end
        end
        % Score player point
        finding=zeros(size_player_projection(2),1);
        finding(:)=0;
        for i=1:size_player_projection(2)
            for j=1:size_player_projection(2)
                xm=abs(real_projection_xy(i,1)-real_player_xy(j,1));
                ym=abs(real_projection_xy(i,2)-real_player_xy(j,2));
                if xm<=score_condition&&ym<=score_condition
                    finding(i)=1;
                    break;
                end
            end
        end
        player_score=sum(finding);
        %% Resulting and reset process
        % Evaluate process
        if player_score>robot_score
            imshow(wingamewindow);
        elseif player_score<robot_score
            imshow(losegamewindow);
        else
            imshow(tiegamewindow);
        end
        pause(6);
        imshow(resetwindow);
        pause(2);
        close
    end
    clearvars robotobject data Irobotobject;
end
