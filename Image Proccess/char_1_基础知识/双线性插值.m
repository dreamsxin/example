% BILINEAR-INTERPLOT SOUCE-IMAGE TO GET A DESTINATE-IMAGE 
% MAXIMUM SCALOR == 5.0, MINIMUM SCALOR == 0.2 
% read source image into memory, and get the primitive rows and cols  
close all; clear all;
I=imread('girl.jpg'); 
imshow(I);
I = rgb2gray(I);
[nrows,ncols]=size(I);  

% acquire scale-factor, the range is 0.2-5.0 
K = str2double(inputdlg('please input scale factor (must between 0.2 - 5.0)', 'INPUT scale factor', 1, {'0.5'}));  
% Validating  
if (K < 0.2) | (K > 5.0) 
    errordlg('scale factor beyond permitted range(0.2 - 5.0)', 'ERROR');  
    error('please input scale factor (must between 0.2 - 5.0)');  
end

% output image width and height are both scaled by factor K  
width = K * nrows;  
height = K * ncols;  
J = uint8(zeros(width,height));  

% width scale and height scale  
heightScale = nrows/width;  
widthScale = ncols/height;  

% bilinear interplot  
for x = 5:width - 5  
    for y = 5:height - 5  
        xx = x * widthScale;  
        yy = y * heightScale;  
        if (xx/double(uint16(xx)) == 1.0) & (xx/double(uint16(xx)) == 1.0)  
            J(x,y) = I(int16(xx),int16(yy));  
        else % a or b is not integer  
        %  b+1->y2  b->y1 a->x1  a+1->x2
            a = double(uint16(xx)); % (a,b) is the base-dot 
            b = double(uint16(yy));  
            x11 = double(I(a,b)); % x11 <- I(a,b)  
            x12 = double(I(a,b+1)); % x12 <- I(a,b+1)  
            x21 = double(I(a+1,b)); % x21 <- I(a+1,b)  
            x22 = double(I(a+1,b+1)); % x22 <- I(a+1,b+1)  
            J(x,y) = uint8( (b+1-yy) * ((xx-a)*x21 + (a+1-xx)*x11) + (yy-b) * ((xx-a)*x22 +(a+1-xx) * x12) ); % calculate J(x,y)  
        end
    end
end  % show the interplotted image  

imwrite(J, 'girl2.jpg', 'jpg');  
figure;  
imshow(J);