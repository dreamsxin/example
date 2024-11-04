## 补丁
```json
{"navigator.userAgent":"Mozilla/15.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36","geolocation:latitude":116.407957,"geolocation:longitude":39.887764,"debug":true}
```

## `fingerprint-injection.patch`
### navigator.
- userAgent
- appCodeName
appVersion
appName
languages
language
platform
oscpu
product
productSub
pdfViewerEnabled
cookieEnabled
onLine
buildID
doNotTrack
globalPrivacyControl
hardwareConcurrency
maxTouchPoints
globalPrivacyControl

### window.
innerWidth
innerHeight
outerWidth
outerHeight
screenX
screenY
devicePixelRatio
scrollMinX
scrollMinY
scrollMaxX
scrollMaxY
history.length

### screen.
pageXOffset
pageYOffset
colorDepth
pixelDepth
screen.availLeft, screen.availTop, screen.availWidth, screen.availHeight

### battery:
charging
dischargingTime
chargingTime
level

## `network-patches.patch`
### headers.
User-Agent
Accept-Language
Accept-Encoding

### locale:
language
region
script

`timezone-spoofing.patch`
### timezone

## `screen-hijacker.patch`
### screen.
height

## `anti-font-fingerprinting.patch`

字体

## `audio-context-spoofing.patch`

### AudioContext:
sampleRate
outputLatency
maxChannelCount

## `font-hijacker.patch`
### fonts

数组

## `geolocation-spoofing.patch`
### geolocation:
latitude
longitude
accuracy


```js
const options = {
  enableHighAccuracy: true,
  timeout: 5000,
  maximumAge: 0,
};

function success(pos) {
  const crd = pos.coords;

  console.log("Your current position is:");
  console.log(`Latitude : ${crd.latitude}`);
  console.log(`Longitude: ${crd.longitude}`);
  console.log(`More or less ${crd.accuracy} meters.`);
}

function error(err) {
  console.warn(`ERROR(${err.code}): ${err.message}`);
}

navigator.geolocation.getCurrentPosition(success, error, options);
```

## `webgl-spoofing.patch`
### webGl: / webGl2:
renderer
vendor
shaderPrecisionFormats:blockIfNotDefined
supportedExtensions

### webGl2:contextAttributes / webGl:contextAttributes
`GetAttribute` 获取
{
	alpha
	depth
	stencil
	antialias
}
webGl:contextAttributes.premultipliedAlpha
preserveDrawingBuffer
failIfMajorPerformanceCaveat
powerPreference

### webGl2:parameters / webGl:parameters
`MParamGL` 获取
- ret[LOCAL_GL_BLEND] = false;
- ret[LOCAL_GL_CULL_FACE] = false;
- ret[LOCAL_GL_DEPTH_TEST] = false;
- ret[LOCAL_GL_DITHER] = true;
- ret[LOCAL_GL_POLYGON_OFFSET_FILL] = false;
- ret[LOCAL_GL_SAMPLE_ALPHA_TO_COVERAGE] = false;
- ret[LOCAL_GL_SAMPLE_COVERAGE] = false;
- ret[LOCAL_GL_SCISSOR_TEST] = false;
- ret[LOCAL_GL_STENCIL_TEST] = false;
- ret[LOCAL_GL_RASTERIZER_DISCARD] = false;

`GLParam`
- LOCAL_GL_DEPTH_RANGE:
- LOCAL_GL_ALIASED_POINT_SIZE_RANGE:
- LOCAL_GL_ALIASED_LINE_WIDTH_RANGE:
- LOCAL_GL_COLOR_CLEAR_VALUE:
- LOCAL_GL_BLEND_COLOR:
- LOCAL_GL_MAX_VIEWPORT_DIMS:
- LOCAL_GL_SCISSOR_BOX:
- LOCAL_GL_VIEWPORT:
- LOCAL_GL_COMPRESSED_TEXTURE_FORMATS:
- LOCAL_GL_COLOR_WRITEMASK:
- dom::WEBGL_debug_renderer_info_Binding::UNMASKED_RENDERER_WEBGL:
- dom::WEBGL_debug_renderer_info_Binding::UNMASKED_VENDOR_WEBGL:

## `webrtc-ip-spoofing.patch`
### webrtc:
ipv4
ipv6
