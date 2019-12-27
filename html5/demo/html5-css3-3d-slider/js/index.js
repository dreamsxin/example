function initInputs() {
  var allInputs = document.body.querySelectorAll(".bar-input");

  for (var i = 0; i < allInputs.length; i++) {
    var input = allInputs[i];
    var barId = input.parentNode.id;
    var styleEl = document.head.appendChild(document.createElement("style"));

    if (i == allInputs.length - 1) {
      //set indicator
      var indicator=input.parentNode.querySelector('.bar .indicator');
      setBarIndicator(barId, input, styleEl, indicator);
      input.oninput = setBarIndicator.bind(this, barId, input, styleEl, indicator);
      input.onchange = setBarIndicator.bind(this, barId, input, styleEl, indicator);
    } else {
      setBar(barId, input, styleEl);
      input.oninput = setBar.bind(this, barId, input, styleEl);
      input.onchange = setBar.bind(this, barId, input, styleEl);
    }
  }
}

function setBar(barId, input, styleEl) {
  styleEl.innerHTML =
    "#" + barId + " .bar-face.percentage:before {width:" + input.value + "%;}";
}

function setBarIndicator(barId, input, styleEl, indicatorEl) {
  styleEl.innerHTML =
    "#" + barId + " .bar-face.percentage:before {width:" + input.value + "%;}";
  indicatorEl.style.marginLeft = (input.value - 10) + '%';
  indicatorEl.textContent = input.value + '%';
}

initInputs();