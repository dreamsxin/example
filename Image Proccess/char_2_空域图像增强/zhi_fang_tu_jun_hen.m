I=imread('./pics/huafen.bmp');
A = rgb2gray(I);
J = histeq(A);
subplot(2,2,1),imshow(A);
subplot(2,2,2),imshow(J);
subplot(2,2,3),imhist(A);
subplot(2,2,4),imhist(J);
