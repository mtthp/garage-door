const RtmClient = require('@slack/client').RtmClient
const CLIENT_EVENTS = require('@slack/client').CLIENT_EVENTS
const RTM_EVENTS = require('@slack/client').RTM_EVENTS
const util = require('util')

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
 * Should call a endpoint which activate the door
 * eg. http://SOME-IP-ADDRESS/toggle
 */
function openGarageDoor() {
  return true
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
  console.log('Message:', message) //this is no doubt the lamest possible message handler, but you get the idea
  let response = responses[Math.floor(Math.random() * responses.length)]
  if (response.indexOf('%s') >= 0) response = util.format(response, "<@" + message.user + ">")
  rtm.sendMessage(response, message.channel)
})

rtm.start()