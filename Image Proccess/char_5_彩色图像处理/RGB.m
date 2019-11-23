I = imread('./pics/Fig6.08.jpg');
R = I(:,:,1);
G = I(:,:,2);
B = I(:,:,3);
subplot(2,2,1),imshow(I),title('原始图像');
subplot(2,2,2),imshow(R),title('红色分量');
subplot(2,2,3),imshow(G),title('绿色分量');
subplot(2,2,4),imshow(B),title('蓝色分量');