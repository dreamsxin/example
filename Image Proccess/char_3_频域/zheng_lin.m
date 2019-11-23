clear all;close all;
h = zeros(400,400);
for i = 1:400   %АнПлµННЁВЛІЁЖч
    for j = 1:400
        d = sqrt((i-201)^2+(j-201)^2);
        if d < 5
            h(i,j) = 1;
        else 
            h(i,j)=0;
        end
    end
end
h = ifftshift(h);
figure; imshow(real(h));title('НјПс');
im=ifft2(h);
max_pix = max(max(im)); min_pix = min(min(im));
im = (im-min_pix)/(max_pix-min_pix)*255;
im = ifftshift(im);
figure,imshow(uint8(im));
