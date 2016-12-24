<?php

function generate() {
    $captcha = new Phalcon\Chart\Captcha("hello",NULL,20,80,34);
    return $captcha->render();
}

function generate2() {
    $pixel = new ImagickPixel('#000');
    $imagick = new \Imagick;
    $imagick->newimage(200,100, $pixel);
    $pixel = new ImagickPixel('#fff');
    $draw = new ImagickDraw();
    $draw->setFillColor($pixel);
    $draw->setFontSize(20);

    $draw->setgravity(Imagick::GRAVITY_CENTER);
    $imagick->annotateimage($draw, 0, 0, 0, "hello world 111111");
    $roll = 45;
    $corner = 41;
    $imagick->rollimage($roll, 0);
    $imagick->swirlimage(-$corner);
    $imagick->rollimage(-$roll*2, 0);
    $imagick->swirlimage(43);
    $imagick->rollimage($roll, 0);

    $imagick->setImageFormat('png');
    echo $imagick->getImageBlob();
}

function generate3($code, $w, $h) {
    $pixel = new ImagickPixel('#000');
    $imagick = new \Imagick;
    $imagick->newimage($w,$h, $pixel);

    $pixel = new ImagickPixel('#fff');
    $draw = new ImagickDraw();
    $draw->setFillColor($pixel);
    $draw->setFontSize(20);

    $draw->setgravity(Imagick::GRAVITY_CENTER);
    $imagick->annotateimage($draw, 0, 0, 0, $code);

    $color = 'rgb('.mt_rand(0,255).','.mt_rand(0,255).','.mt_rand(0,255).')';
    $pixel = new ImagickPixel($color);
    $draw = new ImagickDraw();
    $draw->setFillColor($pixel);
    $draw->setFontSize(20);
    $draw->setgravity(Imagick::GRAVITY_WEST);
    $imagick->annotateimage($draw, 0, 0, 0, mt_rand(1, 9));

    $color = 'rgb('.mt_rand(0,255).','.mt_rand(0,255).','.mt_rand(0,255).')';
    $pixel = new ImagickPixel($color);
    $draw = new ImagickDraw();
    $draw->setFillColor($pixel);
    $draw->setFontSize(20);
    $draw->setgravity(Imagick::GRAVITY_EAST);
    $charts = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $imagick->annotateimage($draw, 0, 0, 0, $charts[mt_rand(0, 24)]);

    $roll = rand(20, 50);
    $corner = rand(30, 50);
    $imagick->rollimage($roll, 0);
    $imagick->swirlimage(-$corner);
    $imagick->rollimage(-$roll*2, 0);
    $imagick->swirlimage(rand(30, 50));
    $imagick->rollimage($roll, 0);


    $imagick->setImageFormat('png');
    echo $imagick->getImageBlob();
}

header("Content-Type: image/png");
echo generate();
