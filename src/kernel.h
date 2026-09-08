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
#include "utilities.h"

/**
 *	@brief	Simulate a single geometric Brownian motion path from `t = 0` to
 *		`t = maturityTime` in `numberOfSteps` steps and return the asset
 *		price at maturity. The white-noise increment uses
 *		`UxHwDoubleGaussDist`: in UxHw mode this returns the full standard
 *		normal distribution, yielding a distributional price at maturity;
 *		in Monte Carlo mode it returns a single sample, yielding one
 *		Monte Carlo sample price.
 *
 *	@param	numberOfSteps		: Number of time steps to simulate.
 *	@param	periodicVolatility	: The periodic volatility of the model.
 *	@param	maturityTime		: The maturity time.
 *	@param	riskFreeRate		: The risk-free rate.
 *	@param	initialPortfolioValue	: The initial portfolio value.
 *	@return	double			: Returns the asset price at maturity.
 */
double
geometricBrownianMotionSinglePath(
	size_t  numberOfSteps,
	double  periodicVolatility,
	double  maturityTime,
	double  riskFreeRate,
	double  initialPortfolioValue);

/**
 *	@brief	UxHw-mode calculation kernel. Computes the selected output(s) using
 *		distributional arithmetic on a single path. Writes per-output
 *		results into `outputVariables` and the single distributional asset
 *		price / payoff / return into `monteCarloOutputSamples[0]`.
 *
 *	@param	arguments		: Command-line arguments.
 *	@param	outputVariables		: Array of size `kOutputVariableIndexMax` to fill.
 *	@param	monteCarloOutputSamples	: Single-element array for the distributional result.
 *	@return	double			: Returns the value of the selected output (or the put option payoff when all outputs are selected).
 */
double
calculateOutputUxHw(
	CommandLineArguments *  arguments,
	double *                outputVariables,
	double *                monteCarloOutputSamples);

/**
 *	@brief	Monte Carlo calculation kernel. Runs
 *		`arguments->common.numberOfMonteCarloIterations` independent paths
 *		into `monteCarloOutputSamples` and computes the selected output(s)
 *		over the sample array using `fmax`-based payoff math (no UxHw
 *		distributional API). Writes per-output results into
 *		`outputVariables`.
 *
 *	@param	arguments		: Command-line arguments.
 *	@param	outputVariables		: Array of size `kOutputVariableIndexMax` to fill.
 *	@param	monteCarloOutputSamples	: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@return	double			: Returns the value of the selected output (or the put option payoff when all outputs are selected).
 */
double
calculateOutputMonteCarlo(
	CommandLineArguments *  arguments,
	double *                outputVariables,
	double *                monteCarloOutputSamples);
