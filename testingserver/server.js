const net = require('node:net');

// Set of all currently connected sockets
const connectedSockets = new Set();

const PORT = 24018;
const ADDRESS = '127.0.0.1';
const TIMEOUT = 10000;
const ENCODING = 'utf-8'

console.log("Starting test server...");

// broadcast to all connected sockets except one
connectedSockets.broadcast = function(data, except) {
    for (let sock of this) {
        if (sock !== except) {
            var bufferNull = new Buffer([0x00])
            buf = new Buffer(data)
            buffer = Buffer.concat([buf, bufferNull]);
            sock.write(buffer);   
        }
    }
}

const server = net.createServer(function(sock){
    console.log('New client connected address:' + sock.address().address);
    sock.setEncoding(ENCODING);
    sock.setTimeout(TIMEOUT);

    connectedSockets.add(sock);

    sock.on('end', function() {
        connectedSockets.delete(sock);
    });

    sock.on('data', function(data) {
        console.debug(`Server received data: ${JSON.stringify(data)}`);

        connectedSockets.broadcast(data, sock);
    });

}); 


server.listen(PORT, ADDRESS);