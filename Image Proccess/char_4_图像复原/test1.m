close all;clear all;

% get the original image
original = imread('./pics/test1_1.tif');
%subplot(2,3,1),imshow(original),title('original image');
%figure(),imshow(original);
% distort the image
% rescale the image
scale = 0.7;
distorted = imresize(original,scale);
% rotate the image
theta = 35;
distorted = imrotate(distorted,theta);
%subplot(2,3,2),imshow(distorted),title('distorted image');
%figure(),imshow(distorted);

% select control points
fixedPoints = [1 1;1 679];
movingPoints = [1 335;278 712];
tform = fitgeotrans(movingPoints,fixedPoints,'nonreflectivesimilarity');

tformInv = invert(tform);
Tinv = tformInv.T;
ss = Tinv(2,1);
sc = Tinv(1,1);
sacle_recovered = sqrt(ss*ss+sc*sc);

theta_recovered = atan2(ss,sc)*180/pi;

Roriginal = imref2d(size(original));
recovered = imwarp(distorted,tform,'OutputView',Roriginal);
montage({original,recovered})