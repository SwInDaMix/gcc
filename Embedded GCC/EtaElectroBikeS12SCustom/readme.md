# Custom firmware for S12S e-bike controller
The idea to make this firmware is to support popular e-bike controllers like S06S and S12S.

**Sinusoidal Commutation in the Real World: The Phase-to-Phase Method**

Pure sinusoidal drive voltages sound great in theory, but are rarely used in practical designs because they’re inefficient to generate for each motor winding with respect to ground. The preferred approach  is to generate three sinusoidal voltages *between* the three phases. This is done by varying the PWM duty cycle (and hence drive voltages) relative to ground using the characteristic profile in *figure below*, rather than a pure sinusoid. When any phase is measured with respect to ground, the waveform is sinusoidally coupled with third-order harmonics; the phase current driving the motor follows the pure sine-wave profile of the phase-to-phase voltage.

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaElectroBikeS12SCustom/bldc_sinusoidal_control.png)

## Hardware mod for S12S controller
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaElectroBikeS12SCustom/harware_mode1.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaElectroBikeS12SCustom/harware_mode2.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaElectroBikeS12SCustom/harware_mode3.jpg)

![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/EtaElectroBikeS12SCustom/harware_mode4.jpg)


