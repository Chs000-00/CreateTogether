const net = require('node:net');

// Set of all currently connected sockets
const connectedSockets = new Set();

// broadcast to all connected sockets except one
connectedSockets.broadcast = function(data, except) {
    for (let sock of this) {
        if (sock !== except) {
            sock.write(data);
        }
    }
}

const server = net.createServer(function(sock){
    console.log('New client connected');
    connectedSockets.add(sock);

    sock.on('end', function() {
        connectedSockets.delete(sock);
    });

    sock.on('data', function(data) {
        console.log(`Server received data: ${JSON.stringify(data)}`);

        connectedSockets.broadcast(data, sock);
    });

}); 

server.listen(24018, '127.0.0.1');