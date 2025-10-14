
const net = require('net');
const { SocksClient } = require('socks');


// The proxy URL to be parsed
const proxyUrl = 'http://xxx:xxx@proxyhost:port';

// Function to parse the proxy URL and create the upstreamProxy object
function createUpstreamProxy(url) {
  const parsedUrl = new URL(url);
  return {
    host: parsedUrl.hostname,
    port: parseInt(parsedUrl.port, 10),
    type: 5, // SOCKS5
    userId: parsedUrl.username,
    password: parsedUrl.password
  };
}

// Configuration for the upstream proxy
// const upstreamProxy = {
//   host: 'your_upstream_proxy_host', // Replace with your upstream proxy host
//   port: 1080, // Replace with your upstream proxy port
//   type: 5, // SOCKS5
//   userId: 'your_username', // Replace with your username
//   password: 'your_password' // Replace with your password
// };

const upstreamProxy = createUpstreamProxy(proxyUrl);

// Create a SOCKS5 server
const server = net.createServer((clientSocket) => {
  console.log('Client connected');

  let stage = 0;
  let targetHost, targetPort;

  clientSocket.on('data', async (data) => {
    try {
      if (stage === 0) {
        // Stage 0: SOCKS handshake
        // Client sends: [Version, NMethods, Methods]
        // We respond: [Version, Method]
        // For this example, we only support "no authentication"
        if (data[0] !== 0x05) {
          console.error('Unsupported SOCKS version');
          clientSocket.end();
          return;
        }
        clientSocket.write(Buffer.from([0x05, 0x00])); // No authentication
        stage = 1;
      } else if (stage === 1) {
        // Stage 1: Client request
        // Client sends: [Version, Command, RSV, AddressType, DestinationAddress, DestinationPort]
        const version = data[0];
        const command = data[1];
        const addressType = data[3];

        if (version !== 0x05 || command !== 0x01) { // We only support CONNECT command
          console.error('Unsupported command or version');
          clientSocket.end();
          return;
        }

        if (addressType === 0x01) { // IPv4
          targetHost = data.slice(4, 8).join('.');
          targetPort = data.readUInt16BE(8);
        } else if (addressType === 0x03) { // Domain name
          const domainLength = data[4];
          targetHost = data.slice(5, 5 + domainLength).toString();
          targetPort = data.readUInt16BE(5 + domainLength);
        } else {
          console.error('Unsupported address type');
          clientSocket.end();
          return;
        }

        console.log(`Request to: ${targetHost}:${targetPort}`);

        // Establish connection to the upstream proxy
        const options = {
          proxy: upstreamProxy,
          command: 'connect',
          destination: {
            host: targetHost,
            port: targetPort
          }
        };

        const { socket: upstreamSocket } = await SocksClient.createConnection(options);
        
        // Send success response to the client
        const response = Buffer.from([
          0x05, 0x00, 0x00, 0x01,
          0x00, 0x00, 0x00, 0x00, // Dummy address
          0x00, 0x00 // Dummy port
        ]);
        clientSocket.write(response);

        // Pipe data between client and upstream
        clientSocket.pipe(upstreamSocket);
        upstreamSocket.pipe(clientSocket);

        upstreamSocket.on('error', (err) => {
          console.error('Upstream socket error:', err);
          clientSocket.end();
        });

        stage = 2; // Data transfer stage
      }
    } catch (err) {
      console.error('Error during SOCKS negotiation:', err);
      clientSocket.end();
    }
  });

  clientSocket.on('error', (err) => {
    console.error('Client socket error:', err);
  });

  clientSocket.on('close', () => {
    console.log('Client disconnected');
  });
});

const PORT = 1080;
server.listen(PORT, '0.0.0.0', () => {
  console.log(`SOCKS5 proxy server listening on port ${PORT}`);
});
