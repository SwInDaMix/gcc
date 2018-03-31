# Home security
This is a project I'm currently working on: home security governor. Its main nodes are alarm with motion and smoke sensors and electric lock controlled by wireless remote key.

## Remote key schematic 
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_schematic.png)

## Remote key routed PCB
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_pcb.png)

## Remote key prototype device
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_prototype_device_front.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/remote_key_prototype_device_rear.jpg)

## Main control software

Software is at [GitHub](https://github.com/SwInDaMix/sw-hub/tree/master/CS/EtaSecurityGovernorManager).

It will be used to setup and control all the units of the system, including registered remote keys and their permission:

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/main_control_registered_remote_keys.png)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/main_control_registered_remote_key_permissions.png)

## Remote key protocol

Communication of governor and remote key is encrypted by private 256-bit keys, those key is exchanged on registration. Any request is a challenge to the governor from the remote key and only registered one can win it :stuck_out_tongue_winking_eye::

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaHomeSecurity/main_control_governor_remote_key_communication.png)

After a challenge won, governor may initiate a key replace procedure thus making the private key non-static. This adds additional reliability to protocol security.

## It's not finished yet !!!

It's a lot of work waiting to be done, but bold start has been made.
