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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "utilities.h"
#include "geometric-brownian-motion.h"


/**
 *	@brief	Calculates the output of the program, based on the command-line arguments. Writes
 *		to `outputVariables`, depending on the selected output and also writes the results
 *		to `monteCarloOutputSamples`, which needs to be allocated both in MonteCarlo Mode
 *		and in Laplace execution mode.
 *
 *	@param	arguments		: A pointer to the command-line arguments.
 *	@param	outputVariables		: A pointer to the array where the output values are written.
 *	@param	monteCarloOutputSamples	: A pointer to the array where the Monte Carlo samples are written.
 *	@return	double			: Returns the output value when a single output has been selected. If
 *						all outputs are selected, returns the output for `kOutputVariableIndexPutOptionPrice`.
 */
static double
calculateOutput(CommandLineArguments *  arguments, double *  outputVariables, double *  monteCarloOutputSamples)
{
	double		result;
	double		stockPriceAtMaturity;
	double		zeroCouponValue;
	double		valueAtRisk;
	double 		simulatedReturns;
	bool		calculateAllOutputs;
	size_t 		numberOfSteps;

	calculateAllOutputs 	= (arguments->common.outputSelect == kOutputVariableIndexMax);
	zeroCouponValue 	= exp(
					-arguments->periodicMeanReturn *
						(arguments->maturityTime -
						kGeometricBrownianMotionConfigDefaultStartDate)
					);

	/*
	 *	Compute number of steps in simulation based on the frequency.
	 */
	if (arguments->frequencyIndex == kGeometricBrownianMotionConfigFrequencyIndexDays)
	{
		numberOfSteps = (size_t)arguments->maturityTime * kGeometricBrownianMotionConfigFrequencyDays;
	}
	else if (arguments->frequencyIndex == kGeometricBrownianMotionConfigFrequencyIndexMonths)
	{
		numberOfSteps = (size_t)arguments->maturityTime * kGeometricBrownianMotionConfigFrequencyMonths;
	}
	else
	{
		numberOfSteps = (size_t)arguments->maturityTime * kGeometricBrownianMotionConfigFrequencyYears;
	}

	/*
	 *	First, calculate the distribution of prices at maturity.
	 */
	stockPriceAtMaturity = geometricBrownianMotion(
			arguments->common.numberOfMonteCarloIterations,
			numberOfSteps,
			arguments->periodicVolatility,
			arguments->maturityTime,
			arguments->periodicMeanReturn,
			arguments->initialPortfolioValue,
			monteCarloOutputSamples);


	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputVariableIndexStockPriceAtMaturity))
	{
		result = outputVariables[kOutputVariableIndexStockPriceAtMaturity] = stockPriceAtMaturity;
	}

	/*
	 *	Compute call option payoff.
	 */
	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputVariableIndexCallOptionPrice))
	{
		double	payoff;

		/*
		 *	Next, calculate the payoff based on price at maturity and strike price.
		 *	We will be replacing the price values in `monteCarloOutputSamples` with the
		 *	payoff values.
		 */
		for (size_t i = 0; i < arguments->common.numberOfMonteCarloIterations; i++)
		{
			payoff = callOptionPayoff(monteCarloOutputSamples[i], arguments->strikePrice);
			/*
			 *	Finally, multiply mean payoff by present value of zero-coupon bond
			 *	(see An Introduction to Quantitative Finance, Blyth,
			 *	page 4, first paragraph just below Figure 1.1):
			 */
			payoff = payoff * zeroCouponValue;
			if (arguments->common.outputSelect == kOutputVariableIndexCallOptionPrice)
			{
				monteCarloOutputSamples[i] = payoff;
			}
		}

		result = outputVariables[kOutputVariableIndexCallOptionPrice] = payoff;

	}

	/*
	 *	Compute put option payoff.
	 */
	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputVariableIndexPutOptionPrice))
	{
		double	payoff;

		/*
		*	Next, calculate the payoff based on price at maturity and strike price.
		*	We will be replacing the price values in `monteCarloOutputSamples` with the
		*	payoff values.
		*/
		for (size_t i = 0; i < arguments->common.numberOfMonteCarloIterations; i++)
		{
			payoff = putOptionPayoff(monteCarloOutputSamples[i], arguments->strikePrice);
			payoff = payoff * zeroCouponValue;
			if (arguments->common.outputSelect == kOutputVariableIndexPutOptionPrice)
			{
				monteCarloOutputSamples[i] = payoff;
			}
		}

		result = outputVariables[kOutputVariableIndexPutOptionPrice] = payoff;
	}

	/*
	 *	Compute simulated returns and/or value at risk.
	 */
	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputVariableIndexValueAtRisk) || (arguments->common.outputSelect == kOutputVariableIndexSimulatedReturns))
	{
		for (size_t i = 0; i < arguments->common.numberOfMonteCarloIterations; i++)
		{
			simulatedReturns = computeSimulatedReturns(monteCarloOutputSamples[i], arguments->initialPortfolioValue);
			monteCarloOutputSamples[i] = simulatedReturns;
		}

		if (calculateAllOutputs || (arguments->common.outputSelect == kOutputVariableIndexSimulatedReturns))
		{
			result = outputVariables[kOutputVariableIndexSimulatedReturns] = simulatedReturns;
		}

		if (calculateAllOutputs || (arguments->common.outputSelect == kOutputVariableIndexValueAtRisk))
		{
			if(arguments->common.isMonteCarloMode)
			{
				valueAtRisk = calculatePercentageQuantileOfDoubleSamples(
							monteCarloOutputSamples,
							arguments->quantileProbability,
							arguments->common.numberOfMonteCarloIterations);
			}
			else 
			{
				valueAtRisk = computeValueAtRisk(simulatedReturns, arguments->quantileProbability);
			}

			for (size_t i = 0; i < arguments->common.numberOfMonteCarloIterations; i++)
			{
				monteCarloOutputSamples[i] = valueAtRisk;
			}

			result = outputVariables[kOutputVariableIndexValueAtRisk] = valueAtRisk;
		}
	}
	return result;
}

int
main(int argc, char *  argv[])
{
	CommandLineArguments	arguments;
	double			output;
	double *		monteCarloOutputSamples = NULL;
	clock_t			start;
	clock_t			end;
	double			cpuTimeInSeconds;
	double			outputVariables[kOutputVariableIndexMax];
	const char *		outputVariableNames[kOutputVariableIndexMax] =
				{
					[kOutputVariableIndexStockPriceAtMaturity]	= "Stock price at maturity",
					[kOutputVariableIndexCallOptionPrice]		= "Call option payoff",
					[kOutputVariableIndexPutOptionPrice]		= "Put option payoff",
					[kOutputVariableIndexValueAtRisk]		= "Value at Risk",
					[kOutputVariableIndexSimulatedReturns]		= "Simulated Returns",
				};
	MeanAndVariance		meanAndVariance;

	/*
	 *	Get command-line arguments.
	 */
	if (getCommandLineArguments(argc, argv, &arguments))
	{
		return kCommonConstantReturnTypeError;
	}

	/*
	 *	MonteCarlo output samples are used even in the Laplace use case to store
	 *	the result of intermediate steps.
	 */
	monteCarloOutputSamples =
		(double *) checkedMalloc(
				arguments.common.numberOfMonteCarloIterations * sizeof(double),
				__FILE__,
				__LINE__);

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	/*
	 *	For this application, the Monte Carlo loop is in the core algorithm
	 *	called by `calculateOutput()`. We therefore call it just once, but we
	 *	pass it the `monteCarloOutputSamples` array to fill in with the
	 *	relevant samples.
	 */
	output = calculateOutput(&arguments, outputVariables, monteCarloOutputSamples);

	/*
	 *	If not doing Laplace version, then approximate the cost of the third phase of
	 *	Monte Carlo (post-processing), by calculating the mean and variance.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(monteCarloOutputSamples, arguments.common.numberOfMonteCarloIterations);
		output = outputVariables[arguments.common.outputSelect] = meanAndVariance.mean;
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end = clock();
		cpuTimeInSeconds = ((double) (end - start)) / CLOCKS_PER_SEC;
	}

	if (arguments.common.isBenchmarkingMode)
	{
		/*
		 *	In benchmarking mode, we print:
		 *		(1) single result (for calculating Wasserstein distance to reference)
		 *		(2) time in microseconds (benchmarking setup expects cpu time in microseconds)
		 */
		printf("%lf %" PRIu64 "\n", output, (uint64_t)(cpuTimeInSeconds*1000000));
	}
	else
	{
		/*
		 *	Print the results (either in JSON or standard output format)
		 */
		if (!arguments.common.isOutputJSONMode)
		{
			if (arguments.common.outputSelect == kOutputVariableIndexMax)
			{
				for (size_t i = 0; i < kOutputVariableIndexMax; i++)
				{
					printf("%s: %.2lf\n", outputVariableNames[i], outputVariables[i]);
				}
			}
			else
			{
					printf("%s: %.2lf\n", outputVariableNames[arguments.common.outputSelect], outputVariables[arguments.common.outputSelect]);
			}
		}
		else
		{
			printJSONFormattedOutput(&arguments, monteCarloOutputSamples, outputVariables, outputVariableNames);
		}

		/*
		 *	Print timing result.
		 */
		if (arguments.common.isTimingEnabled)
		{
			printf("\nCPU time used: %lf seconds\n", cpuTimeInSeconds);
		}

		/*
		 *	Write output data.
		 */
		if (arguments.common.isWriteToFileEnabled)
		{
			if (writeOutputDoubleDistributionsToCSV(
				arguments.common.outputFilePath,
				outputVariables,
				outputVariableNames,
				kOutputVariableIndexMax))
			{
				return kCommonConstantReturnTypeError;
			}
		}
	}

	/*
	 *	Save Monte carlo outputs in an output file.
	 *	Free dynamically-allocated memory.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		if (arguments.common.outputSelect == kOutputVariableIndexValueAtRisk)
		{
			saveMonteCarloDoubleDataToDataDotOutFile(monteCarloOutputSamples, (uint64_t)(cpuTimeInSeconds*1000000), 1);
		}
		else
		{
			saveMonteCarloDoubleDataToDataDotOutFile(monteCarloOutputSamples, (uint64_t)(cpuTimeInSeconds*1000000), arguments.common.numberOfMonteCarloIterations);
		}
	}
	free(monteCarloOutputSamples);

	return 0;
}
