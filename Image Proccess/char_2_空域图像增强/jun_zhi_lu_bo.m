clear all;
close all;
im = imread('./pics/standard_lena.bmp');

h = ones(5,5)/25;

out = imfilter(im,h);
subplot(1,2,1);imshow(im);
subplot(1,2,2);imshow(out);