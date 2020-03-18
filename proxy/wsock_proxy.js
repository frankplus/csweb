#!/usr/bin/env node

const fs = require('fs');

// Listen port of websocket client connection 
const CLIENT_WEBSOCKET_LISTEN = 2900;

const KEY_PATH = 'frankinfotech.it.key';
const CRT_PATH = 'frankinfotech.it.crt';


// websocket >> UDP proxy
var Buffer = require('buffer').Buffer;
var dgram = require('dgram');
const WebSocket = require('ws');
const https = require('https');

const server = https.createServer({
	cert: fs.readFileSync(CRT_PATH),
	key: fs.readFileSync(KEY_PATH)
  });

var wss = new WebSocket.Server({ server });

// var wss = new WebSocket.Server({ port: CLIENT_WEBSOCKET_LISTEN }); // no ssl

wss.on('connection', function (ws) {
	//Create a udp socket for this websocket connection
	var udpClient = dgram.createSocket('udp4');

	// IP and PORT of UDP server
	var SERVER_IP = '52.59.187.247';
	var SERVER_PORT = 42692;

	//When a message is received from udp server send it to the ws client
	udpClient.on('message', function (msg, rinfo) {
		ws.send(msg.subarray());
		// console.log("UDP: server > client");
		// console.log(msg);
	});

	//When a message is received from ws client send it to udp server.
	ws.on('message', function (message) {
		var msgBuff = new Buffer(message);

		if(message[0] == 67) { // CONN $IP $PORT
			if(message.toString('utf8', 0, 4) == "CONN") {
				var split = message.toString().split(" ");
				SERVER_IP = split[1];
				SERVER_PORT = parseInt(split[2]);
			} else {
				console.log("packet with first byte 67 but not a connect packet.");
			}
		}
		else{
			udpClient.send(msgBuff, 0, msgBuff.length, SERVER_PORT, SERVER_IP);
			// console.log("client > server");
			// console.log(message);
		}
	});
});

server.listen(CLIENT_WEBSOCKET_LISTEN);