
const CloudTuya = require("cloudtuya");
const Config = require("./config.js");

var api = null;
var initialized = false;


async function init()
{
   api = new CloudTuya({
               userName: Config.userName,
               password: Config.password,
               bizType: Config.bizType,
               countryCode: Config.countryCode,
               region: Config.region,
            });
  
  const tokens = await api.login();

  initialized = true;
}

async function device_set_state(id, state) 
{
   console.log("Set device: " + id + " -> " + state);
   
   const resp = await api.setState({
      devId: id,
      setState: state === "1" ? 1 : 0,
    });
  
    if (resp.header.code !== "SUCCESS") 
    { 
      throw new Exception("Set device state failed");
    }
}


exports.init = init;
exports.device_set_state = device_set_state;
