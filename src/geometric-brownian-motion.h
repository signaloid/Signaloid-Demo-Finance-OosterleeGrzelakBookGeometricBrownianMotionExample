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

#pragma once

#include <stdlib.h>

/**
 *	@brief	Calculate and return the final price of an asset using the geometric
 *		Brownian motion path algorithm. The implementation is an adaptation
 *		of the original Python Code in C.
 *		https://github.com/LechGrzelak/QuantFinanceBook/blob/master/PythonCodes/Chapter%2014/Fig14_07.py#L32
 *
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo iterations to run.
 *	@param	numberOfSteps			: Number of time steps that will be simulated.
 *	@param	periodicVolatility              : The periodic volatility of the model.
 *	@param	maturityTime		        : The maturity time.
 *	@param	periodicMeanReturn              : The periodic mean return.
 *	@param	initialPortfolioValue           : The initial portfolio value.
 *	@param	benchmarkingDataSamples 	: A pointer to an array where samples for each different path are stored.
 *	@return	double				: Returns the last element of `benchmarkingDataSamples`.
 */
double	geometricBrownianMotion(
        size_t          numberOfMonteCarloIterations,
        size_t          numberOfSteps,
        double          periodicVolatility, 
        double          maturityTime,
        double          periodicMeanReturn,
        double          initialPortfolioValue,
        double *        benchmarkingDataSamples
);

/**
 *	@brief	Calculate the call option payoff based on the given stock price at maturity
 *		and strike price.
 *
 *	@param	stockPriceAtMaturity	: The stock price at maturity of the asset.
 *	@param	strikePrice		: The strike price of the asset.
 *	@return double			: Returns the call option payoff.
 */
double	callOptionPayoff(double stockPriceAtMaturity, double strikePrice);

/**
 *	@brief	Calculate the put option payoff based on the given stock price at maturity
 *		and strike price.
 *
 *	@param	stockPriceAtMaturity	: The stock price at maturity of the asset.
 *	@param	strikePrice		: The strike price of the asset.
 *	@return double			: Returns the put option payoff.
 */
double	putOptionPayoff(double stockPriceAtMaturity, double strikePrice);

/**
 *	@brief	Calculate the simulated returns of portfolio.
 *
 *	@param	stockPriceAtMaturity	: The stock price at maturity of the asset.
 *      @param	initialPortfolioValue	: The initial value of the portfolio.
 *	@return double			: Returns the simulated returns of the asset.
 */
double	computeSimulatedReturns(double stockPriceAtMaturity, double initialPortfolioValue);


/**
 *	@brief	Calculate the value at risk (VaR) based on the given stock price at maturity,
 *		the initial portfolio value, and the quantile probability.
 *
 *	@param	simulatedReturns	: The simulated returns of the asset.
 *      @param	quantileProbability	: The quantile at which to measure the VaR.
 *	@return double			: Returns the VaR, which is a scalar value.
 */
double	computeValueAtRisk(double simulatedReturns, double quantileProbability);
