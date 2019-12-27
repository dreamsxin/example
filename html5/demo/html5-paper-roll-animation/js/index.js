(function(){
  var cont = document.querySelector('#cont');
  
  document.addEventListener('mousemove', updRotation, false);
  
  function updRotation(e){
    document.querySelector('#msg').style.opacity = 0;
    cont.style.webkitTransform = 'rotateY(' + e.x / 10 + 'deg) rotateZ(-' + e.y / 10 + 'deg)';   
    cont.style.transform = 'rotateY(' + e.x / 10 + 'deg) rotateZ(-' + e.y / 10 + 'deg)';   
  }
})();