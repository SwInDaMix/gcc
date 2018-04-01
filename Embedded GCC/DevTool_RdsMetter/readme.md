# Tool to measure resistance of an open FET channel (N-channel only)
It is a simple an useful in practice tool to determine Rds(on) of N-channel FET. Principle is simple: device loads an open FET channel with as much current as it can to gain enough voltage drop on it.


## Schematic
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/schematic.png)


## Routed PCB
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/pcb.png)


## Prototype device
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/prototype_device1.jpg)

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/prototype_device2.jpg)


## Installed and ready to use device
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/installed_device1.jpg)

### IRF830

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/installed_device2.jpg)

### IRFP4468

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_RdsMetter/installed_device3.jpg)

## What's next?

I've found one issue: when measuring a small FET in SOT95 package it may blow up from high current regardless of the fact it is applied by less then a second to it. So there need to be a second button like "Soft measure" with small current amount. Also need to add support to measure P-channel FETs.