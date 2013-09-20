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

#ifndef __cdice_h
#define __cdice_h

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <cstring>


using namespace std;

#define ROUNDINT(x) int(x + 0.5)
#define ROUNDDOUBLE(x) double(int(x + 0.5))

#ifndef EPS
    #define EPS 10.0E-9
#endif
#define TRUE 1
#define FALSE 0

//This macro check for equality of a double precision variable within eps
#define ISEQUAL(x,y) (x <= y+EPS && x >= y-EPS)


struct Files
{
    // file names
	string factorfileName;           //parameter set(s) to be tested; overrides exogenous
	string saveratefileName;         //savings rate input  (mode 1)
	string emisctlfileName;          //emission control rate input  (mode 1; mode 2)
	string participationfileName;    //participation rate input  (mode 1, not currently used)
	string exogenousfileName;        //all standard values of exogenous factors  (mode 1; mode 2)
			
};


// structure Config describes all the exogenous factors/time series
// required to run an instance of the CDICE model
// Names in parentheses are the variable names in GAMS code, if different).
struct Config
{
    //Preference Factors
	double prstp;        //inital rate of social time preference per year
	double elasmu;       //elasticity of marginal utility of consumption
	double *rr; //[60];  //average utility social discount rate (used to calc obj fn)
		
	//Labor/Population and Productivity
	double pop0;        //2005 world population in millions
	double popasym;     //asymptotic population
	double gpop0;       //growth rate of population per decade
	double *gfacpop; //[60]; //growth factor for population
	double *pop;  //[60];    //level of population/labor (L)	
	double a0;          //initial level of total factor productivity
	double ga0;         //initial growth rate for technology per decade
	double dela;        //decline rate of technology change per decade
	double *ga; //[60]; //growth rate of technology
	double *tfp; //[60]; //level of total factor productivity (AL)
	
	//Capital Stock etc.
	double k0;          //2005 capital stock in trillions of US$
	double dk;          //depreciation rate of capital per year
	double gama;        //capital elasticity in production function
	double saverate0;   //first period savings rate (propagate)
	
	//Carbon Intensity and Emissions
	double sig0;        //2005 CO2eq/GWP emissions ratio
	double gsigma;      //initial growth rate of sigma per decade
	double dsig;        //decline rate of decarbonization per decade
	double dsig2;       //quadratic term in decarbonization
	double *gsig; //[60];   //cumulative improvement in energy efficiency
	double *sigma; //[60];  //CO2eq/GWP emissions ratio (carbon intensity)
	double eland0;      //2005 carbon emissions from land use change (GtC/decade)
	double dtree;      //change in carbon emissions from land use (per decade) (not named in GAMS)
	double *etree; //[60]; //emissions from land use change
	double fosslim;    //maximum available extractable fossil fuels (GtC) [constraint check only]
	
	//Carbon Cycle Box Model 
	double mat2000;    //2005 atmospheric burden (GtC)
	double mup2000;    //2005 upper stratum burden (land & upper ocean) (GtC)  (MU)
	double mlo2000;    //2005 lower stratum burden (deep ocean) (GtC)   (ML)
	//following coeffcients control rate of transfer of carbon between carbon cycle boxes
	//only two of these (b12 and b23) need to be specifed (others are derived)
	double b11;       //remains in atmosphere
	double b12;       //atmosphere to upper stratum
	double b21;       //upper stratum to atmosphere
	double b22;       //remains in upper stratum
	double b23;       //upper stratum to lower stratum
	double b32;       //lower stratum to upper stratum
	double b33;       //remains in lower stratum
	
	//Climate Box Model
	double fex0;      //estimate of 2000 forcings of non-CO2 GHGs
	double fex1;      //estimate of 2100 forcings of non-CO2 GHGs
	double *forcoth;  // forcings from non-CO2 GHGs
	double tatm0;     //2000 atmospheric temperature change since 1900 (degC)
	double tocn0;     //2000 ocean temperature change since 1900 (degC) (TOCEAN0)
	double preanthco2; //pre-anthropogenic forcing (W/m2)
	double t2xco2;    //equilibrium temperature impact of CO2 doubling
	double fco22x;    //estimated forcings of equilibrium CO2 doubling
	double lambda;    //fco22x/t2xco2 (LAM)
	double c1;        //climate equation coefficient for the uppper level
	double c3;        //tranfer coefficient upper to lower level
	double c4;        //transfer coefficient for lower level
	
	//Climate Damages
	double a1;        //damage intercept coefficient (also known as AA1)
	double a2;        //damage quadratic term coefficient (aka AA2)
	double a3;        //damage exponent (assumed quadratic) (aka AA3)
	
	//Abatement Costs
	double pback0;    //2005 cost of backstop (thousand US$/tC replaced) (PBACK)
	double theta2;    //exponent of cost control function (EXPCOST2)
	double backrat;   // ratio of initial to final backstop
	double gback;     //initial decline in backstop cost (percent per decade)
	double *theta1; //[60]; //adjusted cost for backstop (COST1)
	
	//Participation in emissions controls (who pays for abatment)
	// This is currently initialized at full participation; 
	// partfrac21 (GAMS) is partfracn (the ceiling value at time step 25)
	double partfrac1;  //2005 fraction of economy under control regime
	double partfrac2;  //2015 fraction of economy under control regime
	double partfracn;  //fraction of economy under control regime at time step 25
	double dpartfrac;  //decline rate of participation
	double *phi; //[60]; //fraction of emissions in control regime (PARTFRACT)

	// for internal generation of control rate time series: (not done yet)
	double limmu;      //upper limit on control rate (LIMMIU) (typically 1.0)
	double controlrate0; //first period control rate (if generating internally)
	
	//Scaling coefficients in discounted utility function (objective function)
	//     (which are tuned in GAMS - and are relatively arbitrary here)
	double scale1;    //multiplicative scaling coefficient
	double scale2;    //additive scaling coefficient
	
	//Miscellaneous other stuff
	int nDecades;     //defines length of integration (60 decades)
	double startYear; //the mid-decade date of the first decade in the execution
	int mode;         //defines mode of execution: 
	                  // mode 1 is simulation with emission control series,
	                  //   savings rates (and participation rates input)
					  //   Participation rate time series can be input in
					  //   in mode 1 by uncommenting code in 2 places:
	                  //      init_CDICE in CDICEMain.cpp
	                  //      calcExog in CDICE.cpp
				      // mode 2 is simulation with emission control series input
	double *dateSeries; //[60]; //mid-decade dates in time series

};

//structure Endog describes all the GAMS decision variables (investment and
//emission control rate) and the endogenous variables calculated by
//the model. Names in parentheses are the names in the GAMS code.
// Note that emission control rate is fraction of production which is
// carbon-free. (Participation rate determines fraction of economy that pays
// for the abatement.)
struct Endog
{
	//Economic Model Variables
	double *kap; //[60];              //capital stock  (K)
	double *gross_production; //[60]; //gross output of production function (YGROSS)
	double *net_production; //[60];   //output net of damages, not abatement (YNET)
	double *production; //[60];       //output net of both damages and abatement costs (Y)
	double *abatement_factor; //[60]; //intermediate variable
	double *damages_factor; //[60];   //intermediate variable
	double *abate_cost; //[60];       //abatement costs (ABATECOST)	
	double *damages; //[60];          //climate damages
	double *total_cost; //[60];       //sum abatement costs and climate damages
	double *emiss_ind; //[60];        //industrial emissions
	double *emiss_tot; //[60];        //total emissions (industrial plus land use) (E)
	double *emiss_cum; //[60];        //cumulative emissions (CCA)
	double *invest; //[60];           //investment (I)
	double *savings; //[60];          //savings (S)
	double *consum; //[60];           //consumption (C)
	double *control_rate; //[60];     //emission control rate (MIU)
	double *utility; //[60];          //utility of consumption for time step (PERIODU)
	double discounted_utility;        //discounted utility; the objective function (UTILITY)
	//note the following are derived variables
	double *ctax; //[60];             //carbon tax
	double *ctax_part; //[60];        //carbon tax accounting for less than full participation 
	double *percap_consum; //[60];    // per capita consumption (CPC)
	double *percap_prod; //[60];      //per capita output  (PCY)
	double *real_int_rate; //[60];    //"real interest rate per annum" (RI) - used to calc PVs:
	double *rir_disc_factor; //[60];  //real interest rate discount factors
	double *pv_damages; //[60];       //present value of damages to date
	double *pv_abate_cost; //[60];    //present value of abatement costs to date
	double *pv_total_cost; //[60];    //present value of (damages + abatement costs) to date
	//note the following to be implemented in future (maybe)
	double *social_cost_carbon; //[60]; //social cost of carbon
	double *margy; //[60];            //production marginal
	double *margc; //[60];            //consumption marginal
	
	
	//Carbon and Climate Model Variables (note matav calculated, but not currently used)
	double *mat; //[60];       //mass of carbon in atmosphere (GtC)
	double *mup; //[60];       //mass of carbon in upper stratum (GtC) (MU)
	double *mlo; //[60];       //mass of carbon in lower stratum (GtC) (ML)
	double *matav; //[60];     //two decade average of mass of carbon in atmosphere (Gt C)
	double *tatm; //[60];      //atmospheric temperature increase (degC)
	double *tocn; //[60];      //ocean temperature increase (degC) (TOCEAN in GAMS)
	double *forc; //[60];      //atmospheric radiative forcing (W/m2)

	//Constraint trackers
	int fosslim_step;      //time step at which the nominal limit is exceeded
	int temp2_step;        //time step at which 2C exceeded
	int temp4_step;        //time step at which 4C exceeded
	int temp6_step;        //time step at which 6C exceeded
	int temp8_step;        //time step at which 8C exceeded
	int temp10_step;       //time step at which 10C exceeded
	int mat2x_step;        //time step at which 2XCO2 exceeded
	int mat25x_step;       //time step at which 2.5XCO2 exceeded
	int mat3x_step;        //time step at which 3XCO2 exceeded
	int model_fail_step;   //time step at which any of the following first occurs:
	                       //  abatement_factor >= 1.00 (fixed to 0.95)
	                       //  capital stock falls below 10.0 (fixed to 10.0)
	                       //  consumption falls below 10.0 (fixed at 10.0)

};






// define the uber-structure

struct DICE
{
	
	Files files;
	Config config;
	Endog endog;
	
};	

//Create an Individual structure (key results) 
struct Individual {

     double *xreal;    //these are the parameters being tested
     double *obj;      //the objective value (discounted utility)
	 double *tag_alongs1;  //objective function, other whole run metrics
	 double *tag_alongs2;  //climate-related metrics for sensitivity testing
	 double *tag_alongs3;  //economic-related metrics for sensitivity testing
	 double *tag_alongs4;  //more economic-related metrics for sensitivity testing
	 double *tag_alongs5;  //even more economic-related metrics

};

//Function prototypes for all the functions defined here

//make sure these match up with the functions!

// functions in CDICEMain:
void init_CDICE();  //read control file; allocate/initialize data structures
void calc_CDICE(Individual *ind, int samplenum);  // sets up a model execution
void finalize_CDICE();  //close files/end processing
//void main();

// functions in CDICEInput:
void allocateDICE(DICE *dice, int control_scenario, int control_startyear);  //to allocate and initialize DICE structure/contents
void readSavingsRate(DICE *dice);                     //externally supplied savings rate time series
void readControlRate(DICE *dice);                     //externally supplied emission control rate time series
void readParticipationRate(DICE *dice);               //externally supplied participation rate time series
void readExogenousFactors(DICE *dice);                //all non time series exogenous variables/factors

//functions in CDICE:  //  the DICE model code is here
void processModel(DICE *dice, int writeModelDump, int samplenum);  //accomplish one model execution
void calcExog(DICE *dice);   // calculate exogenous time series
void calcClim1(DICE *dice);   // calculate climate submodel time step1
void calcEcon1(DICE *dice);   //calculate economic submodel time step1
void calcClim(DICE *dice, int t);  // calculate climate model time step
void calcEcon(DICE *dice, int t);  // calculate economic model time step
void postProcess(DICE *dice, int writeModelDump, int samplenum);   // calculate derived final model results



#endif // __cdice_h

//#endif //CDICEMOD




