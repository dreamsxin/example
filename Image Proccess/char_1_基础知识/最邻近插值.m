img = imread('girl.jpg');
img = rgb2gray(img);
imshow(img);
[m n]=size(img);%128 x 124
w = m*6;
h = n*6;
imgn = zeros(w,h);

%放大后行列间距是原图行列间距的倍数
% plus:间距数量为：  行数（列数） - 1，压缩后小方格长宽
% w_step，h_step倍 （小于1）
w_step = (m-1)/(w-1);
h_step = (n-1)/(h-1);

for x=1:w
    for y=1:h
        %原点（1，1）还对应（1，1）
        pix = [(x-1)*w_step (y-1)*h_step];
        %依据四舍五入来判断离哪个最近
        imgn(x,y)=img(round(pix(1))+1,round(pix(2))+1);
    end
end
figure,imshow(uint8(imgn))