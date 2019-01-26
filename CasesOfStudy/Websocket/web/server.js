var WebSocketServer = require('ws').Server

wss = new WebSocketServer({ port: 6363 });
console.log("INIT");

wss.on('connection', function connection(ws) {
    console.log("nueva coneccion:"+ws)
    ws.send('Conectado');
    ws.on('message', function incoming(message) {
        console.log('received: %s', message);
        ws.send(message);
    });
    
});