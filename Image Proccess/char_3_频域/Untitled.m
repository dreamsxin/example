clear all;
close all;
im = zeros(400,400);
im(185:255,195:205) = 255;
im_DFT = fft2(im);
imshow(log(abs(im_DFT)));

