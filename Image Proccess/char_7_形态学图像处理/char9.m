clc
clear
A1 = imread('./pics/Fig0911(a)(noisy_fingerprint).tif');
se = strel('square',3);
A2 = imerode(A1,se);
subplot(1,2,1),imshow(A1),title('Ô­Ê¼Í¼Ïñ');
subplot(1,2,2),imshow(A2),title('¸¯Ê´Í¼Ïñ');
