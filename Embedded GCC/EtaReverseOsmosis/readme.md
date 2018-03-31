# Reverse osmosis controller
I've made a decision to make this controller after a four-way valve in my system was out of order and went to rubbish bin :relieved:. So i bought a couple of 12v valves on ali and .... and here you can see the result.


## Schematic
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaReverseOsmosis/schematic.png)


## Routed PCB
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaReverseOsmosis/pcb.png)


## Prototype device
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaReverseOsmosis/prototype_device.jpg)


## Installed and ready to use device
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaReverseOsmosis/installed_device.jpg)

It work pretty simple: when pressure gauge of accumulator tank triggers (it is setup to half of full pressure), it starts filtering. Filtering is a three step procedure: clean membrane, filter watter until pressure gauge is off again (reached half of full pressure) and then 45 minutes after that, clean membrane again. Also there is a low pressure gauge at water inlet, which triggers the device off (ignoring tank pressure gauge).

The cleaning procedure before and after watter filtering, may slightly prolong the life of the membrane thus reducing the cost of replacements.

## Small illustration of how it cleans the membrane

[![Alt text for your video](https://img.youtube.com/vi/6y_FyEgpJkM/0.jpg)](https://youtu.be/6y_FyEgpJkM)

I'm very satisfied with the device: it fulfills all my needs and expectation. It works in my system almost 3 years from 2016, and it never fails.

## What's next?

I'm planing to add analog pressure gauge support for tank, thus allowing to filter the watter until the pressure stops to increase. Also I'm planning to add a flowmeter to control how much watter have been filtered so far, and signal when it's time to replace cartridges.