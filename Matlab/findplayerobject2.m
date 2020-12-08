function playerobject=findplayerobject2(I)
C=im2double(I(:,:,2));
D=C;
D(C>0.4)=0;
D(C<=0.4)=1;
D=imfill(D,'holes');
D=bwareaopen(D, 4000);
[L, n]=bwlabel(D); %label the object, L:pixel array with 1,2,3..n, n:number of object.
sizeI=size(I);
Object_struct=struct();
if n==0
    Object_struct(1).centroid=[0 0];
else
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
end
playerobject=Object_struct;