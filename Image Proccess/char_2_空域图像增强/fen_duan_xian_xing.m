im = imread('./pics/huafen.bmp');
im = im(:,:,1);
subplot(2,2,1);
imshow(im);
subplot(2,2,2);
imhist(im);
rmin = min(min(im));
rmax = max(max(im));
k = 255/(rmax - rmin);
im = k.*(im - rmin);
subplot(2,2,3);
imshow(im);
subplot(2,2,4);
imhist(im);