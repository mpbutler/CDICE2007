ReadMe for the CDICE2007 output file documentation.

Contents of this directory:

Modelout.txt contains the file format for the 'model dump' output from CDICE2007.
This is a square file with 47 rows and 60 columns.
The first row holds summary data for the model run with this parameter sample,
as well as the sample values.
Remaining rows are time series from the model.
The format was designed to match an extended 'put' file we created for the DICE GAMS code.

There are 5 metrics files. It is possible, by changing code in CDICEMain.cpp, to 
change the number and contents of the metrics files. The current
configuration supported ensemble runs of millions of parameter samples, hence
the separation of the metric files into manageable chunks.

The metrics files have 1 row for each parameter sample.

cdice_metrics_1.txt is a summary of key metrics for this parameter sample,
diagnostic information about the model execution, and the parameter sample values.

cdice_metrics_2.txt contains snapshots of climate data at specific decadal checkpoints.
The climate data include atmospheric temperature, ocean temperature, the carbon in 
the atmosphere, and radiative forcing.

cdice_metrics_3.txt contains snapshots of economic data at the same decadal checkpoints.
The economic data include global world production, consumption, climate damages,
and abatement costs.

cdice_metrics_4.txt contain snapshots of additional data at the same decadal checkpoints.
The data here include decadal emissions, cumulative emissions through the previous
decade, carbon tax, and carbon tax for less than full global participation
(if applicable).

cdice_metrics_5.txt contains total policy scenario costs (climate damages plus
abatement costs) for the same decadal checkpoints.

Detailed file contents are documented in the files in this folder.