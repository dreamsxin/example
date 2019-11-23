clear all; close all;
W=0.2549
I = imread('./pics/caomei.bmp');
%imshow(I);
I = double(I);
I = I / 255;
[m n c] = size(I);
R = I(:,:,1);
G = I(:,:,2);
B = I(:,:,3);
for x=1:m
    for y=1:n
        if(abs(R(x,y)-0.6863)>W/2)
            R(x,y) = 0.5;G(x,y) = 0.5;B(x,y) = 0.5;
        end
        if(abs(G(x,y)-0.1608)>W/2)
            R(x,y) = 0.5;G(x,y) = 0.5;B(x,y) = 0.5;
        end
        if(abs(B(x,y)-0.1922)>W/2)
            R(x,y) = 0.5;G(x,y) = 0.5;B(x,y) = 0.5;
        end
    end
end
I1(:,:,1) = R;
I1(:,:,2) = G;
I1(:,:,3) = B;
I1=I1*255;
I1=uint8(I1);
%figure; imshow(I1);
subplot(1,2,1),imshow(I);
subplot(1,2,2),imshow(I1);

