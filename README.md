### CDICE2007 - Simulation of DICE2007 in C/C++


Copyright (C) 2012-2013 Martha Butler, Patrick Reed, Thorsten Wagener.
Licensed under the GNU Lesser General Public License.

Citations:
* Two articles in preparation and revision. Links to be posted when available.

* Nordhaus, W.D., 2008. A Question of Balance: Weighing the Options of Global Warming Policies.
 Yale University Press, New Haven, Connecticut.

### Brief Description: 
The CDICE simulation model is based on the DICE2007 economic and climate integrated 
assessment model. Run in GAMS, the DICE intertemporal model optimizes 
time series of emission controls (fraction of production) and investments 
to maximize the discounted sum of decadal utilities of consumption.
The discounted utility is the objective function and the emission controls and
investments are the control variables.

For the purposes of CDICE, a DICE policy scenario is represented by the
emissions control rate and the savings rate (investment as a fraction of
production) as determined in a DICE GAMS run.
A policy scenario is read into CDICE along with a parameter sample file
with alternative values for exogenous parameters to test the vulnerability
of the DICE policy scenario to uncertainties in several of the exogenous 
parameters. 

Note that the GAMS version of DICE2007 may also be run with a fixed savings
rate, which effectively removes investment as a control variable. CDICE can
also be run in the fixed savings mode (see mode options in the sample script
provided).

### Contents:
* `cdice_src/`: the source code for CDICE

* `policy_scenario_files/`: these are DICE GAMS outputs for 4 policy scenarios 
    (optimal, business as usual, 2C atmospheric temperature increase constraint,
	and 2xCO2 atmospheric concentration constraint) and 2 modes (mode 1: emmission
	controls and investment as control variables and mode 2: emission controls only
	as savings rate is fixed to 22 percent).		
    For more information about the scenarios, see the Readme in 
    `policy_scenario_files/`.
  
* `example_sample_files/`: These are files containing as the first row
  the standard DICE GAMS values for exogenous parameters. Subsequent rows
  vary each parameter one at a time to override the standard values (not
  a recommended method for sensitivity testing of nonlinear models).
  Examples are given for testing 7 parameters of the 8 also
  explored by W.D. Nordhaus (see Table 7-1 in A Question of Balance),
  as well as 30 and 31 parameters. This directory also contains
  a file with the standard values of the exogenous parameters and
  a file listing the parameters sampled in each example sample file.
  
* `example_control_files/`: Choose the control file that matches the
  number of parameters being sampled and the policy scenario mode. 
  The control file settings are described in each control file.

* `output_file_descriptions/`: Detailed documentation of contents
  of the output files produced. 

* `run_cdice_example.sh`: a sample script to run CDICE with
  the DICE optimal policy scenario and varying 7 parameters.
  The example creates summary metrics files and full time
  series output for each parameter set in the sample file.
  The first parameter set contains standard values of the
  parameters.


### To compile the model and run the sample script:
* Compile the CDICE model: `cd cdice_src && make`
* Move the output of the make into the main directory: `mv cdice ..`
* Run the example script: `./run_cdice_example.sh`


### What else could be done with this model:
* We have constructed very large ensembles of model output by simultaneously
sampling exogenous parameters to identify the parameters having the most
influence on selected model outputs or metrics.
* Different combinations of exogenous parameters can be sampled by changing
appropriate code in the module CDICEMain.cpp (see notes in the code) and
the parameter_count in the control file.
* The summary model outputs saved for ensemble analysis in the metrics files
can also be changed in the code in CDICEMain.cpp (see notes in the code).
* With some coding changes in CDICEMain.cpp, CDICE could be used in
a multi-objective optimizing framework.
* Alternative formulations of sub-model functions in DICE/CDICE could
be tested. 


The CDICE2007 model is free software: you can distribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

The CDICE2007 model is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public License along
with the CDICE2007 model. If not, see <http://www.gnu.org/licenses/>.
