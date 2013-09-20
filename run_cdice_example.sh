#!/bin/sh
#
# example script for submitting cdice
#    need executable and the following files in the run directory:
#       cdice_control.txt (specifies number of parameters, samples, sample file, mode, etc.)
#       a parameter sample file (named in cdice_control.txt)
#       control_rate.txt  (emission control rates from the DICE policy scenario)
#       savings_rate.txt  (savings rate from the same DICE policy scenario - mode 1 only)
#
#  move the executable from the make to this directory
#
# set up the experiment to run the DICE optimal policy in mode 1 for a 7-parameter experiment
#     the first row in the sample file has the DICE standard values of the parameters
#     the remaining rows sample each parameter one at a time at different settings
#
echo "run started at `date`"
# get the sample file for this experiment
cd example_sample_files/
cp factors7_2sigma_examples.txt ../
cd ..
# get the DICE policy scenario for this experiment
cd policy_scenario_files/mode1/
cp opt_nfs_mu.txt ../../control_rate.txt
cp opt_nfs_saverate.txt ../../savings_rate.txt
cd ..
cd ..
#  or for the corresponding mode2 run (fixed savings rate)
#cd policy_scenario_files/mode2/ 
#cp policy_scenario_files/mode2/opt_fs_mu.txt ../../control_rate.txt
#cd ..
#cd ..
#
# get the control file for this experiment (choose mode to match policy)
cd example_control_files/
cp cdice_control_p7_m1.txt ../cdice_control.txt
#cp cdice_control_p7_m2.txt ../cdice_control.txt
cd ..
#
# all set up, so run the experiment
./cdice 
#
# this next code block removes extra white space between fields in metrics output files
#for xx in `seq 5`; do
#    sed -e "{s/^ *//;s/ \+/ /g}" cdice_metrics$xx.txt > metrics$xx.txt
#    rm cdice_metrics$xx.txt
#done
#
echo "run ended at `date`"
