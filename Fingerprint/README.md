# What Is Browser Fingerprinting?

- https://github.com/abrahamjuliot/creepjs
- https://browserscan.net/
- https://github.com/HMaker/HMaker.github.io
- https://bot-detector.rebrowser.net/
- https://github.com/rebrowser/rebrowser-bot-detector
- https://github.com/TheGP/untidetect-tools

Browser fingerprinting is a process of identifying web clients by collecting specific data points from devices, HTTP connections, and software features.

Think of it this way: a real fingerprint image is a unique identifier that looks like a combination of lines and curves, right? Now, in the digital world, each line and curve encodes information from your end, including:

- Operating system type and language.
- Browser type, version, and extensions.
- Time zone.
- Language and fonts.
- Battery level.
- Keyboard layout.
- User-agent.
- CPU class.
- Navigator properties.
- Screen resolution.
- Other.
Although the details above are generic, it's extremely rare for two users to have a hundred percent matching data points. Each one generates a unique fingerprint, as even a slight variation results in a different outcome.

For example, hundreds of millions of users visit Google using Chrome. But how many use Chrome with a single extension in a 1366x768 resolution on an HP OMEN PC with 16 GB of RAM and 4GB Nvidia graphics on driver version 25.20.99.9221? Not many.

## Tool

https://github.com/snowplow/snowplow-javascript-tracker

## Fingerprinting Techniques
Sites use multiple methods to interact with web clients' properties in order to access and identify user data. Let's look at some of them!

### Canvas Fingerprinting
The HTML5 Canvas is an API with built-in objects, methods, and properties for drawing texts and graphics on a canvas. Browsers can leverage its features to render website content.

In 2012, Mowery and Shacham found that the final user-visible canvas graphic is directly affected by several factors, including:

- Operating system.
- Browser version.
- Graphics card.
- Installed fonts.
- Sub-pixel hinting.
- Antialiasing.
- 
That means the produced images and text differ depending on the device's graphical capabilities. As a result, browser fingerprinting websites request web clients to process canvas images. That generates valuable information that sites collect and store in their browser fingerprint database.

To learn more about how websites use HTML5 Canvas features to block web scrapers, check out our in-depth article on canvas fingerprinting (to be published soon).

### WebGL
Like HTML5 Canvas, WebGL is a graphics API specifically for rendering 3D interactive images.

Yes, you guessed it! These graphics are rendered differently per the device's graphical capabilities. So, browser fingerprinting websites can task a browser's WebGL API to produce 3D images to extract a device's features from the result.

### AudioContext
The Web Audio API is an interface for processing audio. Any device can generate audio signals and apply certain compression levels and filters by linking audio modules together to get a specific result.

AudioContext fingerprinting works the same way as HTML5 Canvas and WebGL. It processes audio signals present differences based on the device audio stack (software and hardware features).

That is a relatively novel method for collecting fingerprinting data, so only a few websites have scripts that implement the Web Audio API.

### Browser Extensions
Browser extensions are small software modules that extend the functionalities and features of a browser. Some of the most popular ones are ad blockers and VPNs.

That said, the most important element for browser-fingerprinting websites is how the extensions are integrated. Typically, they get some of their resources via the web. So, by checking for specific URLs, websites can query the presence or absence of an extension. For example, to get the logo of an extension, the site can use its knowledge of the device and follow a URL of the form extension://<extensionID>/<pathToFile> to retrieve it.

However, not every extension has such accessible resources, and not all are detectable using this technique.

### JavaScript Standards Conformance
Websites can also identify users based on their underlying JavaScript engine. That's possible because these engines execute JavaScript code differently depending on the browser features. Even subsequent browser versions present differences. Therefore, websites can test web clients to see if they compile according to the JavaScript standard. Then they analyze the supported features to extract fingerprinting data.

### CSS properties
Web browsers have different default values for specific CSS properties. By reading these properties' values, a website determines which browser is used to access the site. For example, most Firefox CSS properties contain the -Moz- prefix, while Safari and Chrome use the -WebKit- prefix.

### Fonts
One unpopular but effective browser fingerprinting technique is identifying users based on rendered texts because browsers on different devices render the same font style and character with different bounding boxes. Consequently, sites can query bounding boxes to collect fingerprinting data.

### Transport Layer Security (TLS)
When web clients send HTTPS requests to a website, they do so over Transport Layer Security. The web client must create a secure connection to enable communication, which involves exchanging valuable data with the web server via the TLS handshake. Websites use the information in the exchange to identify users, as these parameters differ per web client.

We covered TLS fingerprinting in detail, so let's focus on how to avoid browser fingerprinting this time.
