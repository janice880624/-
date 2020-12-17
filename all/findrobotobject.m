function robotobject=findrobotobject(I)
I=im2double(I);
sizeI=size(I);
J=rgb2hsv(I);
S=J(:,:,2);
V=J(:,:,3);
for i=1:sizeI(1)
    for j=1:sizeI(2)
        if V(i,j)<0.8||S(i,j)>0.45
            I(i,j,:)=0;
        end
    end
end
I(I~=0)=1;
I=im2bw(I);
I=bwareaopen(I, 7000);
% figure,imshow(Im),hold on
[L, n]=bwlabel(I); %label the object, L:pixel array with 1,2,3..n, n:number of object.
Object_struct=struct();
for objectcount=1:n
    object=zeros(sizeI(1),sizeI(2));
    totalx_O=0;
    totaly_O=0;
    pointcount_O=0;
    for i=1:sizeI(1)
        for j=1:sizeI(2)
            if L(i,j)==objectcount
                object(i,j)=1;
                totalx_O=totalx_O+i;
                totaly_O=totaly_O+j;
                pointcount_O=pointcount_O+1;
            else
                object(i,j)=0;
            end
        end
    end
    edge_O=edge(object);
    [H_O,T_O,R_O] = hough(edge_O);
    P_O  = houghpeaks(H_O,1,'threshold',ceil(0.3*max(H_O(:))));
    lines_O = houghlines(edge_O,T_O,R_O,P_O,'FillGap',20,'MinLength',7);
    Object_struct(objectcount).theta=lines_O.theta+90;
    x_O=totalx_O/pointcount_O;
    y_O=totaly_O/pointcount_O;
    Object_struct(objectcount).centroid=[x_O,y_O];
    
end
NERSX_1=125; %NonErrorRegionSize
NERSX_2=60;
NERSY_1=60;
NERSY_2=60;
CamOriginXY=[190 370];
ErrorPercentX=0.05;
ErrorPercentY=0.04;
xy=zeros(1,2);
for i=1:n
    xy=Object_struct(i).centroid;
    if xy(1)<CamOriginXY(1)+NERSX_2 && xy(1)>CamOriginXY(1)-NERSX_1 && xy(2)<CamOriginXY(2)+NERSY_2 && xy(2)>CamOriginXY(2)-NERSY_1
        Object_struct(i).centroid=xy;
%         plot(xy(2),xy(1),'rx');
        continue;
    end
    if xy(1)<CamOriginXY(1)+NERSX_2 && xy(1)>CamOriginXY(1)-NERSX_1
        if xy(2)<CamOriginXY(2)-NERSY_1
            xy(2)=xy(2)-(xy(2)-CamOriginXY(2)+NERSY_2)*ErrorPercentY;
        elseif xy(2)>CamOriginXY(2)+NERSY_2
            xy(2)=xy(2)-(xy(2)-CamOriginXY(2)-NERSY_1)*ErrorPercentY;
        end
    elseif xy(2)<CamOriginXY(2)+NERSY_2 && xy(2)>CamOriginXY(2)-NERSY_1
        if xy(1)<CamOriginXY(1)-NERSX_1
            xy(1)=xy(1)-(xy(1)-CamOriginXY(1)+NERSX_2)*ErrorPercentX;
        elseif xy(1)>CamOriginXY(1)+NERSX_2
            xy(1)=xy(1)-(xy(1)-CamOriginXY(1)-NERSX_1)*ErrorPercentX;
        end
    else
        xy(1)=xy(1)-(xy(1)-196)*ErrorPercentX;
        xy(2)=xy(2)-(xy(2)-376)*ErrorPercentY;
    end
    Object_struct(i).centroid=xy;
%     plot(xy(2),xy(1),'rx');
end
robotobject=Object_struct;