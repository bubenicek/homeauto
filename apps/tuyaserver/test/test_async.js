/**
 * \file main.js        \brief Main entry point
 */

const request = require('./request')

//const CloudTuya = require('cloudtuya');
//const TuyaDevice = require('tuyapi');

async function test_asyncwait()
{
	let {response, body} = await request.get('http://www.google.com');
   if (response.statusCode != 200)
	{ 
		console.log('error');
	}
   else 
	{ 
      console.log('fetched response');
	}
}

async function main() 
{
    console.log("Waiting ...");
    try {
      await init();
    } catch(e)
    {
       console.error(e);
       process.exit();
    }
    console.log("Done");
}

main();
