/**
 * \file main.js        \brief Main entry point
 */

const config = require('./config.js');
var snmp = require ("net-snmp");
var net = require('net');


///////////////////
// Miners control
///////////////////

/** Send request to miner */
async function sendRequest(host, port, request) {

    var client = new net.Socket();
    client.connect(port, host, function() {
        console.log("Connected to " + host);
        client.write(request);
        console.log("Send: " + request);
    });
    
    client.on('data', function(data) {
        console.log('Received: ' + data);
        client.destroy();   // kill client after server's response
    });
    
    client.on('close', function() {
        console.log('Connection closed');
    });

    client.on('error', function(e) {
        console.log(e);
    });
}


async function minerEnable(host, port, enable) {

    var request = {
        "id": 0,
        "jsonrpc" : "2.0",
        "method" : "control_gpu", 
        "params" : [-1, enable ? 1 : 0]};

    await sendRequest(host, port, JSON.stringify(request) + "\n");
}

async function allMinersEnable(enable) {

    console.log("***** ALL MINERS -> " + (enable ? "ON" : "OFF") + "*****");

    for (let miner of config.miners) {
        console.log(miner.name + " -> " + (enable ? "ON" : "OFF"));
        minerEnable(miner.host, miner.port, enable);
    }
}


////////////////////////////////
// Load power process control
////////////////////////////////

var lpstate = 0;
var prevLoadPower = 0;
var delay_cycles = 0;

async function processLoadPower(loadPower) {

    if (loadPower != prevLoadPower) {
        process.stdout.write("\nLoadPower: " + loadPower + "   lpstate: " + lpstate + "  delay_cycles: " + delay_cycles);
        prevLoadPower = loadPower;
    } else {
        process.stdout.write(".");
    }

    switch(lpstate) 
    {
        case 0: 
        {
            if (loadPower >= config.powerHighLimit) {
                // Disable miners
                await allMinersEnable(false);
                lpstate = 1;
            }
        }
        break;

        case 1:
        {
            if (loadPower <= config.powerLowLimit) {
		// Wait some time to try enable miners
		delay_cycles = config.powerHighDelayTime / config.snmp.refresh_interval;
                lpstate = 2;
            }
        }
        break;

	case 2:
	{
		delay_cycles--;
		if (delay_cycles == 0) {
	            if (loadPower <= config.powerLowLimit) {
                	// Enable miners
                	await allMinersEnable(true);
                	lpstate = 0;
		    } else {
		       lpstate = 1;
		    }
		}
	}
	break;
    }

}

async function power_status_timer_callback(arg) {

    // Default options
    var options = {
        port: 161,
        retries: 1,
        timeout: 5000,
        backoff: 1.0,
        transport: "udp4",
        trapPort: 162,
        version: snmp.Version1,
        backwardsGetNexts: true,
        idBitsSize: 32
    };    

    var oids = ["1.3.6.1.4.1.21111.1.2.5.5.1.3.0"];
    var session = snmp.createSession (config.snmp.host, "private", options);

    await session.get(oids, async function (error, varbinds) {
        if (error) {
            console.error (error);
        } else {
            for (var i = 0; i < varbinds.length; i++) {
                if (snmp.isVarbindError (varbinds[i])) {
                    console.error (snmp.varbindError (varbinds[i]));
                } else {
                    //console.log (varbinds[i].oid + " = " + varbinds[i].value);
                    await processLoadPower(varbinds[i].value);
                }
            }
        }
        session.close ();
    });
}

async function main()
{
    allMinersEnable(true);
    setInterval(power_status_timer_callback, config.snmp.refresh_interval);
    console.log("POWER monitor is running ...");
}

// Entry
main();

