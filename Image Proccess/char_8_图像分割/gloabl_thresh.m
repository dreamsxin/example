clear all;close all;
im = imread('./pics/Fig10.25.jpg');
im = rgb2gray(im);
subplot(2,2,1),imshow(im);
im = medfilt2(im,[3,3]);
subplot(2,2,2),imhist(im);
T = graythresh(im);
im = imbinarize(im,T);
subplot(2,2,3),imshow(im);
A = edge(im,'sobel');
subplot(2,2,4)
imshow(A)