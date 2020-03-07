var http = require('http');

// IP and PORT of UDP server
var SERVER_IP = '18.195.223.18'
var SERVER_PORT = 42693

// Listen port of websocket client connection 
var CLIENT_WEBSOCKET_LISTEN = 2900;

// Hostname and port of master server
var MASTER_SERVER = 'cspsp.appspot.com';
var MASTER_PORT = 80;

// Listen port of http client connection
var CLIENT_HTTP_LISTEN = 2800;



// Http proxy
var http = require('http'),
    httpProxy = require('http-proxy');

var options = {
  target: {
    host: MASTER_SERVER,
    port: MASTER_PORT
  },
}

var proxy = httpProxy.createProxyServer(options).listen(CLIENT_HTTP_LISTEN); 
proxy.on('proxyReq', function(proxyReq, req, res, options) {
  proxyReq.setHeader('Host', MASTER_SERVER);
});

var enableCors = function(req, res) {
	if (req.headers['access-control-request-method']) {
		res.setHeader('access-control-allow-methods', req.headers['access-control-request-method']);
	}

	if (req.headers['access-control-request-headers']) {
		res.setHeader('access-control-allow-headers', req.headers['access-control-request-headers']);
	}

	if (req.headers.origin) {
		res.setHeader('access-control-allow-origin', req.headers.origin);
		res.setHeader('access-control-allow-credentials', 'true');
	}
};

// set header for CORS
proxy.on("proxyRes", function(proxyRes, req, res) {
	enableCors(req, res);
});


// websocket >> UDP proxy
var Buffer = require('buffer').Buffer;
var dgram = require('dgram');
var WebSocketServer = require('ws').Server;

var wss = new WebSocketServer({port: CLIENT_WEBSOCKET_LISTEN});

wss.on('connection', function(ws) {
    //Create a udp socket for this websocket connection
    var udpClient = dgram.createSocket('udp4');

    //When a message is received from udp server send it to the ws client
    udpClient.on('message', function(msg, rinfo) {
        ws.send(msg.toString());
	console.log("UDP: server > client");
    });

    //When a message is received from ws client send it to udp server.
    ws.on('message', function(message) {
        var msgBuff = new Buffer(message);
        udpClient.send(msgBuff, 0, msgBuff.length, SERVER_PORT, SERVER_IP);
	console.log("UDP: client > server");
    });
});
