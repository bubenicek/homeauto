/**
 * \file config.js      \title Configuration / constants
 */


module.exports = {

    powerHighLimit: 4100,
    powerLowLimit: 2000,
    powerHighDelayTime : 30000,

    snmp : {
        host : "192.168.8.11",
        refresh_interval : 250,
    },

    miners : [
        {
            name: "Miner01",
            host: "192.168.8.20",
            port: 3333
        },
        {
            name: "Miner02",
            host: "192.168.8.3",
            port: 3333
        },
        {
            name: "Miner05",
            host: "192.168.8.6",
            port: 3335
        }
    ]

}

