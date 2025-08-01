const { createServer } = require('http');
const next = require('next');
const net = require('net');
const WebSocket = require('ws');

const dev = process.env.NODE_ENV !== 'production';
const app = next({ dev });
const handle = app.getRequestHandler();

const TCP_PORT = 5000;
const WS_PORT = 4000;
const HTTP_PORT = 3000;

app.prepare().then(() => {
  // HTTP server for Next.js
  const httpServer = createServer((req, res) => {
    handle(req, res);
  });

  // --- WebSocket server on its own port ---
  const wss = new WebSocket.Server({ port: WS_PORT, host: '0.0.0.0' });

  wss.on('listening', () => {
    console.log(`✅ WebSocket server listening on port ${WS_PORT}`);
  });

  // --- TCP server to accept data from ESP ---
  const tcpServer = net.createServer((socket) => {
    console.log('🟢 TCP client connected from', socket.remoteAddress);

    socket.on('data', (data) => {
      const message = data.toString().trim();
      if (/^\d+,\d+$/.test(message)) {
        console.log('✅ Received:', message);
        wss.clients.forEach((client) => {
          if (client.readyState === WebSocket.OPEN) {
            client.send(message);
          }
        });
      } else {
        console.warn('❗ Invalid message:', message);
      }
    });

    socket.on('end', () => {
      console.log('🔌 TCP client disconnected');
    });

    socket.on('error', (err) => {
      console.error('🚫 TCP socket error:', err.message);
    });
  });

  // ✅ Listen on all interfaces
  tcpServer.listen(TCP_PORT, '0.0.0.0', () => {
    console.log(`✅ TCP Server listening on port ${TCP_PORT}`);
  });

  // --- Start Next.js HTTP server ---
  httpServer.listen(HTTP_PORT, '0.0.0.0', () => {
    console.log(`🚀 Next.js frontend available at http://localhost:${HTTP_PORT}`);
  });
});
