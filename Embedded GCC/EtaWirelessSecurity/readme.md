# Home security
This is a project I'm currently working on: home security governor. Main knots of it is alarm with motion sensor, smoke sensors and electric lock, controlled by wireless remote key.


## Remote Key schematic 
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_schematic.png)


## Remote Key routed PCB
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_pcb.png)


## Remote Key prototype device
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_prototype_device_front.jpg)

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_prototype_device_rear.jpg)

## Main control software

Software is at [GitHub](https://github.com/SwInDaMix/sw-hub/tree/master/CS/EtaSecurityGovernorManager).

It will be used to setup and control all the units of the system, including registered remote keys and their permission:

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/main_control_registered_remote_keys.png)

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/main_control_registered_remote_key_permissions.png)

## Wireless key protocol

Communication of governor and remote key is encrypted by private 256-bit keys, those key is exchanged on registration. Any request is a challenge to the governor from the remote key and only registered one can win it :stuck_out_tongue_winking_eye::

![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/main_control_governor_remote_key_communication.png)

After a challenge won, governor may initiate a key replace procedure thus making the private key non-static. This adds additional reliability to protocol security.

## It's not finished yet !!!

It's a lot work waiting to be done, but bold start has been made.