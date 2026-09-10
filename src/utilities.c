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
#include "utilities.h"
#include <uxhw.h>

void
printUsage(void)
{
	fprintf(stderr, "Example: Geometric Brownian Motion Application Use Case - Signaloid version\n");
	fprintf(stderr, "Usage: Valid command-line arguments are:\n");
	fprintf(
		stderr,
		"\t[-o, --output <Path to output CSV file : str>] (Specify the output file.)\n"
		"\t[-S, --select-output <output : int (Default: %d)>] (Compute 0-indexed output. Calculate all possible outputs if equal to %d.)\n"
		"\t[-M, --multiple-executions <Number of executions : int (Default: 1)>] (Repeated execute kernel for benchmarking.)\n"
		"\t[-T, --time] (Timing mode: Times and prints the timing of the kernel execution.)\n"
		"\t[-j, --json] (Print output in JSON format.)\n"
		"\t[-h, --help] (Display this help message.)\n"
		"\t[--rate, --risk-free-rate <rate : double (Default: %lf)>] (Risk-free rate.)\n"
		"\t[--frequency, --frequency-index <frequency : int (Default: %d)>] (Frequency index.)\n"
		"\t[--initial-value, --initial-portfolio-value <initial value : double (Default: %lf)>] (Initial stock price.)\n"
		"\t[--volatility, --periodic-volatility <volatility : double (Default: %lf)>] (Periodic volatility parameter.)\n"
		"\t[--strike, --strike-price <strike : double (Default: %lf)>] (Strike price for options.)\n"
		"\t[--quantile, --quantile-probability <quantile : double (Default: %lf)>] (Quantile probability for Value at Risk.)\n"
		"\t[--maturity-time, --maturity-time-years <time : double (Default: %lf)>] (Maturity time in years.)\n",
		kOutputVariableIndexMax,
		kOutputVariableIndexMax,
		kGeometricBrownianMotionConfigDefaultRiskFreeRate,
		kGeometricBrownianMotionConfigFrequencyIndexDays,
		kGeometricBrownianMotionConfigDefaultInitialPortfolioValue,
		kGeometricBrownianMotionConfigDefaultPeriodicVolatility,
		kGeometricBrownianMotionConfigDefaultStrikePrice,
		kGeometricBrownianMotionConfigDefaultQuantileProbability,
		kGeometricBrownianMotionConfigDefaultMaturityTime
	);
	fprintf(stderr, "\n");

	return;
}

/**
 *	@brief	Set the default values for the command-line arguments.
 *
 *	@param	arguments	: command-line arguments pointer.
 */
CommonConstantReturnType
setDefaultCommandLineArguments(CommandLineArguments * arguments)
{
	/*
	 *	Older GCC versions have a bug which gives a spurious warning for
	 *	the C universal zero initializer `{0}`. Any workaround makes the
	 *	code less portable or prevents the common code from adding new
	 *	fields to the `CommonCommandLineArguments` struct. Therefore, we
	 *	surpress this warning.
	 *
	 *	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119.
	 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

	*arguments = (CommandLineArguments)
	{
		.common                 = (CommonCommandLineArguments) { 0 },
		.riskFreeRate           = kGeometricBrownianMotionConfigDefaultRiskFreeRate,
		.frequencyIndex         = kGeometricBrownianMotionConfigFrequencyIndexDays,
		.initialPortfolioValue  = kGeometricBrownianMotionConfigDefaultInitialPortfolioValue,
		.periodicVolatility     = kGeometricBrownianMotionConfigDefaultPeriodicVolatility,
		.strikePrice            = kGeometricBrownianMotionConfigDefaultStrikePrice,
		.quantileProbability    = kGeometricBrownianMotionConfigDefaultQuantileProbability,
		.maturityTime           = kGeometricBrownianMotionConfigDefaultMaturityTime
	};
#pragma GCC diagnostic pop

	return kCommonConstantReturnTypeSuccess;
}

#ifdef NO_OS_AVAILABLE
CommonConstantReturnType
setNoOSCommandLineArguments(CommandLineArguments * arguments)
{
	if (arguments == NULL)
	{
		fputs("Error: The provided pointer to arguments is NULL.\n", stderr);

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Start from the defaults so that every demo-specific field is
	 *	initialized, then override the ones the no-OS build fixes. The
	 *	`common` sub-struct is zeroed by this call, so it is set explicitly
	 *	below.
	 */
	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	arguments->frequencyIndex                       = kGeometricBrownianMotionConfigFrequencyIndexDays;
	arguments->initialPortfolioValue                = kGeometricBrownianMotionConfigDefaultInitialPortfolioValue;
	arguments->quantileProbability                  = kGeometricBrownianMotionConfigDefaultQuantileProbability;
	arguments->maturityTime                         = kGeometricBrownianMotionConfigNoOSMaturityTime;
	arguments->common.numberOfMonteCarloIterations  = 1;
	arguments->common.outputSelect                  = kOutputVariableIndexMax;
	arguments->common.isTimingEnabled               = false;
	arguments->common.isMonteCarloMode              = false;
	arguments->common.isOutputJSONMode              = false;
	arguments->common.isWriteToFileEnabled          = false;

	return kCommonConstantReturnTypeSuccess;
}
#endif

CommonConstantReturnType
getCommandLineArguments(
	int                     argc,
	char *                  argv[],
	CommandLineArguments *  arguments)
{
#ifdef NO_OS_AVAILABLE
	/*
	 *	The no-OS build has no command line to parse, so ignore `argc` and
	 *	`argv` and use the hard-coded configuration instead.
	 */
	(void) argc;
	(void) argv;

	puts("Using hard coded command line arguments");

	return setNoOSCommandLineArguments(arguments);

#else
	const char *    riskFreeRateArg             = NULL;
	const char *    frequencyIndexArg           = NULL;
	const char *    initialPortfolioValueArg    = NULL;
	const char *    periodicVolatilityArg       = NULL;
	const char *    strikePriceArg              = NULL;
	const char *    quantileProbabilityArg      = NULL;
	const char *    maturityTimeArg             = NULL;

	if (arguments == NULL)
	{
		fprintf(stderr, "Error: The provided pointer to arguments is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	DemoOption demoSpecificOptions[] = {
		{ .opt = "rate",          .optAlternative = "risk-free-rate",          .hasArg = true, .foundArg = &riskFreeRateArg,          .foundOpt = NULL },
		{ .opt = "frequency",     .optAlternative = "frequency-index",         .hasArg = true, .foundArg = &frequencyIndexArg,        .foundOpt = NULL },
		{ .opt = "initial-value", .optAlternative = "initial-portfolio-value", .hasArg = true, .foundArg = &initialPortfolioValueArg, .foundOpt = NULL },
		{ .opt = "volatility",    .optAlternative = "periodic-volatility",     .hasArg = true, .foundArg = &periodicVolatilityArg,    .foundOpt = NULL },
		{ .opt = "strike",        .optAlternative = "strike-price",            .hasArg = true, .foundArg = &strikePriceArg,           .foundOpt = NULL },
		{ .opt = "quantile",      .optAlternative = "quantile-probability",    .hasArg = true, .foundArg = &quantileProbabilityArg,   .foundOpt = NULL },
		{ .opt = "maturity-time", .optAlternative = "maturity-time-years",     .hasArg = true, .foundArg = &maturityTimeArg,          .foundOpt = NULL },
		{ 0 },
	};

	if (parseArgs(argc, argv, &arguments->common, demoSpecificOptions) != kCommonConstantReturnTypeSuccess)
	{
		fprintf(stderr, "Parsing command line arguments failed\n");
		printUsage();

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Process command-line arguments
	 */
	if (arguments->common.isHelpEnabled)
	{
		printUsage();

		exit(EXIT_SUCCESS);
	}

	if (arguments->common.isInputFromFileEnabled)
	{
		fprintf(stderr, "Reading inputs from CSV file is not currently supported\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	Write to output file is not supported in MonteCarlo Mode.
	 */
	if (arguments->common.isWriteToFileEnabled && arguments->common.isMonteCarloMode)
	{
		fprintf(stderr, "Writing to output file is not supported in MonteCarlo Mode.\n");

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isVerbose)
	{
		fprintf(stderr, "Warning: Verbose mode not supported. Continuing in non-verbose mode.\n");
	}

	/*
	 *	If no output selected from CLA, set the print all value as default.
	 */
	if (!arguments->common.isOutputSelected)
	{
		arguments->common.outputSelect = kOutputVariableIndexMax;
	}

	/*
	 *	If a single output is selected, we must be in Monte Carlo mode.
	 */
	if (arguments->common.outputSelect > kOutputVariableIndexMax)
	{
		fprintf(
			stderr,
			"Output select value (-S option) is greater than the possible number of outputs: Provided %zd. Max: %d\n",
			arguments->common.outputSelect,
			kOutputVariableIndexMax
		);

		return kCommonConstantReturnTypeError;
	}
	/*
	 *	When all outputs are selected, we cannot be in Monte Carlo mode.
	 */
	else if (arguments->common.outputSelect == kOutputVariableIndexMax)
	{
		if (arguments->common.isMonteCarloMode)
		{
			fprintf(stderr, "Error: Please select a single output when in Monte Carlo mode.\n");

			return kCommonConstantReturnTypeError;
		}
	}

	/*
	 * Process the demo-specific command-line arguments
	 */

	if (riskFreeRateArg != NULL)
	{
		double riskFreeRate;

		if (parseDoubleChecked(riskFreeRateArg, &riskFreeRate) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The risk-free rate parameter (--rate, --risk-free-rate) must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
		arguments->riskFreeRate = riskFreeRate;
	}

	if (frequencyIndexArg != NULL)
	{
		int frequencyIndex;

		if (parseIntChecked(frequencyIndexArg, &frequencyIndex) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The frequency index (--frequency, --frequency-index) must be an integer.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (frequencyIndex < 0 || frequencyIndex >= kGeometricBrownianMotionConfigFrequencyIndexMax)
		{
			fprintf(stderr, "Error: The frequency index (--frequency, --frequency-index) must be 0 (days), 1 (months) or 2 (years).\n");

			return kCommonConstantReturnTypeError;
		}
		arguments->frequencyIndex = frequencyIndex;
	}

	if (initialPortfolioValueArg != NULL)
	{
		double initialPortfolioValue;

		if (parseDoubleChecked(initialPortfolioValueArg, &initialPortfolioValue) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The initial portfolio value (--initial-value, --initial-portfolio-value) must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
		arguments->initialPortfolioValue = initialPortfolioValue;
	}

	if (periodicVolatilityArg != NULL)
	{
		double periodicVolatility;

		if (parseDoubleChecked(periodicVolatilityArg, &periodicVolatility) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The periodic volatility parameter (--volatility, --periodic-volatility) must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
		arguments->periodicVolatility = periodicVolatility;
	}

	if (strikePriceArg != NULL)
	{
		double strikePrice;

		if (parseDoubleChecked(strikePriceArg, &strikePrice) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The strike price parameter (--strike, --strike-price) must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
		arguments->strikePrice = strikePrice;
	}

	if (quantileProbabilityArg != NULL)
	{
		double quantileProbability;

		if (parseDoubleChecked(quantileProbabilityArg, &quantileProbability) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The quantile probability parameter (--quantile, --quantile-probability) must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
		arguments->quantileProbability = quantileProbability;
	}

	if (maturityTimeArg != NULL)
	{
		double maturityTime;

		if (parseDoubleChecked(maturityTimeArg, &maturityTime) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The maturity time parameter (--maturity-time, --maturity-time-years) must be a real number.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}
		arguments->maturityTime = maturityTime;
	}

	return kCommonConstantReturnTypeSuccess;

#endif
}
