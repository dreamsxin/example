im = imread('./pics/xingxing.bmp');
im = rgb2gray(im);
subplot(1,3,1);imshow(im);
h = ones(5,5)/25;
im = imfilter(im,h);
level = graythresh(im);
bw = im2bw(im,level);
subplot(1,3,2);imshow(im);
subplot(1,3,3);imshow(bw);