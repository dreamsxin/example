clear all;close all;
I(1:256,1:256)=uint8(50);
I(20:120,50:100)=140;
I(150:220,20:230)=200;

I1 = imnoise(I,'salt & pepper', 0.02);
I2 = imnoise(I,'gaussian',0, 0.0025);
figure(1);
subplot(1,2,1);imshow(I,[]);subplot(1,2,2);imhist(I);
figure(2);
subplot(1,2,1);imshow(I1,[]);subplot(1,2,2);imhist(I1);
figure(3);
subplot(1,2,1);imshow(I2,[]);subplot(1,2,2);imhist(I2);