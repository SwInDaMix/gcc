# Tool to measure capacitance of rechargeable batteries
Once I made this simple tool to log statistic of how LiFePO batteries are charged and discharged and measure its capacitance. All statistic data is stored on SD-card. But then it was improved to do the same with PbAcid and other types of batteries.

It uses ballast resistance to limit current of charge and discharge. It can charge and discharge with current up to 5 amps dissipating heat on ballast resistance.

Can charge and discharge **LiFePo4**, **LiIon**, **LiPo**, **PbAcid 3** cells, **PbAcid 6** cells.

## Schematic
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_SmartCharger/schematic.png)

## Routed PCB
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_SmartCharger/pcb.png)

## Software to investigate statistic
With this software [GitHub](https://github.com/SwInDaMix/sw-hub/tree/master/CS/DevTool_LoggingCharger) you can load statistic data from SD-card to investigate them and compare with other batteries statistic. You can watch voltage of charge/discharge and its midpoint voltage (voltage at which battery stays most of the time during the process). Also it shows average current and whole wattage of taken or given energy during the process.

### LiFePo4 statistics
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_SmartCharger/software_charts_lifepo.png)

### PbAcid 6 cells statistics
![](https://raw.github.com/SwInDaMix/swindamix.github.io/master/docs/DevTool_SmartCharger/software_charts_pbacid.png)

## What's next?
Add possibility of CC/CV charge and discharge modes, support of larger currencies up to 15-20 amps. Add support of other types of batteries like Lithium–titanate (LTO), Li-NMC and others.
