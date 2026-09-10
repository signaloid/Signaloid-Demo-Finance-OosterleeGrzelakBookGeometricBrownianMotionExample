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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <uxhw.h>
#include "utilities.h"
#include "kernel.h"
#include "common.h"

#ifdef NO_OS_AVAILABLE

void
returnZeroNoOS(void);
#endif

int
main(int argc, char *  argv[])
{
	CommandLineArguments        arguments;
	double                      output;
	double *                    monteCarloOutputSamples = NULL;
	clock_t                     start;
	clock_t                     end;
	double                      cpuTimeInSeconds = 0.0;
	double                      outputVariables[kOutputVariableIndexMax];
	const char *                applicationDescription = "Oosterlee-Grzelak Book Geometric Brownian Motion";
	const char *                outputVariableNames[kOutputVariableIndexMax] = {
		[kOutputVariableIndexStockPriceAtMaturity]  = "Stock price at maturity",
		[kOutputVariableIndexCallOptionPrice]       = "Call option payoff",
		[kOutputVariableIndexPutOptionPrice]        = "Put option payoff",
		[kOutputVariableIndexValueAtRisk]           = "Value at Risk",
		[kOutputVariableIndexSimulatedReturns]      = "Simulated Returns",
	};
	kOutputVariableTypeIndex    outputVariableTypes[kOutputVariableIndexMax] = {
		[kOutputVariableIndexStockPriceAtMaturity]  = kOutputVariableTypeDistribution,
		[kOutputVariableIndexCallOptionPrice]       = kOutputVariableTypeDistribution,
		[kOutputVariableIndexPutOptionPrice]        = kOutputVariableTypeDistribution,
		[kOutputVariableIndexValueAtRisk]           = kOutputVariableTypeScalar,
		[kOutputVariableIndexSimulatedReturns]      = kOutputVariableTypeDistribution,
	};
	MeanAndVariance             meanAndVariance;

	/*
	 *	Get command-line arguments. In the no-OS build there is no command
	 *	line, so this instead applies the hard-coded configuration in
	 *	`setNoOSCommandLineArguments()`.
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
			__LINE__
		);

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	/*
	 *	Dispatch to the mode-specific kernel. The Monte Carlo loop (when
	 *	applicable) lives inside `calculateOutputMonteCarlo`; UxHw mode runs a single
	 *	distributional path inside `calculateOutputUxHw`.
	 */
	bool isSelectedOutputScalar = (arguments.common.outputSelect != kOutputVariableIndexMax) &&
	                              (outputVariableTypes[arguments.common.outputSelect] == kOutputVariableTypeScalar);

	if (arguments.common.isMonteCarloMode)
	{
		output = calculateOutputMonteCarlo(&arguments, outputVariables, monteCarloOutputSamples);

		/*
		 *	If not doing UxHw version, then approximate the cost of the third phase of
		 *	Monte Carlo (post-processing), by calculating the mean and variance.
		 */
		if (!isSelectedOutputScalar)
		{
			meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(monteCarloOutputSamples, arguments.common.numberOfMonteCarloIterations);
			output          = outputVariables[arguments.common.outputSelect] = meanAndVariance.mean;
		}
	}
	else
	{
		output = calculateOutputUxHw(&arguments, outputVariables, monteCarloOutputSamples);
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end                 = clock();
		cpuTimeInSeconds    = ((double) (end - start)) / CLOCKS_PER_SEC;
	}

	CommonCommandLineArguments printArguments = arguments.common;

	if (arguments.common.isMonteCarloMode && isSelectedOutputScalar)
	{
		printArguments.isMonteCarloMode             = false;
		printArguments.numberOfMonteCarloIterations = 1;
	}

	/*
	 *	Print json outputs if in JSON output mode.
	 */
	if (arguments.common.isOutputJSONMode)
	{
		printJSONFormattedOutput(
			&printArguments,
			monteCarloOutputSamples,
			outputVariables,
			outputVariableNames,
			kOutputVariableIndexMax,
			applicationDescription
		);
	}
	else
	{
		printHumanConsumableOutput(
			&printArguments,
			kOutputVariableIndexMax,
			outputVariables,
			outputVariableNames,
			outputVariableNames,
			monteCarloOutputSamples
		);
	}

	/*
	 *	Print timing result.
	 */
	if (arguments.common.isTimingEnabled)
	{
		printf("\nCPU time used: %" SignaloidParticleModifier "lf seconds\n", cpuTimeInSeconds);
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
				kOutputVariableIndexMax
		))
		{
			return kCommonConstantReturnTypeError;
		}
	}

	/*
	 *	Save Monte carlo outputs in an output file.
	 *	Free dynamically-allocated memory.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		size_t samplesToSave = isSelectedOutputScalar
		                ? 1
		                : arguments.common.numberOfMonteCarloIterations;

		saveMonteCarloDoubleDataToDataDotOutFile(
			monteCarloOutputSamples,
			(uint64_t) (cpuTimeInSeconds * 1000000),
			samplesToSave
		);
	}
	free(monteCarloOutputSamples);
#ifdef NO_OS_AVAILABLE
	returnZeroNoOS();
#else

	return 0;

#endif
}
