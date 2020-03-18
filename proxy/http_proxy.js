#!/usr/bin/env node

const fs = require('fs');

// Hostname and port of master server
const MASTER_SERVER = 'cspsp.appspot.com';
const MASTER_PORT = 80;

// Listen port of http client connection
const CLIENT_HTTP_LISTEN = 2800;

const KEY_PATH = 'frankinfotech.it.key';
const CRT_PATH = 'frankinfotech.it.crt';

// Http proxy
var httpProxy = require('http-proxy');

var options = {
	target: {
		host: MASTER_SERVER,
		port: MASTER_PORT
	},
	ssl: {
		key: fs.readFileSync(KEY_PATH, 'utf8'),
		cert: fs.readFileSync(CRT_PATH, 'utf8')
	}
}

var proxy = httpProxy.createProxyServer(options).listen(CLIENT_HTTP_LISTEN);
proxy.on('proxyReq', function (proxyReq, req, res, options) {
	proxyReq.setHeader('Host', MASTER_SERVER);
});

var enableCors = function (req, res) {
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
proxy.on("proxyRes", function (proxyRes, req, res) {
	enableCors(req, res);
});