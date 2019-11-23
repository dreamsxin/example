clear all;close all;
im = imread('./pics/moon.bmp');
im = im(:,:,1);
imshow(im);
f = im2double(im);
w = fspecial('laplacian',0);
g1 = imfilter(f,w,'replicate');
f = f-g1;
figure,imshow(f);