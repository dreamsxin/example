clear all;
close all;
A = imread('./pics/standard_lena.bmp');
b0 = 1;
b1 = 2;
b2 = 4;
b3 = 8;
b4 = 16;
b5 = 32;
b6 = 64;
b7 = 128;
B0 = bitand(A,b0);
B1 = bitand(A,b1);
B2 = bitand(A,b2);
B3 = bitand(A,b3);
B4 = bitand(A,b4);
B5 = bitand(A,b5);
B6 = bitand(A,b6);
B7 = bitand(A,b7);
figure(1);
imshow(A);title('ԭʼͼ��');
figure(2);
subplot(2,2,1);imshow(B0,[]);
subplot(2,2,2);imshow(B1,[]);
subplot(2,2,3);imshow(B2,[]);
subplot(2,2,4);imshow(B3,[]);
figure(3);
subplot(2,2,1);imshow(B4,[]);
subplot(2,2,2);imshow(B5,[]);
subplot(2,2,3);imshow(B6,[]);
subplot(2,2,4);imshow(B7,[]);