#include "plotBiquad.h"
#include <math.h>
#include "types.h"

plotBiquad g_plotBiquad;

plotBiquad::plotBiquad()
{
	A1 = A2 =  B1 = B2 = 0;
	B0 = 1;
//	setParameter(0, EQ_PEAKING, 1000,0, 4.3);
}


plotBiquad::~plotBiquad()
{
}


double plotBiquad::slopeToQ(double slope) {

	/* Convert slope to Q for equations - The PA+ shelf slope to Q conversion is different than the others.
	* It was defined to map the following slopes to Qs: [0 3 4.5 6 9 12] -> [0 0.5 1.0 1.5 2.5 4.0]
	* The equation below is a 4th degree polynomial fit to that discrete mapping to allow for a continuous approximation */
	double slope2nd = slope*slope;    // slope^2
	double slope3rd = slope2nd*slope; // slope^3
	double slope4th = slope3rd*slope; // slope^4
	double QShelf = 0.00052122f*slope4th - 0.012491f*slope3rd + 0.10729f*slope2nd - 0.056105f*slope - 0.00017533f;

	return QShelf;
}

void plotBiquad::setParameter(bool enable, int ftype, double f0, double dBgain,double q_or_slope) {
	double w0, sn, cs, alpha,A;
    double b0,b1,b2,a0,a1,a2;

	double Q = q_or_slope;
	double slope = q_or_slope;

	w0 = 2.f * PI * f0 / FS;
	sn = sin(w0);
	cs = cos(w0);
	alpha = sn / (2.f * Q);
    if(dBgain<0.0000001 && dBgain>0) dBgain=0;
	if(dBgain<0 && dBgain>-0.0000001) dBgain=0;

	A = pow(10, (dBgain / 40));    //(for EQ_PEAKING and shelving EQ filters only)
    
	switch (ftype) {
	case EQ_LPF:

		b0 = (1 - cs) / 2;
		b1 = 1 - cs;
		b2 = (1 - cs) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;

	case EQ_HPF:
		b0 = (1 + cs) / 2;
		b1 = -(1 + cs);
		b2 = (1 + cs) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;

	case EQ_BPF:
		b0 = alpha;
		b1 = 0;
		b2 = -alpha;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;

	case EQ_NOTCH:
		b0 = 1;
		b1 = -2 * cs;
		b2 = 1;
		a0 = 1 + alpha;
		a1 = -2 * cs;
		a2 = 1 - alpha;
		break;
    case EQ_APF:
            b0 =   1 - alpha;
            b1 =  -2*cs;
            b2 =   1 + alpha;
            a0 =   1 + alpha;
            a1 =  -2*cs;
            a2 =   1 - alpha;
            break;
	case EQ_PEAKING:
	{
		//This comes from the matlab script DR_eq_tbl_PA.m for PEQ constant Q type.
		
		double G = pow(10.0, dBgain*0.05f);
		double Gsq = G*G;

		double P = tan(w0*0.5f);
		double Psq = P*P;

		// Find Gb2, use G-3db for |Gdb|>=6 and geometric mean for |Gdb|<6
		double Gb2 = 0.0;
		if (dBgain >= 6.0)
			Gb2 = Gsq*0.5;
		else if (dBgain <= -6.0)
			Gb2 = Gsq*2.0;
		else
			Gb2 = G;

		//Compute Aq = sqrt(fabs(Gb2 - 1) / (fabs(Gsq - Gb2))) / q;
		double temp1 = Gb2 - 1;
		double temp2 = Gsq - Gb2;

		if (temp1 < 0)
			temp1 = -temp1;
		if (temp2 < 0)
			temp2 = -temp2;

		double Aq = sqrt(temp1 / temp2) / Q;
		double Bq = Aq*G;

		// set 0 dB gain A's and B's to 1 to prevent large coefficients
		if (dBgain == 0.0)
		{
			Aq = 1.0;
			Bq = 1.0;
		}

		a0 = (1 + Aq*P + Psq);
		a1 = 2 * (Psq - 1);
		a2 = (1 - Aq*P + Psq);
		b0 = (1 + Bq*P + Psq);
		b1 = a1;
		b2 = (1 - Bq*P + Psq);
	}
		break;
            
	case EQ_L_SHELF:
	{
		//This comes from the matlab script DR_eq_tbl_PA.m for High Shelf constant Q type. (\\HPro\lumberjack\HProFilterAnalysis\DriveRackPA\matlab)
		//DR_eq_tbl_PA.m was used to generate a table of coefficients for the DriveRack PA,PX,PA+ PEQ.

		double P = 1.0f / tan(w0*0.5); //the 1/stuff is because I hijacked the high pass design and need to do this to make it lowpass
		double Psq = P*P;
		//				const double Gmax = 5.623413251903f;// pow(10.0f,15.0f/20.0f)

		double QShelf = slopeToQ(slope);
		//double Qc = ceil(QShelf); //Probably what was intended, if not for the bug
		double Qc = 4.0; //This recreates the bug in the original table creation code

						// Constant Q shelf
						// Find Gc, use G-3db for |Gdb|>=6 and geometric mean for |Gdb|<6
		double G = powf(10.0f, dBgain*0.05);
		double Gc = 0.0;
		if (dBgain >= 6.0)
			Gc = (G / SQRT2)*Qc;	// Gdb-3db
		else if (dBgain > 0.0)
			Gc = sqrt(G)*Qc;		// Geometric mean = sqrt(G)
		else if (dBgain == 0.0)
			Gc = G + 1e-6f; 			// G = 1. Add a little bit to avoid divide by 0 when computing Gc
		else if (dBgain > -6.0)
			Gc = sqrt(G) / Qc;	    // Geometric mean = sqrt(G)
		else
			Gc = G*SQRT2 / Qc;		// Gdb-3db

		double G0 = G - (Gc*(G - 1.0));	// Adjusts to make 3db point at Fc - Where does this come from?

		double Aq = sqrt(sqrt(QShelf)*fabs(Gc*Gc - G0) / fabs(G*G - Gc*Gc));
		double Bq = Aq*sqrt(G);
		double Asq = Aq*Aq;
		double Bsq = Bq*Bq;


		a0 = (QShelf*Psq + 2 * P*Aq + Asq);
		a1 = -2 * (QShelf*Psq - Asq);
		a2 = (QShelf*Psq - 2 * P*Aq + Asq);
		b0 = (QShelf*Psq + 2 * P*Bq + Bsq);
		b1 = -2 * (QShelf*Psq - Bsq);
		b2 = (QShelf*Psq - 2 * P*Bq + Bsq);
	}
		break;

	case EQ_H_SHELF: 
	{
		//This comes from the matlab script DR_eq_tbl_PA.m for High Shelf constant Q type. (\\HPro\lumberjack\HProFilterAnalysis\DriveRackPA\matlab)
		//DR_eq_tbl_PA.m was used to generate a table of coefficients for the DriveRack PA,PX,PA+ PEQ.

		double P = tan(w0*0.5);
		double Psq = P*P;
		//				const double Gmax = 5.623413251903f;// pow(10.0f,15.0f/20.0f)

		double QShelf = slopeToQ(slope);
		//double Qc = ceil(QShelf); //Probably what was intended, if not for the bug
		double Qc = 4.0; //This recreates the bug in the original table creation code

						// Constant Q shelf
						// Find Gc, use G-3db for |Gdb|>=6 and geometric mean for |Gdb|<6
		double G = powf(10.0f, dBgain*0.05);
		double Gc = 0.0;
		if (dBgain >= 6.0)
			Gc = (G / SQRT2)*Qc;	// Gdb-3db
		else if (dBgain > 0.0)
			Gc = sqrt(G)*Qc;		// Geometric mean = sqrt(G)
		else if (dBgain == 0.0)
			Gc = G + 1e-6f; 			// G = 1. Add a little bit to avoid divide by 0 when computing Gc
		else if (dBgain > -6.0)
			Gc = sqrt(G) / Qc;	    // Geometric mean = sqrt(G)
		else
			Gc = G*SQRT2 / Qc;		// Gdb-3db

		double G0 = G - (Gc*(G - 1.0));	// Adjusts to make 3db point at Fc - Where does this come from?

		double Aq = sqrt(sqrt(QShelf)*fabs(Gc*Gc - G0) / fabs(G*G - Gc*Gc));
		double Bq = Aq*sqrt(G);
		double Asq = Aq*Aq;
		double Bsq = Bq*Bq;

		a0 = (QShelf*Psq + 2 * P*Aq + Asq);
		a1 = 2 * (QShelf*Psq - Asq);
		a2 = (QShelf*Psq - 2 * P*Aq + Asq);
		b0 = (QShelf*Psq + 2 * P*Bq + Bsq);
		b1 = 2 * (QShelf*Psq - Bsq);
		b2 = (QShelf*Psq - 2 * P*Bq + Bsq);
	}
		break;

	}
	if (enable) {
		double a0Inv = 1 / a0;
		B0 = b0 * a0Inv;
		B1 = b1 * a0Inv;
		B2 = b2 * a0Inv;
		A1 = a1 * a0Inv;
		A2 = a2 * a0Inv;
	}
	else {
		B0 = 1;
		B1 = 0;
		B2 = 0;
		A1 = 0;
		A2 = 0;
	}
}


void plotBiquad::plotCurve(double *x, double *y, long pointslength) {
	double w;
	double numerator;
	double denominator;
	double magnitude;
    double cs,cs2;
    


	for (int i = 0; i < pointslength; i++) {
		w = 2.0L*PI*x[i] / FS; //x : frequency point
        cs=cos(w) ;
        cs2=cos(2*w);
		numerator = B0*B0 + B1*B1 + B2*B2 + 2.0*(B0*B1 + B1*B2)*cs+ 2.0*B0*B2*cs2;
		denominator = 1.0 + A1*A1 + A2*A2 + 2.0*(A1 + A1*A2)*cs + 2.0*A2*cs2;
		magnitude = (numerator / denominator);
        
		y[i] = 10*log10(magnitude); //FrequencyResponse
	}
}
