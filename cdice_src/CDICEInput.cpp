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


//*******************************************************************************************
// allocateDICE defines and initializes Config and Endog and Ref variables
//*******************************************************************************************
void allocateDICE(DICE *dice, int control_mode, int control_startyear)
{
	// allocate/initialize Config 
	//(values here are for documentation purposes only and
	// will be overridden in readExogenousFactors  as well as
	// in parameter/factor testing)
	//    Values here are defaults from DICE-GAMS.

	//char dummychar;  //debug

	//cout << "allocateDICE: beginning to allocate\n"; //debug
	//cin >> dummychar;
	
	dice->config.nDecades = 60;
	dice->config.mode = control_mode;
	dice->config.startYear = double(control_startyear);
	dice->config.dateSeries = new double[dice->config.nDecades];
	
	dice->config.prstp = 0.015;
	dice->config.elasmu = 2.0;
	dice->config.rr  = new double [dice->config.nDecades];
	
	dice->config.pop0 = 6514.;
	dice->config.popasym = 8600.;
	dice->config.gpop0 = 0.35;
	dice->config.gfacpop = new double[dice->config.nDecades];
	dice->config.pop = new double[dice->config.nDecades];
	dice->config.a0 = 0.02722;
	dice->config.ga0 = 0.092;
	dice->config.dela = 0.001;
	dice->config.ga = new double[dice->config.nDecades];
	dice->config.tfp = new double[dice->config.nDecades];
	
	dice->config.k0 = 137.;
	dice->config.dk = 0.100;
	dice->config.gama = 0.300;
	dice->config.saverate0 = 0.22;
	
	dice->config.sig0 = 0.13418;
	dice->config.gsigma = -0.0730;
	dice->config.dsig = 0.003;
	dice->config.dsig2 = 0.0;
	dice->config.gsig = new double[dice->config.nDecades];
	dice->config.sigma = new double[dice->config.nDecades];
	dice->config.eland0 = 11.000;
	dice->config.dtree = 0.10;
	dice->config.etree = new double[dice->config.nDecades];
	dice->config.fosslim = 6000.;
	
	dice->config.mat2000 = 808.9;
	dice->config.mup2000 = 1255.;
	dice->config.mlo2000 = 18365.;
	dice->config.b11 = 0.810712;
	dice->config.b12 = 0.189288;
	dice->config.b21 = 0.097213;
	dice->config.b22 = 0.852787;
	dice->config.b23 = 0.05;
	dice->config.b32 = 0.003119;
	dice->config.b33 = 0.996881;
	
	dice->config.fex0 = -0.06;
	dice->config.fex1 = 0.30;
	dice->config.forcoth = new double[dice->config.nDecades];
	dice->config.tatm0 = 0.7307;
	dice->config.tocn0 = 0.0068;
	dice->config.preanthco2 = 596.4;
	dice->config.t2xco2 = 3.;
	dice->config.fco22x = 3.8;
	dice->config.lambda = dice->config.fco22x/dice->config.t2xco2;
	dice->config.c1 = 0.220;
	dice->config.c3 = 0.300;
	dice->config.c4 = 0.050;
	
	dice->config.a1 = 0.0;
	dice->config.a2 = 0.0028388;
	dice->config.a3 = 2.00;
	
	dice->config.pback0 = 1.17;
	dice->config.theta2 = 2.8;
	dice->config.backrat = 2.;
	dice->config.gback = 0.05;
	dice->config.theta1 = new double[dice->config.nDecades];
	
	dice->config.partfrac1 = 0.25372;
	dice->config.partfrac2 = 1.;
	dice->config.partfracn = 1.;
	dice->config.dpartfrac = 0.;
	dice->config.phi = new double[dice->config.nDecades];

	dice->config.limmu = 1.;
	dice->config.controlrate0 = 0.005;
	
	dice->config.scale1 = 194.;
	dice->config.scale2 = 381800.;

	// Allocate/initialize Endog variables
	dice->endog.fosslim_step = 0;
	dice->endog.temp2_step = 0;
	dice->endog.temp4_step = 0;
	dice->endog.temp6_step = 0;
	dice->endog.temp8_step = 0;
	dice->endog.temp10_step = 0;
	dice->endog.mat2x_step = 0;
	dice->endog.mat25x_step = 0;
	dice->endog.mat3x_step = 0;
	dice->endog.model_fail_step = 0;
	
	dice->endog.kap = new double[dice->config.nDecades];
	dice->endog.gross_production = new double[dice->config.nDecades];
	dice->endog.net_production = new double[dice->config.nDecades];
	dice->endog.production = new double[dice->config.nDecades];
	dice->endog.abatement_factor = new double[dice->config.nDecades];
	dice->endog.damages_factor = new double[dice->config.nDecades];
	dice->endog.abate_cost = new double[dice->config.nDecades];
	dice->endog.damages = new double[dice->config.nDecades];
	dice->endog.total_cost = new double[dice->config.nDecades];
	dice->endog.emiss_ind = new double[dice->config.nDecades];
	dice->endog.emiss_tot = new double[dice->config.nDecades];
	dice->endog.emiss_cum = new double[dice->config.nDecades];
	dice->endog.invest = new double[dice->config.nDecades];
	dice->endog.savings = new double[dice->config.nDecades];
	dice->endog.consum = new double[dice->config.nDecades];
	dice->endog.control_rate = new double[dice->config.nDecades];
	dice->endog.utility = new double[dice->config.nDecades];
	dice->endog.discounted_utility = 0.0;
	dice->endog.ctax = new double[dice->config.nDecades];
	dice->endog.ctax_part = new double[dice->config.nDecades];
	dice->endog.percap_consum = new double[dice->config.nDecades];
	dice->endog.percap_prod = new double[dice->config.nDecades];
	dice->endog.real_int_rate = new double[dice->config.nDecades];
	dice->endog.rir_disc_factor = new double[dice->config.nDecades];
	dice->endog.pv_damages = new double[dice->config.nDecades];
	dice->endog.pv_abate_cost = new double[dice->config.nDecades];
	dice->endog.pv_total_cost = new double[dice->config.nDecades];
	dice->endog.social_cost_carbon = new double[dice->config.nDecades];
	dice->endog.margy = new double[dice->config.nDecades];
	dice->endog.margc = new double[dice->config.nDecades];
	dice->endog.mat = new double[dice->config.nDecades];
	dice->endog.mup = new double[dice->config.nDecades];
	dice->endog.mlo = new double[dice->config.nDecades];
	dice->endog.matav = new double[dice->config.nDecades];
	dice->endog.tatm = new double[dice->config.nDecades];
	dice->endog.tocn = new double[dice->config.nDecades];
	dice->endog.forc = new double[dice->config.nDecades];
	
	
	
	
	//Initialize time series dates
	for (int i=0; i<dice->config.nDecades; i++)
	{
		dice->config.dateSeries[i] = dice->config.startYear + 10.0 * double(i);
	}
	
	
	
	// Initialize time series (some of this is repeated/overridden elsewhere)
	//    config time series will be computed later in Calc_Exog
	//    endog time series are the result of the model and should be reinitialized
	//    before beginning each sample integration...
	for (int i=0; i<dice->config.nDecades; i++)
	{
		dice->config.rr[i] = 0.0;
		dice->config.gfacpop[i] = 0.0;
		dice->config.pop[i] = 0.0;
		dice->config.ga[i] = 0.0;
		dice->config.tfp[i] = 0.0;
		dice->config.gsig[i] = 0.0;
		dice->config.sigma[i] = 0.0;
		dice->config.etree[i] = 0.0;
		dice->config.forcoth[i] = 0.0;
		dice->config.theta1[i] = 0.0;
		dice->config.phi[i] = 0.0;
		
		dice->endog.kap[i] = 0.0;
		dice->endog.gross_production[i] = 0.0;
		dice->endog.net_production[i] = 0.0;
		dice->endog.production[i] = 0.0;
		dice->endog.abatement_factor[i] = 0.0;
		dice->endog.damages_factor[i] = 0.0;
		dice->endog.abate_cost[i] = 0.0;
		dice->endog.damages[i] = 0.0;
		dice->endog.total_cost[i] = 0.0;
		dice->endog.emiss_ind[i] = 0.0;
		dice->endog.emiss_tot[i] = 0.0;
		dice->endog.emiss_cum[i] = 0.0;
		dice->endog.invest[i] = 0.0;
		dice->endog.savings[i] = 0.0;
		dice->endog.consum[i] = 0.0;
		dice->endog.control_rate[i] = 0.0;
		dice->endog.utility[i] = 0.0;
		dice->endog.ctax[i] = 0.0;
		dice->endog.ctax_part[i] = 0.0;
		dice->endog.percap_consum[i] = 0.0;
		dice->endog.percap_prod[i] = 0.0;
		dice->endog.real_int_rate[i] = 0.0;
		dice->endog.rir_disc_factor[i] = 0.0;
		dice->endog.pv_damages[i] = 0.0;
		dice->endog.pv_abate_cost[i] = 0.0;
		dice->endog.pv_total_cost[i] = 0.0;
		dice->endog.social_cost_carbon[i] = 0.0;
		dice->endog.margy[i] = 0.0;
		dice->endog.margc[i] = 0.0;
		dice->endog.mat[i] = 0.0;
		dice->endog.mup[i] = 0.0;
		dice->endog.mlo[i] = 0.0;
		dice->endog.matav[i] = 0.0;
		dice->endog.tatm[i] = 0.0;
		dice->endog.tocn[i] = 0.0;
		dice->endog.forc[i] = 0.0;
		
		
		
	}

	//override a couple of first period values (may also be done elsewhere)
	dice->endog.control_rate[0] = dice->config.controlrate0;
	dice->config.phi[0] = dice->config.partfrac1;


	//cout << "allocateDICE: finished allocating DICE\n";
	//cin >> dummychar;

	return;
}



//***********************************************************************************************
//  readSavingsRate to load an externally defined savings rate time series
//     filename is saveratefileName in files; destination variable is dice->endog.savings
//***********************************************************************************************
void readSavingsRate(DICE *dice)
{
	
	ifstream in;
	in.open(dice->files.saveratefileName.c_str());
	if (!in.is_open()) {cout << "Failed to open savings rate file. Exiting ..."; exit(2);}
	
	for (int i=0; i<dice->config.nDecades; i++)
	{
		in >> dice->endog.savings[i];
		in.ignore(2000,'\n');
	}
	
	in.close();
	
	return;
}


//***********************************************************************************************
//  readControlRate to load an externally defined emission control rate time series
//     filename is emisctlfileName in files; destination variable is dice->endog.control_rate
//***********************************************************************************************
void readControlRate(DICE *dice)
{
	ifstream in;
	in.open(dice->files.emisctlfileName.c_str());
	if (!in.is_open()) {cout << "Failed to open control rate file. Exiting ..."; exit(2);}
	
	for (int i=0; i<dice->config.nDecades; i++)
	{
		in >> dice->endog.control_rate[i];
		in.ignore(2000,'\n');
	}
	
	in.close();
	
	return;
}

//***********************************************************************************************
//   readParticipationRate to load an externally defined participation rate time series
//       filename is participationfileName in files; destination variable is dice->config.phi
//    Note that the code that calls this is currently commented out.
//***********************************************************************************************
void readParticipationRate(DICE *dice)
{
	ifstream in;
	in.open(dice->files.participationfileName.c_str());
	if (!in.is_open()) {cout << "Failed to open participation rate file. Exiting ..."; exit(2);}

	for (int i=0; i<dice->config.nDecades; i++)
	{
		in >>dice->config.phi[i];
		in.ignore(2000,'\n');
	}

	in.close();

	return;
}

//*************************************************************************************************
//    readExogenousFactors to load all non-time-series externally defined factors/parameters
//       required for DICE model execution; includes initial values, submodel parameters, etc.
//       filename is exogenousfileName in files; destination variables are in dice->config.
//    Note that this is currently not executed as its function (loading dice->config. variables)
//       is done in AllocateDICE above.
//*************************************************************************************************
void readExogenousFactors(DICE *dice)
{
	char *junk;
	junk = new char[1000];
	
	ifstream in;
	in.open(dice->files.exogenousfileName.c_str());
	if (!in.is_open()) {cout << "Failed to open exogenous factors file. Exiting ..."; exit(2);}

	//parse each line to load the factors
	// need error/domain checking???

	while(!in.eof())
	{
		in >> junk;
		if (!strcmp(junk, "<nDecades>"))
		{
			in >> dice->config.nDecades;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<prstp>"))
		{
			in >> dice->config.prstp;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<elasmu>"))
		{
			in >> dice->config.elasmu;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<pop0>"))
		{
			in >> dice->config.pop0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<popasym>"))
		{
			in >> dice->config.popasym;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<gpop0>"))
		{
			in >> dice->config.gpop0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<a0>"))
		{
			in >> dice->config.a0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<ga0>"))
		{
			in >> dice->config.ga0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<dela>"))
		{
			in >> dice->config.dela;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<k0>"))
		{
			in >> dice->config.k0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<dk>"))
		{
			in >> dice->config.dk;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<gama>"))
		{
			in >> dice->config.gama;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<sig0>"))
		{
			in >> dice->config.sig0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<gsigma>"))
		{
			in >> dice->config.gsigma;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<dsig>"))
		{
			in >> dice->config.dsig;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<dsig2>"))
		{
			in >> dice->config.dsig2;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<eland0>"))
		{
			in >> dice->config.eland0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<dtree>"))
		{
			in >> dice->config.dtree;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<fosslim>"))
		{
			in >> dice->config.fosslim;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<mat2000>"))
		{
			in >> dice->config.mat2000;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<mup2000>"))
		{
			in >> dice->config.mup2000;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<mlo2000>"))
		{
			in >> dice->config.mlo2000;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b11>"))
		{
			in >> dice->config.b11;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b12>"))
		{
			in >> dice->config.b12;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b21>"))
		{
			in >> dice->config.b21;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b22>"))
		{
			in >> dice->config.b22;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b23>"))
		{
			in >> dice->config.b23;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b32>"))
		{
			in >> dice->config.b32;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<b33>"))
		{
			in >> dice->config.b33;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<fex0>"))
		{
			in >> dice->config.fex0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<fex1>"))
		{
			in >> dice->config.fex1;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<tatm0>"))
		{
			in >> dice->config.tatm0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<tocn0>"))
		{
			in >> dice->config.tocn0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<preanthco2>"))
		{
			in >> dice->config.preanthco2;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<t2xco2>"))
		{
			in >> dice->config.t2xco2;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<fco22x>"))
		{
			in >> dice->config.fco22x;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<c1>"))
		{
			in >> dice->config.c1;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<c3>"))
		{
			in >> dice->config.c3;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<c4>"))
		{
			in >> dice->config.c4;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<a1>"))
		{
			in >> dice->config.a1;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<a2>"))
		{
			in >> dice->config.a2;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<a3>"))
		{
			in >> dice->config.a3;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<pback0>"))
		{
			in >> dice->config.pback0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<theta2>"))
		{
			in >> dice->config.theta2;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<backrat>"))
		{
			in >> dice->config.backrat;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<gback>"))
		{
			in >> dice->config.gback;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<limmu>"))
		{
			in >> dice->config.limmu;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<partfrac1>"))
		{
			in >> dice->config.partfrac1;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<partfrac2>"))
		{
			in >> dice->config.partfrac2;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<partfracn>"))
		{
			in >> dice->config.partfracn;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<dpartfrac>"))
		{
			in >> dice->config.dpartfrac;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<saverate0>"))
		{
			in >> dice->config.saverate0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<controlrate0>"))
		{
			in >> dice->config.controlrate0;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<scale1>"))
		{
			in >> dice->config.scale1;
			in.ignore(2000,'\n');
		}
		else if (!strcmp(junk, "<scale2>"))
		{
			in >> dice->config.scale2;
			in.ignore(2000,'\n');
		}



	}

	in.close();

	delete junk;

	return;
}

//#endif  //CDICEMOD
