%% ¸¯Ê´
clc 
clear
A1 = imread('./pics/Fig9.07(a).jpg');
se1 = strel('disk',5);
A2 = imerode(A1,se1);

se2 = strel('disk',10);
A3 = imerode(A1,se2);

se3 = strel('disk',15);
A4 = imerode(A1,se3);


subplot(2,2,1),imshow(A1),title('A1');
subplot(2,2,2),imshow(A2),title('A2');
subplot(2,2,3),imshow(A3),title('A3');
subplot(2,2,4),imshow(A4),title('A4');