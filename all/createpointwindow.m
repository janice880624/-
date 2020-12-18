function pointwindow=createpointwindow(I,pointplayer,pointrobot)
windowsize=size(I);
sizeheight=windowsize(1);
sizewidth=windowsize(2);
number=imread('number.jpg');
number=im2double(rgb2gray(number));
sizenum=size(number);
numstruct=struct();
for i=1:10
    if i==1
        num=number(:,1:round(sizenum(2)/10));
    elseif i ==6
        num=number(:,round(sizenum(2)/10)*(i-1):round(sizenum(2)/10)*i);
        num(:,1:round(11*sizenum(2)/1220))=1;
    elseif i ==10
        num=number(:,round(sizenum(2)/10)*(i-1):sizenum(2));
    else
        num=number(:,round(sizenum(2)/10)*(i-1):round(sizenum(2)/10)*i);
    end
    num=imresize(num,[round(9/55*sizewidth) round(13/110*sizewidth)]);
    sizeNum=size(num);
    colornum=zeros(sizeNum(1),sizeNum(2),3);
    num=1-num;
    colornum(:,:,3)=num;
    numstruct(i).num=colornum;
end

sizenum=size(numstruct(1).num);
I(round(6*sizeheight/12):round(6*sizeheight/12+sizenum(1)-1),...
    round(sizewidth/5-sizenum(2)/2):round(sizewidth/5+sizenum(2)/2-1),:)=numstruct(pointplayer).num;
I(round(6*sizeheight/12):round(6*sizeheight/12+sizenum(1)-1),...
    round(4*sizewidth/5-sizenum(2)/2):round(4*sizewidth/5+sizenum(2)/2-1),:)=numstruct(pointrobot).num;
for i=1:windowsize(1)
    for j=1:windowsize(2)
        if I(i,j,1)<0.2&&I(i,j,2)<0.2&&I(i,j,3)<0.2
            I(i,j,:)=1;
        end
    end
end
pointwindow=I;