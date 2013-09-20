//****************************************************************************
// CDICE
//     C/C++ VERSION OF DICE
//             Economic - Climate Projections
//             Based on GAMS version of DICE2007 v8
//				(and the Excel version)
//			 Rewritten in C/C++ to test sensitivity of model factors
//				independent of the GAMS/solver environment
//				and for the opportunity to optionally
//				utilize multi-objective optimization
//				and/or alternative model components
//				
//            William D. Nordhaus, Yale University
//				A Question of Balance (2008), Yale University Press
//				and http://nordhaus.econ.yale.edu  (obsolete)
//              Updated URL: http://www.econ.yale.edu/~nordhaus/homepage/index.html
//			 for previous versions, see also
//				DICE 1994:
//				William D. Nordhaus,
//				Managing the Global Commons (1994), The MIT Press
//				DICE1999:
//				William D. Norhaus and Joseph Boyer
//				Warming the World (2000), The MIT Press
//            
//           
//          Translated to C/C++ by Martha P. Butler
//            V 1.0  2011-2013 Penn State University
//
//*****************************************************************************/
/* Copyright (C) 2012 Martha Butler, Patrick Reed, Thorsten Wagener, and others

  CDICE is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.
 
  CDICE is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
  License for more details.
 
  You should have received a copy of the GNU Lesser General Public License
  along with the CDICE code.  If not, see <http://www.gnu.org/licenses/>.
 */


//#ifdef CDICEMOD


#include "CDICE.h"

// some major configuration factors here:
// first three may be updated in control file input
int writeOutput = 0;     //for metrics file(s) (0: no output; 1: metrics file output)
int writeModelDump = 0;  //for complete model output (1 file per sample) (debug)
int nSamples = 1;        //Number of samples
int nParams = 1;         //Number of parameters being sampled (specify in control file)
//int nParams = 31;      // above required moving init_CDICE before allocation of xreal[nparams]
//int nParams = 7;       // Important: to change #params, also change population of xreal[nparams] in function calc_CDICE
int nObj = 1;            //Number of objectives (discounted utility)

//for splitting summary output into multiple metrics files with 1 row per parameter sample/model execution
// change these to fit the experiment
int nTagAlongs1 = 15;   //Number of metrics in metric file 1 (this file also includes parameter sample)
int nTagAlongs2 = 32;   //Number of metrics in metric file 2 (climate)
int nTagAlongs3 = 32;   //Number of metrics in metric file 3 (econ1)
int nTagAlongs4 = 32;   //Number of metrics in metric file 4 (econ2)
int nTagAlongs5 = 8;    //Number of metrics in metric file 5 (econ3) added for total costs

char dummychar;  //debug aid
//Individual structure declaration is in CDICE.h 

Individual ind;
Individual* indPtr = &ind;

//Seed the random number generators  
//NRRand uRand(125698); //uniform random number generator
//NRNormal nRand(0.0, 1.0, 652375); //normal random number generator

DICE dice;
DICE *dicePtr = &dice;



// now for the functions:


void init_CDICE()
{

	// deal with the required control file first
	
	// open and read the control file
	// Allocate (and intitialize) Files within dice structure
	char *junk;
	junk = new char[1000];
	

	//cout << "init_CDICE: reading control file\n";   //debug
	//cin >> dummychar;

	ifstream control_stream;
	string controlFileName = "cdice_control.txt";
	control_stream.open(controlFileName.c_str());
	if (!control_stream.is_open()) {cout << "Failed to open control file. Exiting ..."; exit(1);}

	//read a line, parsing for the specific line content, discard rest of line
	int tempint;
	int control_mode;
	int control_startyear;
	while (!control_stream.eof())
	{
		control_stream >> junk;
		if (!strcmp(junk, "<sample_count>")) 
		{	
			control_stream >> tempint;
			control_stream.ignore(2000,'\n');
			if (tempint > 0)
				nSamples = tempint;
			else
			{
				cout << "sample_count must be a positive integer. Exiting ..."; 
				exit(1);
			}
		}
		// added to control file to eliminate need to recompile for number of parameters
		//  being sampled in the experiment
		else if (!strcmp(junk,"<parameter_count>")) 
		{	
			control_stream >> tempint; 
			control_stream.ignore(2000,'\n');
			// 7 parameters are Nordhaus (2008) Table 7-1 minus fosslim
			// 31 parameters used in global sensitivity analysis experiment
			// 30 is like 30 but without savings rate
			// If a different number of parameters sampled, MUST add/change
			// the code to load xreal[nParam] in calcCDICE below.
			if (tempint == 7 || tempint == 30 || tempint == 31)
				nParams = tempint;
			else
				{cout << "nParams must be 7, 30 or 31. Exiting ..."; exit(1);}
		}
		else if (!strcmp(junk,"<factor_sets_file>")) 
		{	
			control_stream >> dicePtr->files.factorfileName; 
			control_stream.ignore(2000,'\n');
		}
		else if (!strcmp(junk,"<mode>")) 
		{	
			control_stream >> tempint; 
			control_stream.ignore(2000,'\n');
			//(mode 1 input: exog factors, control rates, savings rates, 
			//     optionally participation - this last requires code change)
			//(mode 2 input: exog factors, control rates)
			//  Note requirement for exogenous factor file input removed.
			// both modes 1 and 2 are simulation modes requiring DICE model output
			if (tempint == 1 || tempint == 2)
				control_mode = tempint;
			else
				{cout << "mode must be 1 or 2. Exiting ..."; exit(1);}
		}
		else if (!strcmp(junk,"<write_output>")) 
		{	
			control_stream >> tempint; 
			control_stream.ignore(2000,'\n');
			if (tempint == 0 || tempint == 1)
				writeOutput = tempint;
			else
				{cout << "write_output must be 0 or 1. Exiting ..."; exit(1);}
		}
		else if (!strcmp(junk,"<model_dump>")) 
		{
			control_stream >> tempint; 
			control_stream.ignore(2000,'\n');
			if (tempint == 0 || tempint == 1)
				writeModelDump = tempint;
			else
				{cout << "model_dump must be 0 or 1. Exiting ..."; exit(1);}
		}
		else if (!strcmp(junk,"<start_year>"))
		{
			control_stream >> tempint;
			control_stream.ignore(2000,'\n');
			if (tempint > 1985 && tempint < 2305)
				control_startyear = tempint;
			else
				{cout << "start year must be reasonable. Exiting ..."; exit(1);}
		}
		
		
	}
	control_stream.close();

	cout << "control file\n";  //debug
	cout << "nSamples = " << nSamples << endl;
	cout << "nParams = " << nParams << endl;
	cout << "mode = " << control_mode << endl;
	cout << "writeOutput = " << writeOutput << endl;
	cout << "writeModelDump = " << writeModelDump << endl;
	cout << "factors file name = " << dicePtr->files.factorfileName << endl;
	cout << "start year = " << control_startyear << endl;

	// call the function that allocates (and initializes) Config factors
    allocateDICE(dicePtr, control_mode, control_startyear);

	//set standard input file names depending on control_mode
	// read required input files, load Config variables
	if (control_mode == 1 || control_mode == 2)
	{
		// test whether exog factors file really needed: It is not.
		//dicePtr->files.exogenousfileName = "exogenous_factors.txt";
		//readExogenousFactors(dicePtr);
		dicePtr->files.emisctlfileName = "control_rate.txt";
		readControlRate(dicePtr);
	}

	// if using Participation rate input file, uncomment the following
	//    and make changes as noted above.
	if (control_mode == 1)
	{
		//dicePtr->files.participationfileName = "participation_rate.txt";
		//readParticipationRate(dicePtr);
		dicePtr->files.saveratefileName = "savings_rate.txt";
		readSavingsRate(dicePtr);
	}


	delete junk;
	
	//cout << "done with init_DICE\n";
	//cin >> dummychar;

    return;
}

void calc_CDICE(Individual *ind, int samplenum)
{
   
   
   
   //Create pointers to the real objective value and metrics for this sample
    double *xreal  = ind->xreal;
    double *obj    = ind->obj;
   
	double *tag_alongs1 = ind->tag_alongs1;
	double *tag_alongs2 = ind->tag_alongs2;
	double *tag_alongs3 = ind->tag_alongs3;
	double *tag_alongs4 = ind->tag_alongs4;
	double *tag_alongs5 = ind->tag_alongs5;
	
	// Put the factors from this sample into the dice structure
	//  (this is highly situational!!!)
	// Be sure that this matches nParams from control file!!!
	//  7, 30, and 31 parameter experiments currently defined here

	if (nParams == 31)
	{
	  // This is for 31 parameter experiment.
		//   recomment to use mode 2 (savings rate sampled, not input)
	  dicePtr->config.popasym = xreal[0];
	  dicePtr->config.gpop0   = xreal[1];
	  dicePtr->config.ga0     = xreal[2];
	  dicePtr->config.dela    = xreal[3];
	  dicePtr->config.sig0    = xreal[4];
	  dicePtr->config.gsigma  = xreal[5];
	  dicePtr->config.dsig    = xreal[6];
	  dicePtr->config.dsig2   = xreal[7];
	  dicePtr->config.eland0  = xreal[8];
	  dicePtr->config.dtree   = xreal[9];
	  dicePtr->config.b12	  = xreal[10];
	  dicePtr->config.b23     = xreal[11];
	  dicePtr->config.fex0    = xreal[12];
	  dicePtr->config.fex1    = xreal[13];
	  dicePtr->config.t2xco2  = xreal[14];
	  dicePtr->config.fco22x  = xreal[15];
	  dicePtr->config.c1      = xreal[16];
	  dicePtr->config.c3      = xreal[17];
	  dicePtr->config.c4      = xreal[18];
	  dicePtr->config.a1      = xreal[19];
	  dicePtr->config.a2      = xreal[20];
	  dicePtr->config.a3      = xreal[21];
	  dicePtr->config.pback0  = xreal[22];
	  dicePtr->config.theta2  = xreal[23];
	  dicePtr->config.backrat = xreal[24];
	  dicePtr->config.gback   = xreal[25];
	  dicePtr->config.partfrac1 = xreal[26];
	  dicePtr->config.partfrac2 = xreal[27];
	  dicePtr->config.partfracn = xreal[28];
      dicePtr->config.dpartfrac = xreal[29];
	  dicePtr->config.saverate0 = xreal[30];
	}

	if (nParams == 30)
	{
	  // This is for 30 parameter experiment.
	  //   recommend to use mode 1 (with DICE savings rate and emission control rate)
	  dicePtr->config.popasym = xreal[0];
	  dicePtr->config.gpop0   = xreal[1];
	  dicePtr->config.ga0     = xreal[2];
	  dicePtr->config.dela    = xreal[3];
	  dicePtr->config.sig0    = xreal[4];
	  dicePtr->config.gsigma  = xreal[5];
	  dicePtr->config.dsig    = xreal[6];
	  dicePtr->config.dsig2   = xreal[7];
	  dicePtr->config.eland0  = xreal[8];
	  dicePtr->config.dtree   = xreal[9];
	  dicePtr->config.b12	  = xreal[10];
	  dicePtr->config.b23     = xreal[11];
	  dicePtr->config.fex0    = xreal[12];
	  dicePtr->config.fex1    = xreal[13];
	  dicePtr->config.t2xco2  = xreal[14];
	  dicePtr->config.fco22x  = xreal[15];
	  dicePtr->config.c1      = xreal[16];
	  dicePtr->config.c3      = xreal[17];
	  dicePtr->config.c4      = xreal[18];
	  dicePtr->config.a1      = xreal[19];
	  dicePtr->config.a2      = xreal[20];
	  dicePtr->config.a3      = xreal[21];
	  dicePtr->config.pback0  = xreal[22];
	  dicePtr->config.theta2  = xreal[23];
	  dicePtr->config.backrat = xreal[24];
	  dicePtr->config.gback   = xreal[25];
	  dicePtr->config.partfrac1 = xreal[26];
	  dicePtr->config.partfrac2 = xreal[27];
	  dicePtr->config.partfracn = xreal[28];
      dicePtr->config.dpartfrac = xreal[29];
	}
	
    if (nParams == 7)
	{
	  // This is for 7 parameter experiment!
	  dicePtr->config.popasym   = xreal[0];
	  dicePtr->config.ga0       = xreal[1];
	  dicePtr->config.gsigma    = xreal[2];
	  dicePtr->config.b12       = xreal[3];
	  dicePtr->config.t2xco2    = xreal[4];
	  dicePtr->config.a2        = xreal[5];
	  dicePtr->config.pback0    = xreal[6];
	}

	//char dummychar;

	//cout << "calc_CDICE: loaded factors into config locations\n";
	
	//cin >> dummychar;

	
	
	// For this factor/parameter set:
    // Reinitialize all results variables/time series to zero

	dicePtr->endog.discounted_utility = 0.0;
	dicePtr->endog.fosslim_step = 0;
	dicePtr->endog.temp2_step = 0;
	dicePtr->endog.temp4_step = 0;
	dicePtr->endog.temp6_step = 0;
	dicePtr->endog.temp8_step = 0;
	dicePtr->endog.temp10_step = 0;
	dicePtr->endog.mat2x_step = 0;
	dicePtr->endog.mat25x_step = 0;
	dicePtr->endog.mat3x_step = 0;
	dicePtr->endog.model_fail_step = 0;
	
	for (int i=0; i<dicePtr->config.nDecades; i++)
	{
		dicePtr->config.rr[i] = 0.0;
		dicePtr->config.gfacpop[i] = 0.0;
		dicePtr->config.pop[i] = 0.0;
		dicePtr->config.ga[i] = 0.0;
		dicePtr->config.tfp[i] = 0.0;
		dicePtr->config.gsig[i] = 0.0;
		dicePtr->config.sigma[i] = 0.0;
		dicePtr->config.etree[i] = 0.0;
		dicePtr->config.forcoth[i] = 0.0;
		dicePtr->config.theta1[i] = 0.0;

		//uncomment this 'if' when using participation rate input in mode 1
		// (and initialize use calcExog to fill phi for mode 2 only)
		//if (dicePtr->config.mode == 2)
		//{
			dicePtr->config.phi[i] = 0.0;      // may be set from external data for mode 1
											   // otherwise initialize unconditionally here
		//}

		if (dicePtr->config.mode == 2)
		{
			dicePtr->endog.savings[i] = 0.0;   //set from external data only in mode 1
		}
		
		dicePtr->endog.kap[i] = 0.0;
		dicePtr->endog.gross_production[i] = 0.0;
		dicePtr->endog.net_production[i] = 0.0;
		dicePtr->endog.production[i] = 0.0;
		dicePtr->endog.abatement_factor[i] = 0.0;
		dicePtr->endog.damages_factor[i] = 0.0;
		dicePtr->endog.abate_cost[i] = 0.0;
		dicePtr->endog.damages[i] = 0.0;
		dicePtr->endog.emiss_ind[i] = 0.0;
		dicePtr->endog.emiss_tot[i] = 0.0;
		dicePtr->endog.emiss_cum[i] = 0.0;
		dicePtr->endog.invest[i] = 0.0;
		dicePtr->endog.consum[i] = 0.0;
		//dicePtr->endog.control_rate[i] = 0.0;  //'decision variable' - set from external data in modes 1&2!!!
		dicePtr->endog.utility[i] = 0.0;
		dicePtr->endog.ctax[i] = 0.0;
		dicePtr->endog.ctax_part[i] = 0.0;
		dicePtr->endog.percap_consum[i] = 0.0;
		dicePtr->endog.percap_prod[i] = 0.0;
		dicePtr->endog.real_int_rate[i] = 0.0;
		dicePtr->endog.pv_damages[i] = 0.0;
		dicePtr->endog.pv_abate_cost[i] = 0.0;
		dicePtr->endog.pv_total_cost[i] = 0.0;
		dicePtr->endog.social_cost_carbon[i] = 0.0;
		dicePtr->endog.margy[i] = 0.0;
		dicePtr->endog.margc[i] = 0.0;
		dicePtr->endog.mat[i] = 0.0;
		dicePtr->endog.mup[i] = 0.0;
		dicePtr->endog.mlo[i] = 0.0;
		dicePtr->endog.matav[i] = 0.0;
		dicePtr->endog.tatm[i] = 0.0;
		dicePtr->endog.tocn[i] = 0.0;
		dicePtr->endog.forc[i] = 0.0;
	}

    
	//cout << "calc_CDICE: dice initialized for samplenum " << samplenum << endl;
	//cout << "calc_CDICE: calling processModel\n";
	//cin >> dummychar;
	
    //Process this sample
	
    processModel(dicePtr, writeModelDump, samplenum);

   //cout << "calc_CDICE: back from processModel, putting results in obj and tag_along\n";
   //cin >> dummychar;

    //Fill out the individuals objective value(s)
    obj[0] = dicePtr->endog.discounted_utility;
    

    //Fill out the tag along variables  (this is subject to change)
	//Metrics files contain summary information about the outcome for each sample, 1 row per sample per file
	
    tag_alongs1[0]  = dicePtr->endog.emiss_cum[59];
	tag_alongs1[1]  = dicePtr->endog.pv_damages[20];  //200 years for NPV
	tag_alongs1[2]  = dicePtr->endog.pv_abate_cost[20];
	//tag_alongs1[1]  = dicePtr->endog.pv_damages[25];  modified above to 200 years from 250 years
	//tag_alongs1[2]  = dicePtr->endog.pv_abate_cost[25];
	tag_alongs1[3]   = dicePtr->endog.pv_total_cost[20];
	tag_alongs1[4]   = 0.0;  //for future social cost of carbon in 2015
	tag_alongs1[5]  = dicePtr->endog.fosslim_step;
	tag_alongs1[6]  = dicePtr->endog.temp2_step;
	tag_alongs1[7]  = dicePtr->endog.temp4_step;
	tag_alongs1[8]  = dicePtr->endog.temp6_step;
	tag_alongs1[9]  = dicePtr->endog.temp8_step;
	tag_alongs1[10]  = dicePtr->endog.temp10_step;
	tag_alongs1[11]  = dicePtr->endog.mat2x_step;
	tag_alongs1[12] = dicePtr->endog.mat25x_step;
	tag_alongs1[13] = dicePtr->endog.mat3x_step;
	tag_alongs1[14] = dicePtr->endog.model_fail_step;

	//second set has atmospheric temperature, ocean temperature,
	//  atmospheric burden, radiative forcing

	// Feel free to change the output metrics and/or snapshot dates
	//    and/or number of metric files output
	//specific shapshot dates for tag_alongs2/metrics2 file
	tag_alongs2[0]   = dicePtr->endog.tatm[2];   //2025
	tag_alongs2[1]   = dicePtr->endog.tatm[5];   //2055
	tag_alongs2[2]   = dicePtr->endog.tatm[7];   //2075
	tag_alongs2[3]   = dicePtr->endog.tatm[10];  //2105
	tag_alongs2[4]   = dicePtr->endog.tatm[12];  //2125
	tag_alongs2[5]   = dicePtr->endog.tatm[15];  //2155
	tag_alongs2[6]   = dicePtr->endog.tatm[20];  //2205
	tag_alongs2[7]   = dicePtr->endog.tatm[25];  //2255

	tag_alongs2[8]   = dicePtr->endog.tocn[2];
	tag_alongs2[9]   = dicePtr->endog.tocn[5];
	tag_alongs2[10]  = dicePtr->endog.tocn[7];
	tag_alongs2[11]  = dicePtr->endog.tocn[10];
	tag_alongs2[12]  = dicePtr->endog.tocn[12];
	tag_alongs2[13]  = dicePtr->endog.tocn[15];
	tag_alongs2[14]  = dicePtr->endog.tocn[20];
	tag_alongs2[15]  = dicePtr->endog.tocn[25];

	tag_alongs2[16]  = dicePtr->endog.mat[2];
	tag_alongs2[17]  = dicePtr->endog.mat[5];
	tag_alongs2[18]  = dicePtr->endog.mat[7];
	tag_alongs2[19]  = dicePtr->endog.mat[10];
	tag_alongs2[20]  = dicePtr->endog.mat[12];
	tag_alongs2[21]  = dicePtr->endog.mat[15];
	tag_alongs2[22]  = dicePtr->endog.mat[20];
	tag_alongs2[23]  = dicePtr->endog.mat[25];

	tag_alongs2[24]  = dicePtr->endog.forc[2];
	tag_alongs2[25]  = dicePtr->endog.forc[5];
	tag_alongs2[26]  = dicePtr->endog.forc[7];
	tag_alongs2[27]  = dicePtr->endog.forc[10];
	tag_alongs2[28]  = dicePtr->endog.forc[12];
	tag_alongs2[29]  = dicePtr->endog.forc[15];
	tag_alongs2[30]  = dicePtr->endog.forc[20];
	tag_alongs2[31]  = dicePtr->endog.forc[25];


	//third set has production, consumption, damages, abatement costs
    //specific shapshot dates for tag_alongs3/metrics file 3
	tag_alongs3[0]   = dicePtr->endog.production[2];
	tag_alongs3[1]   = dicePtr->endog.production[5];
	tag_alongs3[2]   = dicePtr->endog.production[7];
	tag_alongs3[3]   = dicePtr->endog.production[10];
	tag_alongs3[4]   = dicePtr->endog.production[12];
	tag_alongs3[5]   = dicePtr->endog.production[15];
	tag_alongs3[6]   = dicePtr->endog.production[20];
	tag_alongs3[7]   = dicePtr->endog.production[25];

	tag_alongs3[8]   = dicePtr->endog.consum[2];
	tag_alongs3[9]   = dicePtr->endog.consum[5];
	tag_alongs3[10]  = dicePtr->endog.consum[7];
	tag_alongs3[11]  = dicePtr->endog.consum[10];
	tag_alongs3[12]  = dicePtr->endog.consum[12];
	tag_alongs3[13]  = dicePtr->endog.consum[15];
	tag_alongs3[14]  = dicePtr->endog.consum[20];
	tag_alongs3[15]  = dicePtr->endog.consum[25];

	tag_alongs3[16]  = dicePtr->endog.damages[2];
	tag_alongs3[17]  = dicePtr->endog.damages[5];
	tag_alongs3[18]  = dicePtr->endog.damages[7];
	tag_alongs3[19]  = dicePtr->endog.damages[10];
	tag_alongs3[20]  = dicePtr->endog.damages[12];
	tag_alongs3[21]  = dicePtr->endog.damages[15];
	tag_alongs3[22]  = dicePtr->endog.damages[20];
	tag_alongs3[23]  = dicePtr->endog.damages[25];

	tag_alongs3[24]  = dicePtr->endog.abate_cost[2];
	tag_alongs3[25]  = dicePtr->endog.abate_cost[5];
	tag_alongs3[26]  = dicePtr->endog.abate_cost[7];
	tag_alongs3[27]  = dicePtr->endog.abate_cost[10];
	tag_alongs3[28]  = dicePtr->endog.abate_cost[12];
	tag_alongs3[29]  = dicePtr->endog.abate_cost[15];
	tag_alongs3[30]  = dicePtr->endog.abate_cost[20];
	tag_alongs3[31]  = dicePtr->endog.abate_cost[25];

	
	//fourth set has decadal emissions, cumulative emissions,
	// carbon tax with complete participation
	// carbon tax with incomplete participation

    //specific shapshot dates for tag_alongs4/metrics4 file
	tag_alongs4[0]   = dicePtr->endog.emiss_tot[2];
	tag_alongs4[1]   = dicePtr->endog.emiss_tot[5];
	tag_alongs4[2]   = dicePtr->endog.emiss_tot[7];
	tag_alongs4[3]   = dicePtr->endog.emiss_tot[10];
	tag_alongs4[4]   = dicePtr->endog.emiss_tot[12];
	tag_alongs4[5]   = dicePtr->endog.emiss_tot[15];
	tag_alongs4[6]   = dicePtr->endog.emiss_tot[20];
	tag_alongs4[7]   = dicePtr->endog.emiss_tot[25];

	tag_alongs4[8]   = dicePtr->endog.emiss_cum[2];
	tag_alongs4[9]   = dicePtr->endog.emiss_cum[5];
	tag_alongs4[10]  = dicePtr->endog.emiss_cum[7];
	tag_alongs4[11]  = dicePtr->endog.emiss_cum[10];
	tag_alongs4[12]  = dicePtr->endog.emiss_cum[12];
	tag_alongs4[13]  = dicePtr->endog.emiss_cum[15];
	tag_alongs4[14]  = dicePtr->endog.emiss_cum[20];
	tag_alongs4[15]  = dicePtr->endog.emiss_cum[25];
	
	tag_alongs4[16]  = dicePtr->endog.ctax[2];
	tag_alongs4[17]  = dicePtr->endog.ctax[5];
	tag_alongs4[18]  = dicePtr->endog.ctax[7];
	tag_alongs4[19]  = dicePtr->endog.ctax[10];
	tag_alongs4[20]  = dicePtr->endog.ctax[12];
	tag_alongs4[21]  = dicePtr->endog.ctax[15];
	tag_alongs4[22]  = dicePtr->endog.ctax[20];
	tag_alongs4[23]  = dicePtr->endog.ctax[25];

	tag_alongs4[24]  = dicePtr->endog.ctax_part[2];
	tag_alongs4[25]  = dicePtr->endog.ctax_part[5];
	tag_alongs4[26]  = dicePtr->endog.ctax_part[7];
	tag_alongs4[27]  = dicePtr->endog.ctax_part[10];
	tag_alongs4[28]  = dicePtr->endog.ctax_part[12];
	tag_alongs4[29]  = dicePtr->endog.ctax_part[15];
	tag_alongs4[30]  = dicePtr->endog.ctax_part[20];
	tag_alongs4[31]  = dicePtr->endog.ctax_part[25];


    //specific shapshot dates for tag_alongs5/metrics5 file
	tag_alongs5[0]   = dicePtr->endog.total_cost[2];
	tag_alongs5[1]   = dicePtr->endog.total_cost[5];
	tag_alongs5[2]   = dicePtr->endog.total_cost[7];
	tag_alongs5[3]   = dicePtr->endog.total_cost[10];
	tag_alongs5[4]   = dicePtr->endog.total_cost[12];
	tag_alongs5[5]   = dicePtr->endog.total_cost[15];
	tag_alongs5[6]   = dicePtr->endog.total_cost[20];
	tag_alongs5[7]   = dicePtr->endog.total_cost[25];
	
	//in some modes (other than simulation modes 1 & 2)
	//will need to include decision variables in metrics
	//emission control rate and investment time series
    

    return;
}

void finalize_CDICE()
{

   // report anything?
	cout << "Done!";
	
    return;
}



//  main function for running CDICE by itself
// arguments  are in required control file

int main()     
{
	// assume a fixed name control file with all processing options 
	// containing mode, sample count and factor set filename (read in init function)
	// init_CDICE moved before allocation of "Individual" so that nParams
	// can be specified in the control file read in init_CDICE
	init_CDICE();

    //Allocate the "Individual" which holds the objective, parameters, and tag alongs;
	// Tag-alongs are for model results to be output in metric(s) files
    indPtr->obj       = new double [nObj];
    indPtr->xreal     = new double [nParams];
	indPtr->tag_alongs1 = new double [nTagAlongs1];
	indPtr->tag_alongs2 = new double [nTagAlongs2];
	indPtr->tag_alongs3 = new double [nTagAlongs3];
	indPtr->tag_alongs4 = new double [nTagAlongs4];
	indPtr->tag_alongs5 = new double [nTagAlongs5];


    //Initialize CDICE (arguments are in the control file)

	//init_CDICE();  //moved above
		
	//open the factor sets file (these are the parameter sets being tested in simulation mode)
	ifstream factors;
	factors.open(dicePtr->files.factorfileName.c_str());
	if (!factors.is_open()){cout << "Failed to open factor sets file. Exiting ..."; exit(3);}
	

	//open the metrics output file(s)
	//Adjust for number of files used
	//IF code commented out to satisfy Visual Studio
	//if (writeOutput == 1)  -unconditionally open & close; conditionally write
	//{
		ofstream metrics1;
		string metricsfileName1 = "cdice_metrics1.txt";
		metrics1.open(metricsfileName1.c_str());
		metrics1.setf(ios_base::fixed);
		metrics1.precision(6);
		if (!metrics1.is_open()) {cout << "Failed to open metrics file1. Exiting ..."; exit(4);}

		ofstream metrics2;
		string metricsfileName2 = "cdice_metrics2.txt";
		metrics2.open(metricsfileName2.c_str());
		metrics2.setf(ios_base::fixed);
		metrics2.precision(6);
		if (!metrics2.is_open()) {cout << "Failed to open metrics file2. Exiting ..."; exit(4);}

		ofstream metrics3;
		string metricsfileName3 = "cdice_metrics3.txt";
		metrics3.open(metricsfileName3.c_str());
		metrics3.setf(ios_base::fixed);
		metrics3.precision(6);
		if (!metrics3.is_open()) {cout << "Failed to open metrics file3. Exiting ..."; exit(4);}

		ofstream metrics4;
		string metricsfileName4 = "cdice_metrics4.txt";
		metrics4.open(metricsfileName4.c_str());
		metrics4.setf(ios_base::fixed);
		metrics4.precision(6);
		if (!metrics4.is_open()) {cout << "Failed to open metrics file4. Exiting ..."; exit(4);}

		ofstream metrics5;
		string metricsfileName5 = "cdice_metrics5.txt";
		metrics5.open(metricsfileName5.c_str());
		metrics5.setf(ios_base::fixed);
		metrics5.precision(6);
		if (!metrics5.is_open()) {cout << "Failed to open metrics file5. Exiting ..."; exit(4);}
	//}
	
	//cout << "main: starting sample loop\n";
	//cin >> dummychar;

	//get the factors/parameters from the factor sets file
	double *tempval = new double[nParams];	

    for (int s=0; s<nSamples; s++)
    {
	    // get the next factor set (sample)
		for (int p=0; p<nParams; p++)
		{
			factors >> tempval[p];
			if (factors.fail()) {cout <<"bad read on factor sets at row " << s; exit(4);}
		}
		// skip rest of line
		factors.ignore(2000,'\n');
		
		//cout << "main: sample number " << s << endl;
		
		// Load factors into Ind structure and initialize rest of Ind for this sample
		for (int i=0; i<nObj; i++) indPtr->obj[i] = 0.0;
		for (int i=0; i<nParams; i++) indPtr->xreal[i] = tempval[i];
		for (int i=0; i<nTagAlongs1; i ++) indPtr->tag_alongs1[i] = 0.0;
		for (int i=0; i<nTagAlongs1; i ++) indPtr->tag_alongs2[i] = 0.0;
		for (int i=0; i<nTagAlongs3; i ++) indPtr->tag_alongs3[i] = 0.0;
		for (int i=0; i<nTagAlongs4; i ++) indPtr->tag_alongs4[i] = 0.0;
		for (int i=0; i<nTagAlongs5; i ++) indPtr->tag_alongs5[i] = 0.0;

        //Process this sample
        calc_CDICE(indPtr, s);
		
		// Adjust following based on number/content of summary metric files
		if (writeOutput == 1)
		{
			//Write the result for this individual to the metrics files

			for (int i=0; i<nObj; i++)        metrics1 << setw(17) << indPtr->obj[i];
			for (int i=0; i<nTagAlongs1; i++) metrics1 << setw(17) << indPtr->tag_alongs1[i];
			for (int i=0; i<nParams; i++)     metrics1 << setw(17) << indPtr->xreal[i];
			metrics1 << endl;

			//Add the model_fail_step as first variable in each row for subsequent identification:
			metrics2 << setw(17) << indPtr->tag_alongs1[14];
			for (int i=0; i<nTagAlongs2; i++) metrics2 << setw(17) << indPtr->tag_alongs2[i];
			metrics2 << endl;

			metrics3 << setw(17) << indPtr->tag_alongs1[14];
			for (int i=0; i<nTagAlongs3; i++) metrics3 << setw(17) << indPtr->tag_alongs3[i];
			metrics3 << endl;

			metrics4 << setw(17) << indPtr->tag_alongs1[14];
			for (int i=0; i<nTagAlongs4; i++) metrics4 << setw(17) << indPtr->tag_alongs4[i];
			metrics4 << endl;

			metrics5 << setw(17) << indPtr->tag_alongs1[14];
			for (int i=0; i<nTagAlongs5; i++) metrics5 << setw(17) << indPtr->tag_alongs5[i];
			metrics5 << endl;

		}
		
    }

    delete tempval;

    finalize_CDICE();

    //Close the factors file and all output files
	//Adjust for file usage.
    factors.close();
	metrics1.close();
	metrics2.close();
	metrics3.close();
	metrics4.close();
	metrics5.close();
	
	

    return 0;
}





//#endif //CDICEMOD


