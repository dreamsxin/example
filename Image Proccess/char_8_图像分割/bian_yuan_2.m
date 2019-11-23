clear all;close all;
im = imread('./pics/standard_lena.bmp');
esobel = edge(im,'sobel');
erob = edge(im,'roberts');
eprew = edge(im,'prewitt');

subplot(2,2,1),imshow(im),title('ԭͼ');
subplot(2,2,2),imshow(esobel),title('sobel');
subplot(2,2,3),imshow(erob),title('roberts');
subplot(2,2,4),imshow(eprew),title('prewitt');
