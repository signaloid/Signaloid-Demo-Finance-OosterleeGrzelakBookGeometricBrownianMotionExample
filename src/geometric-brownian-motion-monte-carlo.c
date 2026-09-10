/*
 *	Copyright (c) 2024-2026, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#include <stddef.h>
#include <math.h>
#include "geometric-brownian-motion-monte-carlo.h"
#include "kernel.h"
#include "common.h"

double
geometricBrownianMotionMonteCarlo(
	size_t      numberOfMonteCarloIterations,
	size_t      numberOfSteps,
	double      periodicVolatility,
	double      maturityTime,
	double      riskFreeRate,
	double      initialPortfolioValue,
	double *    monteCarloOutputSamples)
{
	for (size_t jj = 0; jj < numberOfMonteCarloIterations; jj++)
	{
		monteCarloOutputSamples[jj] = geometricBrownianMotionSinglePath(
			numberOfSteps,
			periodicVolatility,
			maturityTime,
			riskFreeRate,
			initialPortfolioValue
		);
	}

	return monteCarloOutputSamples[numberOfMonteCarloIterations - 1];
}

double
callOptionPayoffMonteCarlo(
	double *        payoffs,
	const double *  stockPricesAtMaturity,
	size_t          numberOfMonteCarloIterations,
	double          strikePrice,
	double          zeroCouponValue)
{
	double payoff = 0.0;

	for (size_t ii = 0; ii < numberOfMonteCarloIterations; ii++)
	{
		payoff = fmax(stockPricesAtMaturity[ii] - strikePrice, 0.0) * zeroCouponValue;

		if (payoffs != NULL)
		{
			payoffs[ii] = payoff;
		}
	}

	return payoff;
}

double
putOptionPayoffMonteCarlo(
	double *        payoffs,
	const double *  stockPricesAtMaturity,
	size_t          numberOfMonteCarloIterations,
	double          strikePrice,
	double          zeroCouponValue)
{
	double payoff = 0.0;

	for (size_t ii = 0; ii < numberOfMonteCarloIterations; ii++)
	{
		payoff = fmax(strikePrice - stockPricesAtMaturity[ii], 0.0) * zeroCouponValue;

		if (payoffs != NULL)
		{
			payoffs[ii] = payoff;
		}
	}

	return payoff;
}

double
computeSimulatedReturnsMonteCarlo(
	double *        simulatedReturns,
	const double *  stockPricesAtMaturity,
	size_t          numberOfMonteCarloIterations,
	double          initialPortfolioValue)
{
	double returnSample = 0.0;

	for (size_t ii = 0; ii < numberOfMonteCarloIterations; ii++)
	{
		returnSample            = stockPricesAtMaturity[ii] - initialPortfolioValue;
		simulatedReturns[ii]    = returnSample;
	}

	return returnSample;
}

double
computeValueAtRiskMonteCarlo(
	const double *  simulatedReturns,
	size_t          numberOfMonteCarloIterations,
	double          quantileProbability)
{
	return calculatePercentageQuantileOfDoubleSamples(
		simulatedReturns,
		quantileProbability,
		numberOfMonteCarloIterations
	);
}
