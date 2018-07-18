# Custom firmware for S12S e-bike controller
It is a simple an useful in practice tool to investigate how a particular AC load behaves. The tool precisely measures voltage, frequency, apparent, active and effective powers, **cos** φ. It also measure the overall power taken or given (reverse) to or from AC load.

**Sinusoidal Commutation in the Real World: The Phase-to-Phase Method**

Pure sinusoidal drive voltages sound great in theory, but are rarely used in practical designs because they’re inefficient to generate for each motor winding with respect to ground. The preferred approach  is to generate three sinusoidal voltages *between* the three phases. This is done by varying the PWM duty cycle (and hence drive voltages) relative to ground using the characteristic profile in *Figure 3*, rather than a pure sinusoid. When any phase is measured with respect to ground, the waveform is sinusoidally coupled with third-order harmonics; the phase current driving the motor follows the pure sine-wave profile of the phase-to-phase voltage.

## Prototype device
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_ACPowerMetter/prototype_device1.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_ACPowerMetter/prototype_device2.jpg)

## Installed and ready to use device
![schematic capture](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_ACPowerMetter/device_in_work1.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_ACPowerMetter/device_in_work2.jpg)

## Under construction :construction:
The project is not finished. There also will be a PC software ([GitHub](https://github.com/SwInDaMix/sw-hub/tree/master/CS/DevTool_ACPowerMetter))
