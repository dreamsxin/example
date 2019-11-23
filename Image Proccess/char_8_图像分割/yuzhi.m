clear all;close all;
im = imread('./pics/test.bmp');
im = rgb2gray(im);
subplot(2,3,1),imshow(im);
[m n]=size(im);
im = medfilt2(im,[7,7]);
subplot(2,3,2),imshow(im);
subplot(2,3,3),imhist(im);

T1 = 117;
T2 = 177;

for x=1:m
    for y=1:n
        if(im(x,y)<T1)
            im(x,y)=0;
        else
            if(im(x,y)<T2)
                im(x,y)=127;
            else
                im(x,y)=255;
            end
        end
    end
end
subplot(2,3,4),imshow(im);
im=edge(im,'sobel');
subplot(2,3,5),imshow(im);