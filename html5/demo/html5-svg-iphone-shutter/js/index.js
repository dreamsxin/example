// JavaScript Document

var shutterAll = document.getElementsByClassName('shutter');

var shutter01 = document.getElementsByClassName('no-1');
var shutter02 = document.getElementsByClassName('no-2');
var shutter03 = document.getElementsByClassName('no-3');
var shutter04 = document.getElementsByClassName('no-4');
var shutter05 = document.getElementsByClassName('no-5');
var shutter06 = document.getElementsByClassName('no-6');

//Resets

TweenLite.to(shutter01, 0, {transformOrigin:"150% 0%"});
TweenLite.to(shutter02, 0, {transformOrigin:"150% 0%"});
TweenLite.to(shutter03, 0, {transformOrigin:"150% 0%"});
TweenLite.to(shutter04, 0, {transformOrigin:"150% 0%"});
TweenLite.to(shutter05, 0, {transformOrigin:"150% 0%"});
TweenLite.to(shutter06, 0, {transformOrigin:"150% 0%"});

TweenLite.to(shutter02, 0, {rotationZ: 60});
TweenLite.to(shutter03, 0, {rotationZ: 120});
TweenLite.to(shutter04, 0, {rotationZ: 180});
TweenLite.to(shutter05, 0, {rotationZ: 240});
TweenLite.to(shutter06, 0, {rotationZ: 300});

//Timeline

var masterTimeline = new TimelineMax({repeat:-1, yoyo:true, repeatDelay:2});
var shutterAni = new TimelineMax({});


shutterAni
//Fully open
.to(shutter01, 0.5, {rotationZ: "+=27"}, "open")
.to(shutter02, 0.5, {rotationZ: "+=27"}, "open")
.to(shutter03, 0.5, {rotationZ: "+=27"}, "open")
.to(shutter04, 0.5, {rotationZ: "+=27"}, "open")
.to(shutter05, 0.5, {rotationZ: "+=27"}, "open")
.to(shutter06, 0.5, {rotationZ: "+=27"}, "open")


//Closed
.to(shutter01, 0.5, {rotationZ: "-=27"}, "close+=2")
.to(shutter02, 0.5, {rotationZ: "-=27"}, "close+=2")
.to(shutter03, 0.5, {rotationZ: "-=27"}, "close+=2")
.to(shutter04, 0.5, {rotationZ: "-=27"}, "close+=2")
.to(shutter05, 0.5, {rotationZ: "-=27"}, "close+=2")
.to(shutter06, 0.5, {rotationZ: "-=27"}, "close+=2")

//half
.to(shutter01, 0.5, {rotationZ: "+=12"}, "half")
.to(shutter02, 0.5, {rotationZ: "+=12"}, "half")
.to(shutter03, 0.5, {rotationZ: "+=12"}, "half")
.to(shutter04, 0.5, {rotationZ: "+=12"}, "half")
.to(shutter05, 0.5, {rotationZ: "+=12"}, "half")
.to(shutter06, 0.5, {rotationZ: "+=12"}, "half")



//Closed
.to(shutter01, 0.5, {rotationZ: "-=12"}, "close2")
.to(shutter02, 0.5, {rotationZ: "-=12"}, "close2")
.to(shutter03, 0.5, {rotationZ: "-=12"}, "close2")
.to(shutter04, 0.5, {rotationZ: "-=12"}, "close2")
.to(shutter05, 0.5, {rotationZ: "-=12"}, "close2")
.to(shutter06, 0.5, {rotationZ: "-=12"}, "close2")

//Fully open
.to(shutter01, 0.5, {rotationZ: "+=27"}, "open2")
.to(shutter02, 0.5, {rotationZ: "+=27"}, "open2")
.to(shutter03, 0.5, {rotationZ: "+=27"}, "open2")
.to(shutter04, 0.5, {rotationZ: "+=27"}, "open2")
.to(shutter05, 0.5, {rotationZ: "+=27"}, "open2")
.to(shutter06, 0.5, {rotationZ: "+=27"}, "open2")

;

masterTimeline.add(shutterAni)