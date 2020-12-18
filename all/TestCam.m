% clc
% clear all
% close all
% % cam1=webcam(1);
% cam2=webcam(3);
% % cam1.Resolution='1280x960';
% cam2.Resolution='1280x960';
% pause(3);
% I1=snapshot(cam1);


%% cam2 
% figure('units','normalized','outerposition',[0 0 1 1])
%     set(gcf,"WindowState",'maximized');
%     set(gcf,'MenuBar','none')
%     set(gca,'Position',[0 0 1 1 ])
%     set(gcf,'NumberTitle','off');
% % pause(4);
% ratio=2.75; %55cm/20cm
% sizewidth=2000; %pixel
% widthlength=55; %cm x434y102, 109
% heightlength=widthlength/ratio; %cm
% sizeheight=round(sizewidth/ratio);
% pointwindow=resizewindow(imread('pointwindow.jpg'),sizewidth,sizeheight);
% Pointwindow=createpointwindow(pointwindow,3,5);
% imshow(Pointwindow);
% pause(2);
% I2=snapshot(cam2);
% figure,imshow(I2);

%% cam1
% I1=imrotate(I1,270);
% I1=imcrop(I1,[110 450 490 780]);
% imshow(pointwindow);
% I2=imcrop(I2,[193 205 731-193 751-205]);
% figure,imshow(I1);


%% find playerobject
threshold=0.4;
C=im2double(Iplayerobject(:,:,2));
D=C;
D(C>threshold)=0;
D(C<=threshold)=1;
D=imfill(D,'holes');
D=bwareaopen(D, 4000);
imshow(D);

