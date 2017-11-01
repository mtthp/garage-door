const RtmClient = require('@slack/client').RtmClient
const CLIENT_EVENTS = require('@slack/client').CLIENT_EVENTS
const RTM_EVENTS = require('@slack/client').RTM_EVENTS
const util = require('util')
const http = require('http')

require('dotenv-extended').load({
    path: __dirname + '/config/.env',
    defaults: __dirname + '/config/.env.defaults',
    errorOnMissing: false,
    errorOnExtra: false,
    assignToProcessEnv: true,
    overrideProcessEnv: false
})

var rtm = new RtmClient(process.env.SLACK_BOT_TOKEN)

/**
 * Should call a endpoint which activate the controller
 * eg. http://SOME-IP-ADDRESS/toggle
 * @return Promise
 */
function toggleGarageDoor() {
  return new Promise(function(resolve, reject) {
    console.log(">> " + process.env.TOGGLE_URI)
    var request = http.get(process.env.TOGGLE_URI, function (response) {
      let data = ''
      response.on('data', function (chunk) {
        data += chunk
      })
      // The whole response has been received. Print out the result.
      response.on('end', () => {
        console.log("<< " + process.env.TOGGLE_URI)
        console.log(data)
        resolve(data)
      })
    }).on("error", (err) => {
      reject(err)
    })
    request.on('socket', function (socket) {
      socket.setTimeout(parseInt(process.env.HTTP_TIMEOUT)); 
      socket.on('timeout', function() {
        request.abort()
      })
    })
  })
}

const responses = [
  'Message heard loud and clear',
  'Sure thing %s !',
  'Of course %s',
  'Consider it done %s',
  'Yes master'
]

// The client will emit an RTM.AUTHENTICATED event on successful connection, with the `rtm.start` payload if you want to cache it
rtm.on(CLIENT_EVENTS.RTM.AUTHENTICATED, function (rtmStartData) {
  console.log(`Logged in as ${rtmStartData.self.name} of team ${rtmStartData.team.name}, but not yet connected to a channel`)
})

rtm.on(RTM_EVENTS.MESSAGE, function handleRtmMessage(message) {
  console.log('Received message:', message) //this is no doubt the lamest possible message handler, but you get the idea
  toggleGarageDoor()
    .then(function(body) {
      let response = responses[Math.floor(Math.random() * responses.length)]
      if (response.indexOf('%s') >= 0) response = util.format(response, "<@" + message.user + ">")
      rtm.sendMessage(response, message.channel)
    })
    .catch(function(error) {
      rtm.sendMessage("Something went wrong : ```" + error + "```", message.channel)
    });
  
})

rtm.start()