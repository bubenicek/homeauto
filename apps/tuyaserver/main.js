/**
 * \file main.js        \brief Main entry point
 */

const express = require('express');
const https = require('https');
const fs = require('fs');

const Config = require("./config.js");
const tuya = require("./tuya.js");


async function main() 
{
	 await tuya.init();

    const app = express();
    
    // Configure express midlewares
    app.use(express.json());    // Parse JSON body
    app.use(express.static(__dirname + '/www'));    //Store all HTML files in view folder.

    // Register JSON API request handlers
    app.get('/api/system/info', (req, res) => {
        res.send('tuyaserver alive\r\n');
    });

    app.get('/api/device/set/:id/:state', (req, res) => {

		//console.log("Set device ID: " + req.params.id + " to state: " + req.params.state);
		
		tuya.device_set_state(req.params.id, req.params.state);

      res.send('\r\n');
    });

	if (Config.use_https) 
	{
   	// Start listenign https
		https.createServer({
  			key: fs.readFileSync('server.key'),
  			cert: fs.readFileSync('server.cert')
		}, app)
			.listen(Config.http_port, function () {
  			console.log('Listening on https port: ' + Config.http_port);
		})
	}
	else
	{
   	// Start listening http
   	app.listen(Config.http_port, () => console.log("Listening on port " + Config.http_port + " ..."));
	}
}

main();
