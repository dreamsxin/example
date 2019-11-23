clear all;close all;
im = imread('./pics/bone.bmp');
subplot(2,3,1),imshow(im);
g = bwmorph(im,'skel',inf);
subplot(2,3,2),imshow(g);
f = bwmorph(im,'spur',8);
subplot(2,3,3),imshow(f);
k = im -g;
subplot(2,3,4),imshow(k);