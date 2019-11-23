clear all;
A = imread('./pics/xingxi.bmp');
A = A(:,:,1);
n = 0.005;
N1 = imnoise(A,'gaussian',0,n);
N2 = imnoise(A,'gaussian',0,n);
N3 = imnoise(A,'gaussian',0,n);
N4 = imnoise(A,'gaussian',0,n);
N5 = imnoise(A,'gaussian',0,n);
N6 = imnoise(A,'gaussian',0,n);
N7 = imnoise(A,'gaussian',0,n);
N8 = imnoise(A,'gaussian',0,n);

B = 0.125*N1+0.125*N2+0.125*N3+0.125*N4+0.125*N5+0.125*N6+0.125*N7+0.125*N8;
figure(1);
subplot(1,3,1) ;imshow(A); title('原图');
subplot(1,3,2) ;imshow(N2); title('有噪声图');
subplot(1,3,3) ;imshow(B,[]); title('去噪处理');
