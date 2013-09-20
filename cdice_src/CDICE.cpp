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

extern DICE dice;
extern DICE* dicePtr;


//*****************************************************************************
//  calcExog(dice)  calculate exogenous time series
//     in the Config section of the dice structure
//*****************************************************************************
void calcExog(DICE *dice)
{
   Config *config = &dice->config;
   Endog  *endog  = &dice->endog;   //need this to conditionally propagate savings rate
   int tlim = config->nDecades;

   //cout << "calcExog: beginning " << endl;
 
   
  //Labor/Population
  config->pop[0] = config->pop0;
  config->gfacpop[0] = 0.0;
  for (int t=1; t<tlim; t++)
  {
	config->gfacpop[t] = (exp(config->gpop0*double(t))-1.0) / exp(config->gpop0*double(t));
	config->pop[t] = config->pop0*(1.0-config->gfacpop[t]) + (config->gfacpop[t])*config->popasym;
  }
  
  //Total Factor Productivity
  config->tfp[0] = config->a0;
  config->ga[0] = config->ga0;
  for (int t=1; t<tlim; t++)
  {
	config->ga[t] = config->ga0 * exp(-config->dela*10.0*double(t));
	config->tfp[t] = config->tfp[t-1] / (1.0 - config->ga[t-1]);
  }
  
  //Average Utility Social Discount Rate
  config->rr[0] = 1.0;
  for (int t=1; t<tlim; t++)
  {
	config->rr[t] = 1.0/pow(1.0+config->prstp,10.0*double(t));
  }
  
   //Carbon Content of Emissions (carbon intensity)
   config->gsig[0] = config->gsigma;
   config->sigma[0] = config->sig0;
   for (int t=1; t<tlim; t++)
   {
	config->gsig[t] = config->gsigma * exp(-10.0*config->dsig*double(t) - 10.0*config->dsig2*pow(double(t),2.0));
	config->sigma[t] = config->sigma[t-1] / (1.0 - config->gsig[t]);
   }
   
   //Abatement Costs
   config->theta1[0] = config->pback0 * config->sigma[0] / config->theta2;
   for (int t=1; t<tlim; t++)
   {
	config->theta1[t] = (config->pback0 * config->sigma[t] / config->theta2) *
						((config->backrat - 1.0 + exp(-config->gback*double(t)))/config->backrat);
   }
   

   //Carbon Emissions from Land Use Change/Deforestation
   for (int t=0; t<tlim; t++)
   {
	config->etree[t] = config->eland0 * pow((1.0-config->dtree),double(t));
   }
   
   //Radiative forcing from other greenhouse gases
   for (int t=0; t<10; t++)
   {
	config->forcoth[t] = config->fex0 + 0.1 * (config->fex1 - config->fex0)*double(t);
   }
   for (int t=10; t<tlim; t++)
   {
	config->forcoth[t] = config->fex1;
   }
   
   //Participation Rate in Emissions Control
   // the phi time series is an external input for mode 1, but calcuated here in mode 2
   // Phi is not currently input in mode 1. If it is activated, uncomment the following
   // conditional 'if' code.
   //if (config->mode == 2)
   //{
		config->phi[0] = config->partfrac1;
		for (int t=1; t<25; t++)
		{
			config->phi[t] = config->partfracn + (config->partfrac2-config->partfracn) *
   										exp(-config->dpartfrac*(double(t)-1.0));
		}
		for (int t=25; t<tlim; t++)
		{
			config->phi[t] = config->partfracn;
		}
		//config->phi[0] = 0.25372;  //per WDN assumptions about Kyoto compliance
   //}

   //Reset lambda
   config->lambda = config->fco22x / config->t2xco2;
   
   //Reset the box model coefficients
   // b12 and b23 are independent
   config->b11 = 1.0 - config->b12;
   config->b21 = 587.473 * config->b12 / 1143.894;
   config->b22 = 1.0 - config->b21 - config->b23;
   config->b32 = 1143.894 * config->b23 / 18340.0;
   config->b33 = 1.0 - config->b32;

   //Propagate savings rate in mode 2 only
   // This fixes the savings rate to a constant rate .
   //   (savings rate is the fraction of GWP allocated to investment)
   if (config->mode == 2)
   {
		for (int t=0; t<tlim; t++)
		{
			endog->savings[t] = config->saverate0;
		}
   }
   
   return;
}

//*****************************************************************************
//	calcClim1 is the first time step for climate related variables
//*****************************************************************************
void calcClim1(DICE *dice)
{

	Config *config = &dice->config;
	Endog  *endog  = &dice->endog;
	
	//Establish the carbon pools
	endog->mat[0] = config->mat2000;
	endog->mup[0] = config->mup2000;
	endog->mlo[0] = config->mlo2000;
	endog->matav[0] = config->mat2000;  //not used here
	
	//Calculate radiative forcing
	endog->forc[0] = config->fco22x * 
						(log((endog->mat[0] + 1.0e-6) / config->preanthco2) / log(2.0))
						+ config->forcoth[0];

	//Establish initial temperatures in atmosphere and ocean
	endog->tatm[0] = config->tatm0;
	endog->tocn[0] = config->tocn0;
	
	return;
}

//*****************************************************************************
//	calcEcon1 is the first time step for the economic variables
//*****************************************************************************
void calcEcon1(DICE *dice)
{

	Config *config = &dice->config;
	Endog  *endog  = &dice->endog;

	//char dummychar;
	//cout << "calcEcon1: beginning first time step\n";
	
	//Calculate abatement cost factor
	endog->abatement_factor[0] = pow(config->phi[0],(1.0-config->theta2)) *
									config->theta1[0] * 
									pow(endog->control_rate[0],config->theta2);
	
	
	//cout << endog->abatement_factor[0] << endl;
	//cin >> dummychar;

	//Calculate climate damages factor
	endog->damages_factor[0] = config->a1 * endog->tatm[0] +
								config->a2*pow(endog->tatm[0],config->a3);

	//Initialize capital stock
	endog->kap[0] = config->k0;
	
	//Calculate output (production) - 3 flavors
	// gross production is gross production
	// net production is net of climate damages
	// production is net of both climate damages and abatement costs

	endog->gross_production[0] = config->tfp[0] *
									pow(config->pop[0],(1.0-config->gama)) *
									pow(endog->kap[0],config->gama);
	//cout << endog->gross_production[0] << endl;
	//cin >> dummychar;

	endog->production[0] = endog->gross_production[0] *
						(1.0-endog->abatement_factor[0])/(1.0+endog->damages_factor[0]);
	//cout << endog->production[0] << endl;
	//cin >> dummychar;

	endog->net_production[0] = endog->gross_production[0] /
								(1.0 + endog->damages_factor[0]);
	//cout << endog->net_production[0]<<endl;
	//cin >> dummychar;

	//to explicitly determine damages and abatement costs
	//(these could be moved to postProcess - derived quantities)
	endog->damages[0] = endog->gross_production[0] -
						endog->gross_production[0]/(1.0+endog->damages_factor[0]);
	endog->abate_cost[0] = endog->gross_production[0] * endog->abatement_factor[0];
	endog->total_cost[0] = endog->damages[0] + endog->abate_cost[0];
	
	//Calculate industrial emissions and total emissions
	endog->emiss_ind[0] = 10.0 * config->sigma[0] *
							(1.0 - endog->control_rate[0]) *
							endog->gross_production[0];
	
	endog->emiss_tot[0] = endog->emiss_ind[0] + config->etree[0];
	endog->emiss_cum[0] = 0.0;
	
	//Establish investment, savings rate, consumption
	// in simulation mode (modes 1 and 2)
	if (config->mode == 1 || config->mode == 2)
	{
		endog->invest[0] = endog->savings[0] * endog->production[0];
	}
	else
	{
		endog->savings[0] = endog->invest[0] / (0.001 + endog->production[0]);
	}

	endog->consum[0] = endog->production[0] - endog->invest[0];
	
	return;
}


//*****************************************************************************
//  calcClim is the decadal time step for the climate model component
//*****************************************************************************
void calcClim(DICE *dice, int t)
{

	Config *config = &dice->config;
	Endog  *endog  = &dice->endog;
	
	//update the carbon pools (carbon cycle box model)
	endog->mat[t] = endog->emiss_tot[t-1] +
						config->b11 * endog->mat[t-1] +
						config->b21 * endog->mup[t-1];

	endog->mup[t] = config->b12 * endog->mat[t-1] +
						config->b22 * endog->mup[t-1] +
						config->b32 * endog->mlo[t-1];
						
	endog->mlo[t] = config->b33 * endog->mlo[t-1] +
						config->b23 * endog->mup[t-1];
						
	endog->matav[t] = (endog->mat[t] + endog->mat[t-1]) / 2.0;	//[not used]

	//calculate forcing; changed from using matav to mat
	endog->forc[t] = config->fco22x * 
						(log((endog->mat[t] +1.0e-6) / config->preanthco2) / log(2.0))
						+ config->forcoth[t];
						
	//update atmospheric and ocean temperatures
	endog->tatm[t] = endog->tatm[t-1] + config->c1 *
						(endog->forc[t] - config->lambda * endog->tatm[t-1] -
						config->c3 * (endog->tatm[t-1] - endog->tocn[t-1]));
	
	endog->tocn[t] = endog->tocn[t-1] +
						config->c4 * (endog->tatm[t-1] - endog->tocn[t-1]);

	// set time step switches if various temp and mat limits are exceeded
	// pad the threshold limits a bit, so as not to count 'close calls'
	// testing 0.1%
	if (endog->tatm[t] > 2.002 && endog->temp2_step < 1)
	{
		endog->temp2_step = t+1;
	}
	if (endog->tatm[t] > 4.004 && endog->temp4_step < 1)
	{
		endog->temp4_step = t+1;
	}
	if (endog->tatm[t] > 6.006 && endog->temp6_step < 1)
	{
		endog->temp6_step = t+1;
	}
	if (endog->tatm[t] > 8.008 && endog->temp8_step < 1)
	{
		endog->temp8_step = t+1;
	}
	if (endog->tatm[t] > 10.010 && endog->temp10_step < 1)
	{
		endog->temp10_step = t+1;
	}
	// atmospheric burden of 1193 Gt C corresponds to 560 ppm CO2
	// use 1491 Gt C for 2.5X and 1790 for 3x - plus 0.1%
	if (endog->mat[t] > 1194.2 && endog->mat2x_step < 1)
	{
		endog->mat2x_step = t+1;
	}
	if (endog->mat[t] > 1492.5 && endog->mat25x_step < 1)
	{
		endog->mat25x_step = t+1;
	}
	if (endog->mat[t] > 1798.0 && endog->mat3x_step < 1)
	{
		endog->mat3x_step = t+1;
	}
	
	return;
}

//*****************************************************************************
//	calcEcon is the decadal time step for the economics model component
//*****************************************************************************
void calcEcon(DICE *dice, int t)
{

	Config *config = &dice->config;
	Endog  *endog  = &dice->endog;
	
	//Calculate abatement cost factor
	endog->abatement_factor[t] = pow(config->phi[t],(1.0-config->theta2)) *
									config->theta1[t] * 
									pow(endog->control_rate[t],config->theta2);

	// Add a catch on bad abatement_factor here - if this factor is >= 1.0
	//   then production is negative and subsequent model steps eventually fail
	// This situation is a complex interaction between control rate and phi
	//   especially where phi[t] is smallish (<0.25) and control_rate[t] approaches 1.0
	if (endog->abatement_factor[t] > 0.95)
	{
		endog->abatement_factor[t] = 0.95;

		if (endog->model_fail_step < 1)
		{
			endog->model_fail_step = t+1;
		}
	}

	//Calculate climate damages factor
	endog->damages_factor[t] = config->a1 * endog->tatm[t] +
								config->a2 * pow(endog->tatm[t],config->a3);
								
	//Update capital stock and mark model execution if low capital condition
	endog->kap[t] = pow(1.0 - config->dk,10.0) * endog->kap[t-1] +
						10.0 * endog->invest[t-1];
	if (endog->kap[t] < 10.0)
	{
		endog->kap[t] = 10.0;
		if (endog->model_fail_step < 1)
		{
			endog->model_fail_step = t+1;
		}
	}


	//Calculate output (production) - 3 flavors
	// gross production is gross production
	// net production is net of climate damages
	// production is net of both climate damages and abatement costs
	endog->gross_production[t] = config->tfp[t] *
									pow(config->pop[t],(1.0-config->gama)) *
									pow(endog->kap[t],config->gama);
	
	endog->production[t] = endog->gross_production[t] *
						(1.0-endog->abatement_factor[t])/(1.0+endog->damages_factor[t]);
	
	endog->net_production[t] = endog->gross_production[t] /
								(1.0 + endog->damages_factor[t]);

	//to explicitly determine damages and abatement costs
	//(these could be moved to postProcess - derived quantities)
	endog->damages[t] = endog->gross_production[t] -
						endog->gross_production[t]/(1.0+endog->damages_factor[t]);
	endog->abate_cost[t] = endog->gross_production[t] * endog->abatement_factor[t];
	endog->total_cost[t] = endog->damages[t] + endog->abate_cost[t];
	
	//Calculate industrial emissions and total emissions
	//note that once the control rate goes to 1.0 (100%), 
	// the only emissions are from land use change
	endog->emiss_ind[t] = 10.0 * config->sigma[t] *
							(1.0 - endog->control_rate[t]) *
							endog->gross_production[t];
	
	endog->emiss_tot[t] = endog->emiss_ind[t] + config->etree[t];
	//note that (as per GAMS), emiss_cum[t] is cumulative through step [t-1]
	//and includes land use as well as industrial emissions
	endog->emiss_cum[t] = endog->emiss_cum[t-1] + endog->emiss_tot[t-1];


	// set a switch if fosslim is exceeded
	// and at what time step limit is first exceeded - allow some threshold slip here
	if (endog->emiss_cum[t] > (config->fosslim * 1.001) && endog->fosslim_step < 1)
	{
		endog->fosslim_step = t+1;
	}
	
	//Establish investment, savings rate, consumption
	// Mark model execution if low consumption condition
	if (config->mode == 1 || config->mode == 2)
	{
		endog->invest[t] = endog->savings[t] * endog->production[t];
	}
	else
	{
		endog->savings[t] = endog->invest[t] / (0.001 + endog->production[t]);
	}

	endog->consum[t] = endog->production[t] - endog->invest[t];
	if (endog->consum[t] < 10.0)
	{
		endog->consum[t] = 10.0;
		if (endog->model_fail_step < 1)
		{
			endog->model_fail_step = t+1;
		}
	}
	
	

	return;
}


//*****************************************************************************
//	postProcess derived calculations that can be done after time stepping
//  	calculation of metrics from derived data
//      output of detailed model data if requested
//*****************************************************************************
void postProcess(DICE *dice, int writeModelDump, int samplenum)
{

	Files *files = &dice->files;
	Config *config = &dice->config;
	Endog  *endog  = &dice->endog;

	
	//Calculate additional model outputs
	for (int t=0; t<config->nDecades; t++)
	{
		//Carbon Tax (note that for modes 1 and 2, this is effectively exogenous
		//             as the control rate is provided)
		endog->ctax[t] = config->theta2 * config->theta1[t] *
							pow(endog->control_rate[t],(config->theta2-1.0)) /
							config->sigma[t] * 1000.0;
	    //Carbon Tax allowing for incomplete Participation
		// (will be same as above if phi is 1)
		endog->ctax_part[t] = endog->ctax[t] * pow(config->phi[t],(1.0-config->theta2));

		//Per Capita Consumption and Production
		endog->percap_consum[t] = endog->consum[t] * 1000.0 / config->pop[t];
		
		endog->percap_prod[t] = endog->production[t] * 1000.0 / config->pop[t];
		
		//Real Interest Rate
		endog->real_int_rate[t] = config->gama * 
									endog->production[t] / endog->kap[t] -
									(1.0 - pow(1.0-config->dk,10.0))/10.0;
	
		//Utility for each period
		endog->utility[t] = (pow(endog->consum[t]/config->pop[t],(1.0 - config->elasmu)) - 1.0) /
								(1.0 - config->elasmu);

		//future considerations: Social Cost of Carbon, production and consumption marginals??
	
	}

	//Discounted Utility of Consumption (this is the ojective function)
	double tempsum;
	tempsum = 0.0;
	
	for (int t=0; t<config->nDecades; t++)
	{
		tempsum = tempsum + 
					10.0 * config->rr[t] * config->pop[t] * 
						endog->utility[t] / config->scale1;
	}

	endog->discounted_utility = tempsum + config->scale2;

	//Present Value Calculations for damages and abatement costs
	// uses real interest rate computed above to calculate discount factors
	// (not the rr pure rate of time preference discount factors
	//  which only apply to utility)
	
	endog->pv_damages[0] = endog->damages[0];
	endog->pv_abate_cost[0] = endog->abate_cost[0];
	endog->pv_total_cost[0] = endog->total_cost[0];
	endog->rir_disc_factor[0] = 1.0;

	for (int t=1; t<config->nDecades; t++)
	{
		endog->rir_disc_factor[t] = 1.0/pow(1.0 + endog->real_int_rate[t],10.0*double(t));

		endog->pv_damages[t] =
			endog->pv_damages[t-1] + endog->rir_disc_factor[t] * endog->damages[t];
		endog->pv_abate_cost[t] = 
			endog->pv_abate_cost[t-1] + endog->rir_disc_factor[t] * endog->abate_cost[t];
		endog->pv_total_cost[t] = 
			endog->pv_total_cost[t-1] + endog->rir_disc_factor[t] * endog->total_cost[t];
	}
	
	
	// all variables/time series written out in model dump
	// do this only as necessary
	if (writeModelDump == 1)
	{
		ofstream out;
		ostringstream numstring;
		numstring << samplenum+1;
		string outfileName = "modelout_"+numstring.str()+".txt";
		out.open(outfileName.c_str());
		if (!out.is_open()) {cout << "Failed to open output dump file. Exiting ..."; exit(4);}
		out.setf(ios_base::fixed);
		out.precision(6);

		
		//write the detailed model output /dump (resembles GAMS output/ref data input)
		// this will be a touch tedious for first row / want a 60 column (decade) file
		int ilim = config->nDecades;
		double *tempval = new double[ilim];
		//first row is a bit of a pain to set up
		tempval[0] = endog->discounted_utility;
		tempval[1] = 0.0;
		tempval[2] = 0.0;
		tempval[3] = config->elasmu;
		tempval[4] = config->prstp;
		tempval[5] = config->pop0;
		tempval[6] = config->gpop0;
		tempval[7] = config->popasym;
		tempval[8] = config->a0;
		tempval[9] = config->ga0;
		tempval[10] = config->dela;
		tempval[11] = config->dk;
		tempval[12] = config->gama;
		tempval[13] = config->k0;
		tempval[14] = config->sig0;
		tempval[15] = config->gsigma;
		tempval[16] = config->dsig;
		tempval[17] = config->dsig2;
		tempval[18] = config->eland0;
		tempval[19] = config->mat2000;
		tempval[20] = config->mup2000;
		tempval[21] = config->mlo2000;
		tempval[22] = config->b12;
		tempval[23] = config->b23;
		tempval[24] = config->t2xco2;
		tempval[25] = config->fex0;
		tempval[26] = config->fex1;
		tempval[27] = config->tocn0;
		tempval[28] = config->tatm0;
		tempval[29] = config->c1;
		tempval[30] = config->c3;
		tempval[31] = config->c4;
		tempval[32] = config->fco22x;
		tempval[33] = config->a1;
		tempval[34] = config->a2;
		tempval[35] = config->a3;
		tempval[36] = config->theta2;
		tempval[37] = config->pback0;
		tempval[38] = config->backrat;
		tempval[39] = config->gback;
		tempval[40] = config->fosslim;
		tempval[41] = config->partfrac1;
		tempval[42] = config->partfrac2;
		tempval[43] = config->partfracn;	
		tempval[44] = config->dpartfrac;
		tempval[45] = config->scale1;
		tempval[46] = config->scale2;
		tempval[47] = config->limmu;
		tempval[48] = config->dtree;
		tempval[49] = config->saverate0;
		tempval[50] = endog->fosslim_step;
		tempval[51] = endog->temp2_step;
		tempval[52] = endog->temp4_step;
		tempval[53] = endog->temp6_step;
		tempval[54] = endog->temp8_step;
		tempval[55] = endog->temp10_step;
		tempval[56] = endog->mat2x_step;
		tempval[57] = endog->mat25x_step;
		tempval[58] = endog->mat3x_step;
		tempval[59] = endog->model_fail_step;		
		//now write it out
		for (int i=0;i<ilim;i++) out << setw(17) << tempval[i]; out << endl;
		//next row has date series
		//rest of rows should be more straightforward
		for (int i=0;i<ilim;i++) out << setw(17) << config->dateSeries[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->control_rate[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->forc[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->tatm[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->tocn[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->mat[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->mup[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->mlo[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->emiss_tot[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->kap[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->consum[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->percap_consum[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->percap_prod[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->invest[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->savings[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->real_int_rate[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->production[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->gross_production[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->net_production[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->damages[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->abate_cost[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->emiss_cum[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->utility[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->social_cost_carbon[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->ctax[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->ctax_part[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->margy[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->margc[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->gfacpop[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->pop[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->ga[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->tfp[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->gsig[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->sigma[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->theta1[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->etree[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->rr[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->forcoth[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << config->phi[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->abatement_factor[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->damages_factor[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->pv_abate_cost[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->pv_damages[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->rir_disc_factor[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->total_cost[i]; out << endl;
		for (int i=0;i<ilim;i++) out << setw(17) << endog->pv_total_cost[i]; out << endl;
		
		
		//cout << "wrote model output dump for this model execution" << endl;
		
		delete tempval;
		//close the output file
		out.close();
	
	}

	return;
} 

//*****************************************************************************
//  processModel: the 'guts' of the DICE model for one parameter/factor sample
//    - calculate the exogenous time series
//    - do time step 1 of both climate and econ model components
//    - loop the remaining time steps (climate and econ)
//    - calculate the objective function and other model results
//    - optionally write a model "dump" for this sample
//*****************************************************************************
void processModel(DICE *dice, int writeModelDump, int samplenum)
{

	Config *config = &dice->config;
	Endog  *endog  = &dice->endog;

	//char dummychar;
	//cout << "processModel: calling calcExog\n";
	//cin >> dummychar;
	
	// calculate exogenous time series for this sample
	calcExog(dice);
	
	//cout << "processModel: calling calcClim1\n";
	//cin >> dummychar;

	// do first time step of climate model
	calcClim1(dice);
	
	//cout << "processModel: calling calcEcon1\n";
	//cin >> dummychar;

	//do first time step of economic model
	calcEcon1(dice);

	//cout << "processModel: stepping through the decades\n";
	//cin >> dummychar;
	
	//plod through the decades
	for (int tIndex=1; tIndex<config->nDecades; tIndex++)
	{
		calcClim(dice, tIndex);
		calcEcon(dice, tIndex);
	}
	
	//do the postprocessing

	//cout << "processModel: calling postProcess\n";
	//cin >> dummychar;
	
	postProcess(dice, writeModelDump, samplenum);
	
	
	return;
}

//#endif    //CDICEMOD


