clear all;close all;
im = imread('./pics/Fig1016(a)(building_original).tif');
imshow(im);
%im = rgb2gray(im);
hy  = fspecial('sobel');
hx = hy';
Iy = imfilter(double(im),hy,'replicate');
Ix = imfilter(double(im),hx,'replicate');
gradmap = sqrt(Ix.^2+Iy.^2); 

L = watershed(gradmap);
Lrgb = label2rgb(L);
figure;imshow(Lrgb);

h = fspecial('average',11);
gradmap = imfilter(gradmap,h);

L = watershed(gradmap);

Lrgb = label2rgb(L);
figure;imshow(Lrgb);