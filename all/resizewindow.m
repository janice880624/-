function window=resizewindow(I,sizewidth,sizeheight)
I=imresize(I,[sizeheight sizewidth]);
I=im2double(I);
newwindow=zeros(1100,2000,3);
sizewindow=size(newwindow);
newwindow(1:sizewindow(1)-726,:,:)=1;
newwindow(sizewindow(1)-726:sizewindow(1),:,:)=I;
window=newwindow;