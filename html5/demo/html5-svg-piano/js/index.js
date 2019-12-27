const svg = document.createElementNS("http://www.w3.org/2000/svg", "svg")
const group = document.createElementNS("http://www.w3.org/2000/svg", "g")
const input = {}
const value = {}
Array.from(document.querySelectorAll(".ctrl input")).forEach(inp => {
  let val
  const name = inp.name
  input[name] = inp
  inp.addEventListener('input', change)
  inp.addEventListener('change', () => inp.value = val)
  change()

  function change() {
    val = Math.min(+inp.max, Math.max(+inp.value, +inp.min))
    const step = +inp.step || 1
    val = Math.round(val / step) * step
    value[name] = val
  }
})
const diam1 = 150
const diam2 = 215
const diam3 = 300
const whites = [0, null, 1, null, 2, 3, null, 4, null, 5, null, 6]
const keyNumber = new WeakMap()
let octaveCount = 3
let octaveLoop = 3

input.loop.addEventListener('input', function() {
  value.repeat = input.repeat.value = 1
})
group.classList.add("keyboard")

function angle(turn) {
  const rad = turn * Math.PI * 2
  const cos = Math.cos(rad)
  const sin = Math.sin(rad)
  return {
    rad,
    cos,
    sin
  }
}

function sharpPos(note) {
  if (note <= 5)
    return note * 3 / 7 / 5
  else
    return 3 / 7 + (note - 5) * 4 / 7 / 7
}

function draw() {
  octaveLoop = value.loop
  octaveCount = octaveLoop * input.repeat.value
  for (let octave = 0; octave < octaveCount; octave++) {
    for (let i = 0; i < 12; i++) {
      const white = whites[i]
      const a1 = angle((octave + sharpPos(i)) / octaveCount)
      const a2 = angle((octave + sharpPos(i + 1)) / octaveCount)
      const elm = document.createElementNS("http://www.w3.org/2000/svg", "path")
      if (i === 0) {
        elm.classList.add('ut')
        if (octave % octaveLoop === 0) {
          elm.classList.add('first-ut')
        }
      }
      let path = `\
M ${a1.cos * diam2} ${a1.sin * diam2}\
L ${a1.cos * diam3} ${a1.sin * diam3}\
A ${diam3} ${diam3} 0 0 1 ${a2.cos * diam3},${a2.sin * diam3}\
L ${a2.cos * diam2},${a2.sin * diam2}`
      if (white == null) {
        path += `A ${diam2} ${diam2} 0 0 0 ${a1.cos * diam2} ${a1.sin * diam2}`
        elm.classList.add("sharp")
      } else {
        const a0 = angle((octave + (white / 7)) / octaveCount)
        const a3 = angle((octave + ((white + 1) / 7)) / octaveCount)
        path += `\
A ${diam2} ${diam2} 0 0 1 ${a3.cos * diam2},${a3.sin * diam2}\
L ${a3.cos * diam1},${a3.sin * diam1}\
A ${diam1} ${diam1} 0 0 0 ${a0.cos * diam1},${a0.sin * diam1}\
L ${a0.cos * diam2},${a0.sin * diam2}\
A ${diam2} ${diam2} 0 0 1 ${a1.cos * diam2},${a1.sin * diam2}`
      }
      elm.setAttribute("d", path)
      group.appendChild(elm)
      keyNumber.set(elm, (i + octave * 12) % (12 * octaveLoop))
    }
  }
  svg.appendChild(group)
  document.body.appendChild(svg)
}
draw()
input.loop.addEventListener("input", draw)
input.repeat.addEventListener("input", draw)

function resize() {
  const vw = window.innerWidth
  const vh = window.innerHeight
  const min = Math.min(vw, vh)
  const scale = min / diam3 / 2
  const transform = `translate(${vw / 2}, ${vh / 2}) scale(${scale})`
  group.setAttribute('transform', transform)
}
resize()
window.addEventListener("resize", resize)
group.addEventListener('mouseover', e => play(keyNumber.get(e.target)))
group.addEventListener('mouseleave', stop)

const ac = new AudioContext()
const volume = ac.createGain()
let wave
let oldNote = null
volume.gain.value = 0
volume.connect(ac.destination)
const osc = ac.createOscillator()

function createWave() {
  const max = 13
  const real = new Float32Array(Math.pow(2, max))
  for (let i = 0; i < max; i += octaveLoop) {
    real[Math.pow(2, i)] = 1
    real[Math.pow(2, i) * 3] = value.fifth
    real[Math.pow(2, i) * 5] = value.third
  }
  wave = ac.createPeriodicWave(real, real)
  osc.setPeriodicWave(wave)
}
input.loop.addEventListener("input", createWave)
input.fifth.addEventListener("input", createWave)
input.third.addEventListener("input", createWave)
createWave()
osc.connect(volume)
osc.start()

function play(note) {
  volume.gain.value = .3
  if (oldNote === null || value.slide === 0) {
    const freq = 440 * (Math.pow(2, (note - 9) / 12)) / 32
    osc.frequency.setValueAtTime(freq, ac.currentTime)
    oldNote = note
    return
  }
  const note2 = note
  if (oldNote !== null) {
    while (note - oldNote > 6)
      note -= 12
    while (note - oldNote < -6)
      note += 12
  }
  const oldFreq = 440 * (Math.pow(2, (oldNote - 9) / 12)) / 32
  const freq = 440 * (Math.pow(2, (note - 9) / 12)) / 32
  osc.frequency.cancelScheduledValues(ac.currentTime)
  osc.frequency.setValueAtTime(oldFreq, ac.currentTime)
  osc.frequency.linearRampToValueAtTime(freq, ac.currentTime + value.slide)
  oldNote = note2
}

function stop() {
  volume.gain.value = 0
  oldNote = null
}