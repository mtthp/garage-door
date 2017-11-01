# garage-door
Open a garage door through a Slack bot

## Requirements
- 1 [Slack bot](https://citizencam.slack.com/apps/manage/custom-integrations)
- 1 [NodeJS](https://nodejs.org/en/) app to host the Slack bot
- 1 [ESP8266](https://en.wikipedia.org/wiki/ESP8266) (like the [Wemos D1 Mini](https://wiki.wemos.cc/products:d1:d1_mini)) to control 1 [SG90 Servo](https://servodatabase.com/servo/towerpro/sg90) 

## Installation
```
git clone https://github.com/mtthp/garage-door/
cd garage-door
npm install
```

## Start instructions
Change the env variables accordingly in the `config/.env` file, then
```
npm start
```

## FAQ
**Why host a NodeJS app instead of doing all the work on the ESP8266 (eg. connecting to Slack and managing the bot) ?**

Because some ESP8266 don't have enough memory to store/parse the Slack response messages (using the RTM API). It is inevitable that crashes will occur and the ESP will reboot or be unavailable from time to time.

## TODOs

- Draw some diagram to show a physical representation


## Licence
This project is licensed under the MIT license.
