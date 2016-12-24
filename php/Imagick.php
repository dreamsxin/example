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

header("Content-Type: image/png");
echo generate();
