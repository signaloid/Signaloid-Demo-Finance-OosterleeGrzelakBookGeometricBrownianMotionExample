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
 *		Brownian motion path algorithm, with one independent sample path
 *		per Monte Carlo iteration.
 *
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo iterations to run.
 *	@param	numberOfSteps			: Number of time steps that will be simulated.
 *	@param	periodicVolatility		: The periodic volatility of the model.
 *	@param	maturityTime			: The maturity time.
 *	@param	riskFreeRate			: The risk-free rate.
 *	@param	initialPortfolioValue		: The initial portfolio value.
 *	@param	monteCarloOutputSamples		: A pointer to an array where samples for each different path are stored.
 *	@return	double				: Returns the last element of `monteCarloOutputSamples`.
 */
double
geometricBrownianMotionMonteCarlo(
	size_t      numberOfMonteCarloIterations,
	size_t      numberOfSteps,
	double      periodicVolatility,
	double      maturityTime,
	double      riskFreeRate,
	double      initialPortfolioValue,
	double *    monteCarloOutputSamples);

/**
 *	@brief	Calculate the discounted call option payoff for each Monte Carlo
 *		sample as `fmax(stockPriceAtMaturity - strikePrice, 0) *
 *		zeroCouponValue` and write the result into `payoffs`. `payoffs`
 *		and `stockPricesAtMaturity` may alias for in-place computation.
 *		If `payoffs` is `NULL`, the array is not written but the function
 *		still returns the last sample's discounted payoff.
 *
 *	@param	payoffs				: Output array of discounted payoffs (length `numberOfMonteCarloIterations`), or `NULL` to skip writing.
 *	@param	stockPricesAtMaturity		: Input array of stock prices at maturity.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples.
 *	@param	strikePrice			: The strike price of the asset.
 *	@param	zeroCouponValue			: Discount factor applied to each payoff.
 *	@return double				: Returns the discounted payoff for the last sample.
 */
double
callOptionPayoffMonteCarlo(
	double *        payoffs,
	const double *  stockPricesAtMaturity,
	size_t          numberOfMonteCarloIterations,
	double          strikePrice,
	double          zeroCouponValue);

/**
 *	@brief	Calculate the discounted put option payoff for each Monte Carlo
 *		sample as `fmax(strikePrice - stockPriceAtMaturity, 0) *
 *		zeroCouponValue` and write the result into `payoffs`. `payoffs`
 *		and `stockPricesAtMaturity` may alias for in-place computation.
 *		If `payoffs` is `NULL`, the array is not written but the function
 *		still returns the last sample's discounted payoff.
 *
 *	@param	payoffs				: Output array of discounted payoffs (length `numberOfMonteCarloIterations`), or `NULL` to skip writing.
 *	@param	stockPricesAtMaturity		: Input array of stock prices at maturity.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples.
 *	@param	strikePrice			: The strike price of the asset.
 *	@param	zeroCouponValue			: Discount factor applied to each payoff.
 *	@return double				: Returns the discounted payoff for the last sample.
 */
double
putOptionPayoffMonteCarlo(
	double *        payoffs,
	const double *  stockPricesAtMaturity,
	size_t          numberOfMonteCarloIterations,
	double          strikePrice,
	double          zeroCouponValue);

/**
 *	@brief	Calculate the simulated returns for each Monte Carlo sample as
 *		`stockPriceAtMaturity - initialPortfolioValue` and write the result
 *		into `simulatedReturns`. `simulatedReturns` and
 *		`stockPricesAtMaturity` may alias for in-place computation.
 *
 *	@param	simulatedReturns		: Output array of simulated returns (length `numberOfMonteCarloIterations`).
 *	@param	stockPricesAtMaturity		: Input array of stock prices at maturity.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples.
 *	@param	initialPortfolioValue		: The initial value of the portfolio.
 *	@return double				: Returns the simulated return for the last sample.
 */
double
computeSimulatedReturnsMonteCarlo(
	double *        simulatedReturns,
	const double *  stockPricesAtMaturity,
	size_t          numberOfMonteCarloIterations,
	double          initialPortfolioValue);

/**
 *	@brief	Calculate the value at risk (VaR) from an array of Monte Carlo
 *		simulated-returns samples at the given quantile probability.
 *
 *	@param	simulatedReturns		: Array of Monte Carlo simulated returns samples.
 *	@param	numberOfMonteCarloIterations	: Number of Monte Carlo samples.
 *	@param	quantileProbability		: The quantile at which to measure the VaR.
 *	@return double				: Returns the VaR, which is a scalar value.
 */
double
computeValueAtRiskMonteCarlo(
	const double *  simulatedReturns,
	size_t          numberOfMonteCarloIterations,
	double          quantileProbability);
