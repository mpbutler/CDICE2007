README for policy scenario files    last updated 08/27/2013

The subfolders here contain the control files needed to run CDICE
for four policy scenarios in two different modes (with the DICE
endogenous savings rates or with a fixed savings rate).

DICE creates policy scenarios based on optimization of Discounted
Utility of Consumption subject to constraints. The control variables
are emission controls (fraction of industrial production from non-
carbon energy sources) and investment.

For each DICE policy scenario created in GAMS, we extract control
variables in the form of the emission control time series and a
savings rate time series. The savings rate is the fraction of 
production which is the investment.

We use these two time series (or, optionally, just the emission
control rate with a fixed savings rate) as input to the CDICE
model, defining the policy scenario.  Global world production
and emission pathways are endogenous to the DICE (and CDICE)
model, and are not input.

The subfolders here contain the 'controls' required to run
CDICE for four policy scenarios: 
1) the DICE GAMS optimal scenario  [opt].
2) a baseline or Business As Usual policy scenario [bau]
   where there is only minimal mitigation in the first
   century or so with a very steep ramp up to full
   control in 250 years.
3) a doubled-CO2 atmospheric carbon constrained scenario [mat20]
4) a 2 degrees Celsius atmospheric temperature increase constrained
   scenario [tatm20]
   
Note that the doubled-CO2 and 2 degrees C constrained runs do
not permit overshooting the targets and generally reach
the constraint limits around 2100.   
   
In the mode1 folder, please find the emission controls and
matching savings rates output from DICE GAMS run without fixing
the savings rate (an option in the GAMS model).

The mode2 folder contains the emission controls from the
corresponding DICE GAMS executions with the savings rate
fixed to 22% (as specified in the GAMS code), or overridden
with a sample savings rate in the 31-parameter experiment.
For this reason, please use mode 2 if running the 31
parameter experiment.


   

