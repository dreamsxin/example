/*
* Name: canvas_clock.js
* Author: Michael Kruger
* Brief:
*
* This is a canvas based library with 15 different clocks that can be embedded in webpages. 
* For more info please visit: www.krugaroo.com#canvasClock
*
* Copyright 2016 Krugaroo
* More Info: www.krugaroo.com#canvasClock
*
* License: MIT License
*
* Copyright (c) 2016 Michael Kruger, Krugaroo
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

day_arr=["Sunday", "Monday", "Tuesday","Wednesday","Thursday","Friday","Saturday"];
month_arr=["January","February","March","April","May","June","July","August","September","October","November","December"];

function clock_conti(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
  indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
  time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
  date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;
  
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57+sec*0.1046+milisec/1000*0.1046,-1.569+sec*0.1046+milisec/1000*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/15,1.57+sec*0.1046+milisec/1000*0.1046,1.569+sec*0.1046+milisec/1000*0.1046,1);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=2;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57+min*0.1046+sec/60*0.1046,-1.569+min*0.1046+sec/60*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=3;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/4,-1.57+hour*0.523+min/60*0.523,-1.569+hour*0.523+min/60*0.523,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/80,0,6.28,0);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_conti(size, cns, clockd)},50);
}

function clock_digital(size, cns, clockd)
{
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours();

  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if(hour<10){hour="0"+hour;}
  if(min<10){min="0"+min;}
  if(sec<10){sec="0"+sec;}
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333";
  cns.textBaseline="middle";
  cns.textAlign="center";
  cns.font=size/8+"px Arial";
  cns.fillText(hour+":"+min+":"+sec,(size/2),size/2.5);

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/10,size,cns, clockd);
  }

  clockd.timer=setTimeout(function(){clock_digital(size, cns, clockd)},50);
}

function clock_reverse(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
    indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
    time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57-sec*0.1046,-1.569-sec*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/15,1.57-sec*0.1046,1.569-sec*0.1046,1);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=2;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57-min*0.1046,-1.569-min*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=3;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/4,-1.57-hour*0.523-min/60*0.523,-1.569-hour*0.523-min/60*0.523,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/80,0,6.28,0);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_reverse(size, cns, clockd)},50);
}

function clock_norm(size, cns, clockd)
{
var br=[60,120,180];
var r2=[10,20,30];
var r3=[40,80,120];
var r4=[4,5,7];

  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
    indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
    time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;
  
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57+sec*0.1046,-1.569+sec*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/15,1.57+sec*0.1046,1.569+sec*0.1046,1);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=2;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57+min*0.1046,-1.569+min*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=3;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/4,-1.57+hour*0.523+min/60*0.523,-1.569+hour*0.523+min/60*0.523,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/60,0,6.28,0);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_norm(size, cns, clockd)},50);
}

function clock_follow(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
  indicator(size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  cns.beginPath();
  cns.lineWidth=1;
  cns.arc((size/2),(size/2),size/3,-1.57,-1.569+sec*0.1046+milisec/1000*0.1046,0);
  cns.lineTo((size/2),(size/2));
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/17,1.57+sec*0.1046+milisec/1000*0.1046,1.569+sec*0.1046+milisec/1000*0.1046,1);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/3.5,-1.57,-1.569+min*0.1046+sec/60*0.1046,0);
  cns.lineTo((size/2),(size/2));
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/4.5,-1.57,-1.569+hour*0.523+min/60*0.523,0);
  cns.lineTo((size/2),(size/2));
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/60,0,6.28,0);
  cns.fill();
  cns.closePath();

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
    time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/15, size, cns, clockd);
  }

  clockd.timer=setTimeout(function(){clock_follow(size, cns, clockd)},50);
}

function clock_circles(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
  indicator(size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";
  cns.beginPath();
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3,-1.57,-1.569+sec*0.1046+milisec/1000*0.1046,0);
  cns.lineTo((size/2),(size/2));
  cns.fill();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.lineCap="round";
  cns.beginPath();
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3.5,-1.57,-1.569+min*0.1046+sec/60*0.1046,0);
  cns.lineTo((size/2),(size/2));
  cns.fill();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.lineCap="round";
  cns.beginPath();
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/4.5,-1.57,-1.569+hour*0.523+min/60*0.523,0);
  cns.lineTo((size/2),(size/2));
  cns.fill();
  cns.closePath();

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
  time_add((size/2),size/6*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
  date_add((size/2),size/6*3+size/10, size, cns, clockd);
  }

  clockd.timer=setTimeout(function(){clock_circles(size, cns, clockd)},50);
}

function clock_grow(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
  indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
  time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
  date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;
  
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";
  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),sec*size/200+size/15,-1.57+sec*0.1046+milisec/1000*0.1046,-1.569+sec*0.1046+milisec/1000*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.lineCap="round";
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),min*size/200+sec/60*size/200+size/15,-1.57+min*0.1046+sec/60*0.1046,-1.569+min*0.1046+sec/60*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.lineCap="round";
  cns.lineWidth=2;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),hour*size/200*3+min/60*size/200*3+size/15,-1.57+hour*0.523+min/60*0.523,-1.569+hour*0.523+min/60*0.523,0);
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/60,0,6.28,0);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_grow(size, cns, clockd)},50);
}

function clock_dots(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
  indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
  time_add((size/2),size/6*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
  date_add((size/2),size/6*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;
  
  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.lineCap="round";
  for(var a=0;a<(sec+1);a++)
  {
    var r=parseInt(a)*0.1046;
    var calc=Math.cos(r-1.57)*(size/2.8);
    var y=Math.sin(r-1.57)*(size/2.8);

    cns.beginPath();
    cns.arc(calc+(size/2),y+(size/2),size/100,0,6.28,0);
    cns.fill();
    cns.closePath();
  }

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.lineCap="round";

  for(var a=0;a<(min+1);a++)
  {
    var r=parseInt(a)*0.1046;
    var calc=Math.cos(r-1.57)*(size/3.4);
    var y=Math.sin(r-1.57)*(size/3.4);

    cns.beginPath();
    cns.arc(calc+(size/2),y+(size/2),size/100,0,6.28,0);
    cns.fill();
    cns.closePath();
  }

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";

  for(var a=0;a<(hour+1);a++)
  {
    var r=parseInt(a)*0.523;
    var calc=Math.cos(r-1.57)*(size/4.5);
    var y=Math.sin(r-1.57)*(size/4.5);

    cns.beginPath();
    cns.arc(calc+(size/2),y+(size/2),size/100,0,6.28,0);
    cns.fill();
    cns.closePath();
  }

  clockd.timer=setTimeout(function(){clock_dots(size, cns, clockd)},50);
}

function clock_num(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
    indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
    time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.lineCap="round";
  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3.2,-1.57+sec*0.1046+milisec/1000*0.1046,-1.569+sec*0.1046+milisec/1000*0.1046,0);
  cns.stroke();
  cns.closePath();

  cns.textBaseline="middle";
  cns.textAlign="center";
  cns.font=(size/35+5)+"px Arial";
  cns.fillText(sec,Math.cos(-1.57+sec*0.1046+milisec/1000*0.1046)*size/3+(size/2),Math.sin(-1.57+sec*0.1046+milisec/1000*0.1046)*size/3+(size/2))

  cns.beginPath();
  cns.lineWidth=1;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/15,1.57+sec*0.1046+milisec/1000*0.1046,1.569+sec*0.1046+milisec/1000*0.1046,1);
  cns.stroke();
  cns.closePath();

  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=2;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/3.2,-1.57+min*0.1046+sec/60*0.1046,-1.569+min*0.1046+sec/60*0.1046,0);
  cns.stroke();
  cns.closePath();
  cns.fillText(min,Math.cos(-1.57+min*0.1046+sec/60*0.1046)*size/3+(size/2),Math.sin(-1.57+min*0.1046+sec/60*0.1046)*size/3+(size/2))

  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.strokeStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=3;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/4,-1.57+hour*0.523+min/60*0.523,-1.569+hour*0.523+min/60*0.523,0);
  cns.stroke();
  cns.closePath();
  if(hour==0){var hour2=12;}else{var hour2=hour;}
  cns.fillText(hour2,Math.cos(-1.57+hour*0.523+min/60*0.523)*size/3.5+(size/2),Math.sin(-1.57+hour*0.523+min/60*0.523)*size/3.5+(size/2))

  cns.beginPath();
  cns.lineWidth=2;
  cns.arc((size/2),(size/2),size/80,0,6.28,0);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_num(size, cns, clockd)},50);
}

function clock_random(size, cns, clockd)
{
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours();

  if(!clockd.hasOwnProperty("track")){clockd["track"]=20;}
  clockd["track"]=parseInt(clockd["track"])+1;

  if(parseInt(clockd["track"])>=20)
  {
    cns.clearRect(0,0,size,size);

    cns.beginPath();
    if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
    cns.rect(0,0,size,size);
    cns.fill();
    cns.closePath();
    if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

    if(clockd.hasOwnProperty("date_add") && clockd.date_add)
    {
      date_add((size/2),size/5*3+size/10, size, cns, clockd);
    }

    var hourx=Math.floor(Math.random()*size/1.5)+Math.floor((size/2)/10);
    var houry=Math.floor(Math.random()*size/1.5)+Math.floor((size/2)/10);

    cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
    cns.strokeStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
    //if(hour==0){hour=12;}else{hour=hour;}
    if(hour<10){hour="0"+hour;}
    if(min<10){min="0"+min;}
    if(sec<10){sec="0"+sec;}

    cns.textBaseline="middle";
    cns.textAlign="left";
    cns.font=(size/15)+"pt Arial";
    cns.fillText(hour+":"+min+":"+sec,hourx,houry);

    clockd["track"]=0;
  }

  clockd.timer=setTimeout(function(){clock_random(size, cns, clockd)},50);
}

function clock_digitalran(size, cns, clockd)
{
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours();
  var hexarr=new Array("0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F");
  var a=Math.floor(Math.random()*16);
  var b=Math.floor(Math.random()*16);
  var c=Math.floor(Math.random()*16);
  var d=Math.floor(Math.random()*16);
  var e=Math.floor(Math.random()*16);
  var f=Math.floor(Math.random()*16);

  if(clockd.hasOwnProperty("track"))
  {
   clockd["track"]+=1; 
  }
  else
  {
    clockd["track"]=20;
  }

  if(parseInt(clockd["track"])==20)
  {
    cns.clearRect(0,0,size,size);

    cns.beginPath();
    if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
    cns.rect(0,0,size,size);
    cns.fill();
    cns.closePath();
    if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

    if(clockd.hasOwnProperty("date_add") && clockd.date_add)
    {
      date_add((size/2),size/3*2, size, cns, clockd);
    }

    if(hour==0){var hour2=12;}else{var hour2=hour;}
    if(hour<10){hour="0"+hour;}
    if(min<10){min="0"+min;}
    if(sec<10){sec="0"+sec;}
    cns.strokeStyle="#333333";
    cns.fillStyle="#"+hexarr[a]+hexarr[b]+hexarr[c]+hexarr[d]+hexarr[e]+hexarr[f];
    cns.textBaseline="middle";
    cns.textAlign="center";
    cns.font=size/7+"px Arial";
    cns.fillText(hour2+":"+min+":"+sec,(size/2),size/2.5);

    clockd["track"]=0;
  }
  
  clockd.timer=setTimeout(function(){clock_digitalran(size, cns, clockd)},50);
}

function clock_bars(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var mili=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;
  
  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
    cns.strokeStyle=(clockd.hasOwnProperty("indicate_color"))?clockd["indicate_color"]:"#333333";

    for(var a=0;a<13;a++)
    {
      if(a%3==0){cns.lineWidth=2;}else{cns.lineWidth=1;}
      cns.beginPath();
      cns.moveTo(size/7+size/7.1,size/8+a*size/1.3/12);
      cns.lineTo(size/2.38,size/8+a*size/1.3/12);
      cns.stroke();
      cns.closePath();

      cns.beginPath();
      cns.moveTo(size/2.35+size/7.1,size/8+a*size/1.3/12);
      cns.lineTo(size/1.45,size/8+a*size/1.3/12);
      cns.stroke();
      cns.closePath();
    }

    cns.beginPath();
    cns.moveTo(size/7+size/7.1*1.5,size/8);
    cns.lineTo(size/7+size/7.1*1.5,size/8+size/1.3);
    cns.stroke();
    cns.closePath();

    cns.beginPath();
    cns.moveTo(size/2.38+size/7.1*1.5,size/8);
    cns.lineTo(size/2.38+size/7.1*1.5,size/8+size/1.3);
    cns.stroke();
    cns.closePath();
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
    time_add((size/2),size/20, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/14*13, size, cns, clockd);
  }

  cns.strokeStyle=(clockd.hasOwnProperty("indicate_color"))?clockd["indicate_color"]:"#333333";
  cns.lineCap="round";
  cns.fillStyle=(clockd.hasOwnProperty("indicate_color"))?clockd["indicate_color"]:"#333333";

  cns.beginPath();
  cns.rect(size/7.4,size/8,size/6.8,size/1.3);
  cns.rect(size/2.38,size/8,size/6.8,size/1.3);
  cns.rect(size/1.45,size/8,size/6.8,size/1.3);
  cns.stroke();
  cns.closePath();

  cns.beginPath();
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.rect(size/7.4,size/8+size/1.3-(size/1.3)*(hour*60+min)/720,size/6.8,(size/1.3)*(hour*60+min)/720);
  cns.fill()
  cns.closePath();

  cns.beginPath()
  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.rect(size/2.38,size/8+size/1.3-(size/1.3)*(min*60+sec)/3600,size/6.8,(size/1.3)*(min*60+sec)/3600);
  cns.fill();
  cns.closePath();

  cns.beginPath();
  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.rect(size/1.45,size/8+size/1.3-(size/1.3)*(sec*1000+mili)/60000,size/6.8,(size/1.3)*(sec*1000+mili)/60000);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_bars(size, cns, clockd)},50);
}

function clock_planets(size, cns, clockd)
{
  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  if((clockd.hasOwnProperty("indicate") && clockd.indicate==true) || !clockd.hasOwnProperty("indicate"))
  {
  indicator(size, cns, clockd);
  }

  if(clockd.hasOwnProperty("time_add") && clockd.time_add)
  {
  time_add((size/2),size/5*3, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
  date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  if(clockd.hasOwnProperty("track")){cns.fillStyle=clockd["track"];}else{cns.fillStyle="#DAA520";}
  cns.beginPath();
  cns.lineWidth=3;
  cns.moveTo((size/2),(size/2));
  cns.arc((size/2),(size/2),size/25,0,6.29,0);
  cns.lineTo((size/2),(size/2));
  cns.fill();
  cns.closePath();

  var r=parseInt(milisec)*0.00628;
  var calcms=Math.cos(r-1.57)*(size/25);
  var yms=Math.sin(r-1.57)*(size/25);

  var r=parseInt(sec)*0.1046+parseInt(milisec)/1000*0.1046;
  var calc=Math.cos(r-1.57)*(size/5);
  var y=Math.sin(r-1.57)*(size/5);

  var r=parseInt(min)*0.1046+parseInt(sec)/60*0.1046;
  var calcm=Math.cos(r-1.57)*(size/3.2);
  var ym=Math.sin(r-1.57)*(size/3.2);

  var r=parseInt(hour)*0.523+parseInt(min)/60*0.523;
  var calcu=Math.cos(r-1.57)*(size/2.5);
  var yu=Math.sin(r-1.57)*(size/2.5);

  if(clockd.hasOwnProperty("indicate_color")){cns.strokeStyle=clockd["indicate_color"];}else{cns.strokeStyle="#333";}

  cns.closePath();
  cns.beginPath();
  cns.lineWidth=1;
  cns.arc((size/2),(size/2),(size/5),0,6.28,0);
  cns.stroke();
  cns.closePath();
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=3;
  cns.arc(calc+(size/2),y+(size/2),size/42,0,6.28,0);
  cns.fill();

  cns.closePath();
  cns.beginPath();
  cns.lineWidth=1;
  cns.arc((size/2)+calc,(size/2)+y,size/24,0,6.28,0);
  cns.stroke();
  cns.closePath();
  cns.beginPath();
  cns.lineWidth=3;
  cns.arc((size/2)+calc+calcms,(size/2)+y+yms,size/110,0,6.28,0);
  cns.fill();

  cns.beginPath();
  cns.lineWidth=1;
  cns.arc((size/2),(size/2),size/3.2,0,6.28,0);
  cns.stroke();
  cns.closePath();
  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=3;
  cns.arc(calcm+(size/2),ym+(size/2),size/24,0,6.28,0);
  cns.fill();
  cns.closePath();

  cns.beginPath();
  cns.lineWidth=1;
  cns.arc((size/2),(size/2),size/2.5,0,6.28,0);
  cns.stroke();
  cns.closePath();
  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.beginPath();
  cns.lineWidth=3;
  cns.arc(calcu+(size/2),yu+(size/2),size/22,0,6.28,0);
  cns.fill();
  cns.closePath();

  clockd.timer=setTimeout(function(){clock_planets(size, cns, clockd)},50);
}

function clock_roulette(size, cns, clockd)
{
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  var a=hour-2;var b=hour-1;var c=hour+1;var d=hour+2;var e=hour+3;
  if(a<0){a=24+a;}
  if(b<0){b=24+b;}
  if(c>23){c=c-24;}
  if(d>23){d=d-24;}
  if(e>23){e=e-24;}

  var f=hour;
  if(a<10){a="0"+a;}
  if(b<10){b="0"+b;}
  if(c<10){c="0"+c;}
  if(d<10){d="0"+d;}
  if(e<10){e="0"+e;}
  if(f<10){f="0"+f;}
  cns.font=size/10+"pt Arial";
  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.fillText(a,size/5,size/10-size/5*min/60-size/5*sec/36000);
  cns.fillText(b,size/5,size/10+size/5-size/5*min/60-size/5*sec/36000);
  cns.globalAlpha=0.3;
  cns.fillText(f,size/5,size/10+2*size/5-size/5*min/60-size/5*sec/3600);
  cns.globalAlpha=1;
  cns.fillText(c,size/5,size/10+3*size/5-size/5*min/60-size/5*sec/3600);
  cns.fillText(d,size/5,size/10+4*size/5-size/5*min/60-size/5*sec/3600);
  cns.fillText(e,size/5,size/10+5*size/5-size/5*min/60-size/5*sec/3600);

  var a=min-2;var b=min-1;var c=min+1;var d=min+2;var e=min+3;
  if(a<0){a=60+a;}
  if(b<0){b=60+b;}
  if(c>59){c=c-60;}
  if(d>59){d=d-60;}
  if(e>59){e=e-60;}

  var f=min;
  if(a<10){a="0"+a;}
  if(b<10){b="0"+b;}
  if(c<10){c="0"+c;}
  if(d<10){d="0"+d;}
  if(e<10){e="0"+e;}
  if(f<10){f="0"+f;}
  cns.fillStyle=(clockd.hasOwnProperty("dial2_color"))?clockd["dial2_color"]:"#333333";
  cns.fillText(a,size/2,size/10-size/5*sec/60-size/5*milisec/60000);
  cns.fillText(b,size/2,size/10+size/5-size/5*sec/60-size/5*milisec/60000);
  cns.globalAlpha=0.3;
  cns.fillText(f,size/2,size/10+2*size/5-size/5*sec/60-size/5*milisec/60000);
  cns.globalAlpha=1;
  cns.fillText(c,size/2,size/10+3*size/5-size/5*sec/60-size/5*milisec/60000);
  cns.fillText(d,size/2,size/10+4*size/5-size/5*sec/60-size/5*milisec/60000);
  cns.fillText(e,size/2,size/10+5*size/5-size/5*sec/60-size/5*milisec/60000);

  var a=sec-2;var b=sec-1;var c=sec+1;var d=sec+2;var e=sec+3;
  if(a<0){a=60+a;}
  if(b<0){b=60+b;}
  if(c>59){c=c-60;}
  if(d>59){d=d-60;}
  if(e>59){e=e-60;}

  var f=sec;
  if(a<10){a="0"+a;}
  if(b<10){b="0"+b;}
  if(c<10){c="0"+c;}
  if(d<10){d="0"+d;}
  if(e<10){e="0"+e;}
  if(f<10){f="0"+f;}
  cns.fillStyle=(clockd.hasOwnProperty("dial3_color"))?clockd["dial3_color"]:"#333333";
  cns.fillText(a,size/5*4,size/10-size/5*milisec/1000);
  cns.fillText(b,size/5*4,size/10+size/5-size/5*milisec/1000);
  cns.globalAlpha=0.3;
  cns.fillText(f,size/5*4,size/10+2*size/5-size/5*milisec/1000);
  cns.globalAlpha=1;
  cns.fillText(c,size/5*4,size/10+3*size/5-size/5*milisec/1000);
  cns.fillText(d,size/5*4,size/10+4*size/5-size/5*milisec/1000);
  cns.fillText(e,size/5*4,size/10+5*size/5-size/5*milisec/1000);

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  if(clockd.hasOwnProperty("indicate") && clockd.indicate)
  {
    cns.strokeStyle=(clockd.hasOwnProperty("indicate_color"))?clockd["indicate_color"]:"#333333";
    cns.beginPath();
    cns.moveTo(0,(size/2));
    cns.lineTo(size,(size/2));
    cns.stroke()
    cns.closePath();
  }

  clockd.timer=setTimeout(function(){clock_roulette(size, cns, clockd)},50);
}

function clock_binary(size, cns, clockd)
{
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var milisec=now.getMilliseconds();
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=now.getHours()%12;

  cns.clearRect(0,0,size,size);

  cns.beginPath();
  if(clockd.hasOwnProperty("bg_color")){cns.fillStyle=clockd["bg_color"];}else{cns.fillStyle="#ffffff";}
  cns.rect(0,0,size,size);
  cns.fill();
  cns.closePath();
  if(clockd.hasOwnProperty("bgLoaded") && clockd.bgLoaded==1){if(clockd.hasOwnProperty("bg_opacity")){cns.globalAlpha=clockd["bg_opacity"];cns.drawImage(clockd.bgImage,0,0,size,size);cns.globalAlpha=1;}}

  hourstr="";
  if(hour<1){hourstr="0";}
  while(hour>0)
  {
  hourstr=hour%2+hourstr;
  hour=hour-hour%2;
  hour=hour/2;
  }
  if(hourstr.length<6){while(hourstr.length<6){hourstr="0"+hourstr;}}

  minstr="";
  if(min<1){minstr="0";}
  while(min>0)
  {
  minstr=min%2+minstr;
  min=min-min%2;
  min=min/2;
  }
  if(minstr.length<6){while(minstr.length<6){minstr="0"+minstr;}}

  secstr="";
  if(sec<1){secstr="0";}
  while(sec>0)
  {
  secstr=sec%2+secstr;
  sec=sec-sec%2;
  sec=sec/2;
  }
  if(secstr.length<6){while(secstr.length<6){secstr="0"+secstr;}}

  cns.fillStyle=(clockd.hasOwnProperty("dial1_color"))?clockd["dial1_color"]:"#333333";
  cns.textBaseline="middle";
  cns.textAlign="center";
  cns.font=size/8+"px Courier New";
  cns.fillText(hourstr,(size/2),size/5);
  cns.fillText(minstr,(size/2),size/5*2);
  cns.fillText(secstr,(size/2),size/5*3);

  if(clockd.hasOwnProperty("date_add") && clockd.date_add)
  {
    date_add((size/2),size/5*3+size/10, size, cns, clockd);
  }

  clockd.timer=setTimeout(function(){clock_binary(size, cns, clockd)},50);
}

function clock_stop(clockd)
{
  clearTimeout(clockd.timer);
}

function clock_loadBG(src, clockd)
{
  clockd.bgImage = new Image();
  clockd.bgImage.onload = function(){ clockd.bgLoaded=1; };
  clockd.bgImage.src = src;
  
  if(!clockd.hasOwnProperty("bg_opacity")){clockd.bg_opacity=1;}
}

function indicator(size, cns, clockd)
{
  if(clockd.hasOwnProperty("indicate_color")){cns.strokeStyle=clockd["indicate_color"];}else{cns.strokeStyle="#333";}
  cns.lineWidth=2;

  for(var a=0;a<12;a++)
  {
    var r=parseInt(a)*0.523;
    var calc=Math.cos(r-1.57);
    var y=Math.sin(r-1.57);

    if(a%3==0){var ekstra=size/50;}else{var ekstra=0;}
    cns.beginPath();
    cns.moveTo(calc*(size/3+ekstra)+(size/2),y*(size/3+ekstra)+(size/2));
    cns.lineTo(calc*size/3.25+(size/2),y*size/3.25+(size/2));
    cns.stroke();
    cns.fill();
    cns.closePath();
  }
}

function time_add(x, y, size, cns, clockd)
{
  if(!clockd.hasOwnProperty("time_add"))
  {
    return;
  }
  
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var sec=now.getSeconds();
  var min=now.getMinutes();
  var hour=(clockd.hasOwnProperty("time_24h") && clockd["time_24h"])?now.getHours():now.getHours()%12;

  if(hour==0){hour=12;}
  if(hour<10){hour="0"+hour;}
  if(min<10){min="0"+min;}
  if(sec<10){sec="0"+sec;}
  cns.lineWidth=1;
  cns.fillStyle=(clockd.hasOwnProperty("time_add_color")?clockd["time_add_color"]:"#333");
  cns.textBaseline="middle";
  cns.textAlign="center";
  cns.font=size/15+"px Arial";

  switch(parseInt(clockd["time_add"]))
  {
    case 1:
      cns.fillText(hour+":"+min+":"+sec,x,y);
    break;
    case 2:
      cns.fillText(hour+":"+min,x,y);
    break;
    case 3:
      hour=now.getHours();
        if(hour<10){hour="0"+hour;}
    cns.fillText(hour+":"+min+":"+sec,x,y);
    break;
    default:
      hour=now.getHours();
      if(hour<10){hour="0"+hour;}
      cns.fillText(hour+":"+min,x,y);
  }
}

function date_add(x, y, size, cns, clockd)
{
  if(!clockd.hasOwnProperty("date_add"))
  {
    return;
  }
  
  var now=new Date();
  var time_off=(clockd.hasOwnProperty("timeoffset"))?clockd["timeoffset"]:0;
  now.setTime(now.getTime()+time_off*1000);
  var day=now.getDate();
  var year=now.getFullYear();
  var month=now.getMonth()+1;

  if((month)<10){month="0"+(month);}
  if(day<10){day="0"+day;}

  cns.lineWidth=1;
  cns.fillStyle=clockd["date_add_color"];
  cns.textBaseline="middle";
  cns.textAlign="center";
  cns.font=size/20+"px Arial";

  switch(parseInt(clockd["date_add"]))
  {
    case 1:
      cns.fillText(day+"/"+month+"/"+year,x,y);
    break;
    case 2:
      cns.fillText(month+"/"+day+"/"+year,x,y);
    break;
    case 3:
      day=now.getDay();
      cns.fillText(day_arr[day],x,y);
    break;
    case 4:
      month=now.getMonth();
      cns.fillText(month_arr[month]+" "+day,x,y);
    break;
    default:
      month=now.getMonth();
      cns.fillText(day+" "+month_arr[month],x,y);
  }
}
