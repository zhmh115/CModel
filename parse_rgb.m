clear
clc
fid=fopen('middleOutput.rgb10','rb');
m=fread(fid, 'uint16');
width = 3264;
height = 2448;
mb=m(1:(width * height));
mg=m((width * height + 1):(2 * width * height));
mr=m((2 * width * height + 1):end);
mb=reshape(mb, width, height)';
mg=reshape(mg, width, height)';
mr=reshape(mr, width, height)';
mout(:,:,1)=mr/4;
mout(:,:,2)=mg/4;
mout(:,:,3)=mb/4;
imwrite(uint8(mout),'out.bmp');
