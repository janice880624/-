clc
clear all
close all
cam1=webcam(1);
cam2=webcam(3);
cam1.Resolution='1280x960';
cam2.Resolution='1280x960';
pause(1);
I1=snapshot(cam1);
I2=snapshot(cam2);
figure,imshow(I1);
figure,imshow(I2);

