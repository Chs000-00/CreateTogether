const net = require('node:net');

// Set of all currently connected sockets
const connectedSockets = new Set();

const PORT = 24018;
const ADDRESS = '127.0.0.1';
const TIMEOUT = 10000;
const USE_ENCODING = true;
const ENCODING = 'utf-8'

console.log("Starting test server at port " + PORT);

// broadcast to all connected sockets except one
connectedSockets.broadcast = function(data, except) {

    var bufferNull = new Buffer([0x00])
    buf = new Buffer(data)
    buffer = Buffer.concat([buf, bufferNull]);

    for (let sock of this) {
        if (sock !== except) {
            sock.write(buffer);   
        }
    }
}

const server = net.createServer(function(sock){
    console.log('New client connected address: ' + sock.address().address);

    if (USE_ENCODING) {
        sock.setEncoding(ENCODING);
    }
    
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