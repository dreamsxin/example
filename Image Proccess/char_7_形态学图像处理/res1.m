%% ≈Ú’Õ
clc
clear

A1 = imread('./pics/Fig9.05(a).jpg');
A1 = 255 - A1;
B =[0 1 0
    1 1 1
    0 1 0];
A2 = imdilate(A1,B);
A3 = imdilate(A2,B);
A4 = imdilate(A3,B);

subplot(2,2,1),imshow(A1),title('A1');
subplot(2,2,2),imshow(A2),title('A2');
subplot(2,2,3),imshow(A3),title('A3');
subplot(2,2,4),imshow(A4),title('A4');