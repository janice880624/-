function playerobject=findplayerobject(I,gridnum)
A=rgb2gray(im2double(I));
B=A;
if gridnum==3
    B=A;
    B(A>0.3)=0;
    B(A<=0.3)=1;
    B=bwareaopen(B, 10000);
else
    B(A>0.6)=0;
    B(A<=0.6)=1;
    B=B-bwareaopen(B, 10000);
    B=bwareaopen(B, 3000);
end
[L, n]=bwlabel(B); %label the object, L:pixel array with 1,2,3..n, n:number of object.
sizeI=size(I);
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
    x_O=totalx_O/pointcount_O;
    y_O=totaly_O/pointcount_O;
    Object_struct(objectcount).centroid=[x_O,y_O];
    
end
playerobject=Object_struct;