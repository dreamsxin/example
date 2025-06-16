# Mercure in a Few Words
Mercure is an open solution for real-time communications designed to be fast, reliable and battery-efficient. It is a modern and convenient replacement for both the Websocket API and the higher-level libraries and services relying on it.

Mercure is especially useful to add streaming and asynchronous capabilities to REST and GraphQL APIs. Because it is a thin layer on top of HTTP and SSE, Mercure is natively supported by modern web browsers, mobile applications and IoT devices.

A free (as in beer, and as in speech) reference server, a commercial High Availability version and a hosted service are available.

- https://github.com/dunglas/mercure

## Subscribing
Subscribing to updates from a web browser or any other platform supporting Server-Sent Events is straightforward:
```javascript
// The subscriber subscribes to updates for the https://example.com/users/dunglas topic
// and to any topic matching https://example.com/books/{id}
const url = new URL("https://localhost/.well-known/mercure");
url.searchParams.append("topic", "https://example.com/books/{id}");
url.searchParams.append("topic", "https://example.com/users/dunglas");
// The URL class is a convenient way to generate URLs such as https://localhost/.well-known/mercure?topic=https://example.com/books/{id}&topic=https://example.com/users/dunglas

const eventSource = new EventSource(url);

// The callback will be called every time an update is published
eventSource.onmessage = (e) => console.log(e); // do something with the payload
```
