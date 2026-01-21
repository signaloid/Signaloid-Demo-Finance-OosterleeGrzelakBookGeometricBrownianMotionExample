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
		"\t[-b, --benchmarking] (Benchmarking mode: Generate outputs in format for benchmarking.)\n"
		"\t[-j, --json] (Print output in JSON format.)\n"
		"\t[-h, --help] (Display this help message.)\n"
		"\t[--mean-return, --periodic-mean-return <mean-return : double (Default: %lf)>] (Periodic mean return.)\n"
		"\t[--frequency, --frequency-index <frequency : int (Default: %d)>] (Frequency index.)\n"
		"\t[--initial-value, --initial-stock-price-value <initial value : double (Default: %lf)>] (Initial stock price.)\n"
		"\t[--volatility, --periodic-volatility <volatility : double (Default: %lf)>] (Peridoic volatility parameter.)\n"
		"\t[--strike, --strike-price <strike : double (Default: %lf)>] (Strike price for options.)\n"
		"\t[--quantile, --quantile-probability <quantile : double (Default: %lf)>] (Quantile probability for Value at Risk.)\n"
		"\t[--maturity-time, --maturity-time <time : double (Default: %lf)>] (Maturity time in years.)\n",
		kOutputVariableIndexMax,
		kOutputVariableIndexMax,
		kGeometricBrownianMotionConfigDefaultPeriodicMeanReturn,
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
setDefaultCommandLineArguments(CommandLineArguments *  arguments)
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
		.common	  		= (CommonCommandLineArguments) {0},
		.periodicMeanReturn	= kGeometricBrownianMotionConfigDefaultPeriodicMeanReturn,
		.frequencyIndex		= kGeometricBrownianMotionConfigFrequencyIndexDays,
		.initialPortfolioValue	= kGeometricBrownianMotionConfigDefaultInitialPortfolioValue,
		.periodicVolatility	= kGeometricBrownianMotionConfigDefaultPeriodicVolatility,
		.strikePrice	  	= kGeometricBrownianMotionConfigDefaultStrikePrice,
		.quantileProbability 	= kGeometricBrownianMotionConfigDefaultQuantileProbability,
		.maturityTime		= kGeometricBrownianMotionConfigDefaultMaturityTime
	};
#pragma GCC diagnostic pop

	return kCommonConstantReturnTypeSuccess;
}

CommonConstantReturnType
getCommandLineArguments(
	int			argc,
	char *			argv[],
	CommandLineArguments *	arguments)
{
	const char *	periodicMeanReturnArg		= NULL;
	const char * 	frequencyIndexArg		= NULL;
	const char * 	initialPortfolioValueArg	= NULL;
	const char * 	periodicVolatilityArg 		= NULL;
	const char *	StrikePriceArg 			= NULL;
	const char *	QuantileProbabilityArg 		= NULL;
	const char *	maturityTimeArg 		= NULL;

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
		{ .opt = "mean-return",		.optAlternative = "periodic-mean-return", 	.hasArg = true, .foundArg = &periodicMeanReturnArg,	.foundOpt = NULL },
		{ .opt = "frequency",		.optAlternative = "frequency-index", 		.hasArg = true, .foundArg = &frequencyIndexArg,		.foundOpt = NULL },
		{ .opt = "initial-value", 	.optAlternative = "initial-portfolio-value", 	.hasArg = true, .foundArg = &initialPortfolioValueArg,	.foundOpt = NULL },
		{ .opt = "volatility", 		.optAlternative = "periodic-volatility", 	.hasArg = true, .foundArg = &periodicVolatilityArg,	.foundOpt = NULL },
		{ .opt = "strike", 		.optAlternative = "strike-price", 		.hasArg = true, .foundArg = &StrikePriceArg,		.foundOpt = NULL },
		{ .opt = "quantile", 		.optAlternative = "quantile-probability",	.hasArg = true, .foundArg = &QuantileProbabilityArg,	.foundOpt = NULL },
		{ .opt = "maturity-time", 	.optAlternative = "maturity-time-years", 	.hasArg = true, .foundArg = &maturityTimeArg,		.foundOpt = NULL },
		{0},
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
	 *	If a single output is selected, we must be in benchmarking mode or Monte Carlo mode.
	 */
	if (arguments->common.outputSelect > kOutputVariableIndexMax)
	{
		fprintf(
			stderr,
			"Output select value (-S option) is greater than the possible number of outputs: Provided %zd. Max: %d\n",
			arguments->common.outputSelect,
			kOutputVariableIndexMax);

		return kCommonConstantReturnTypeError;
	}
	/*
	 *	When all outputs are selected, we cannot be in benchmarking mode or Monte Carlo mode.
	 */
	else if (arguments->common.outputSelect == kOutputVariableIndexMax)
	{
		if ((arguments->common.isBenchmarkingMode) || (arguments->common.isMonteCarloMode))
		{
			fprintf(stderr, "Error: Please select a single output when in benchmarking mode or Monte Carlo mode.\n");

			return kCommonConstantReturnTypeError;
		}
	}

	/*
	* Process the demo-specific command-line arguments
	*/

	if (periodicMeanReturnArg != NULL)
	{
		double periodicMeanReturn;
		if (parseDoubleChecked(periodicMeanReturnArg, &periodicMeanReturn) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The periodic mean return rate parameter (--rate) must be a real number.\n");
			printUsage();
			return kCommonConstantReturnTypeError;
		}
		arguments->periodicMeanReturn = periodicMeanReturn;
	}

	if (frequencyIndexArg != NULL)
	{
		int frequencyIndex;
		if (parseIntChecked(frequencyIndexArg, &frequencyIndex) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The frequencyIndex index must be an integer.\n");
			printUsage();
			return kCommonConstantReturnTypeError;
		}
		if (arguments->frequencyIndex > kGeometricBrownianMotionConfigFrequencyIndexMax)
		{
			fprintf(stderr, "Error: frequencyIndex index must be 0 (days), 1 (months) or 2 (years).\n");

			return kCommonConstantReturnTypeError;
		}
		arguments->frequencyIndex = frequencyIndex;
	}

	if (initialPortfolioValueArg != NULL)
	{
		double initialPortfolioValue;
		if (parseDoubleChecked(initialPortfolioValueArg, &initialPortfolioValue) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The initial portfolio value (--init) must be a real number.\n");
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
			fprintf(stderr, "Error: The periodic volatility parameter (--sigma) must be a real number.\n");
			printUsage();
			return kCommonConstantReturnTypeError;
		}
		arguments->periodicVolatility = periodicVolatility;
	}

	if (StrikePriceArg != NULL)
	{
		double strikePrice;
		if (parseDoubleChecked(StrikePriceArg, &strikePrice) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The strike price parameter (--strike) must be a real number.\n");
			printUsage();
			return kCommonConstantReturnTypeError;
		}
		arguments->strikePrice = strikePrice;
	}

	if (QuantileProbabilityArg != NULL)
	{
		double quantileProbability;
		if (parseDoubleChecked(QuantileProbabilityArg, &quantileProbability) != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The quantile probability parameter (--quantile) must be a real number.\n");
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
			fprintf(stderr, "Error: The maturity time parameter (--duration) must be a real number.\n");
			printUsage();
			return kCommonConstantReturnTypeError;
		}
		arguments->maturityTime = maturityTime;
	}

	return kCommonConstantReturnTypeSuccess;
}

void
populateJSONVariableStruct(
	JSONVariable *		jsonVariable,
	double *		outputVariableValues,
	const char *		outputVariableDescription,
	OutputVariableIndex	outputSelect,
	size_t			numberOfOutputVariableValues)
{
	snprintf(jsonVariable->variableSymbol, kCommonConstantMaxCharsPerJSONVariableSymbol, "outputDistributions[%u]", outputSelect);
	snprintf(jsonVariable->variableDescription, kCommonConstantMaxCharsPerJSONVariableDescription, "%s", outputVariableDescription);
	jsonVariable->values = (JSONVariablePointer){ .asDouble = outputVariableValues };
	jsonVariable->type = kJSONVariableTypeDouble;
	jsonVariable->size = numberOfOutputVariableValues;

	return;
}

void
printJSONFormattedOutput(
	CommandLineArguments *	arguments,
	double *		monteCarloOutputSamples,
	double *		outputDistributions,
	const char **		outputVariableDescriptions)
{
	JSONVariable			jsonVariables[kOutputVariableIndexMax];
	OutputVariableIndex		outputSelectLowerBound;
	OutputVariableIndex		outputSelectUpperBound;

	if (arguments->common.outputSelect == kOutputVariableIndexMax)
	{
		outputSelectLowerBound = (OutputVariableIndex)0;
		outputSelectUpperBound = kOutputVariableIndexMax;
	}
	else
	{
		outputSelectLowerBound = arguments->common.outputSelect;
		outputSelectUpperBound = outputSelectLowerBound + 1;
	}

	for (OutputVariableIndex outputSelect = outputSelectLowerBound; outputSelect < outputSelectUpperBound; outputSelect++)
	{
		/*
		 *	If in Monte Carlo mode, `pointerToOutputVariable` points to the beginning of the `monteCarloOutputSamples` array.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` is the length of the `monteCarloOutputSamples` array.
		 *	Else, it points to the entry of the `outputDistributions` to be used.
		 *	In this case, `arguments.common.numberOfMonteCarloIterations` equals 1.
		 */
		double *	pointerToOutputVariable = arguments->common.isMonteCarloMode ? monteCarloOutputSamples : &outputDistributions[outputSelect];

		populateJSONVariableStruct(
			&jsonVariables[outputSelect],
			pointerToOutputVariable,
			outputVariableDescriptions[outputSelect],
			outputSelect,
			arguments->common.numberOfMonteCarloIterations);
	}

	printJSONVariablesTemp(
		&jsonVariables[outputSelectLowerBound],
		outputSelectUpperBound - outputSelectLowerBound,
		"Oosterlee-Grzelak Book Geometric Brownian Motion");

	return;
}


void
printJSONVariablesTemp(JSONVariable *  jsonVariables, size_t count, const char *  description)
{
	/*
	 *	Print JSON outputs.
	 */
	printf("{\n");
	printf("\t\"description\": \"%s\",\n", description);
	printf("\t\"plots\": [\n");

	for (size_t i = 0; i < count; i++)
	{
		printf("\t\t{\n");

		/*
		 *	We include this property in the JSON for backwards compatibility.
		 */
		printf("\t\t\t\"variableID\": \"%s\",\n", jsonVariables[i].variableSymbol);
		printf("\t\t\t\"variableSymbol\": \"%s\",\n", jsonVariables[i].variableSymbol);
		printf("\t\t\t\"variableDescription\": \"%s\",\n", jsonVariables[i].variableDescription);
		printf("\t\t\t\"values\": [\n");
		for (size_t j = 0; j < jsonVariables[i].size; j++)
		{
			switch (jsonVariables[i].type)
			{
				case kJSONVariableTypeDouble:
				{
					printf("\t\t\t\t\"%f\"", jsonVariables[i].values.asDouble[j]);
					break;
				}
				case kJSONVariableTypeFloat:
				{
					printf("\t\t\t\t\"%f\"", jsonVariables[i].values.asFloat[j]);
					break;
				}
				case kJSONVariableTypeDoubleParticle:
				{
					printf("\t\t\t\t\"% " SignaloidParticleModifierTemp "f\"", jsonVariables[i].values.asDouble[j]);
					break;
				}
				case kJSONVariableTypeFloatParticle:
				{
					printf("\t\t\t\t\"% " SignaloidParticleModifierTemp "f\"", jsonVariables[i].values.asFloat[j]);
					break;
				}
				case kJSONVariableTypeUnknown:
				default:
				{
					fatal("kJSONVariableTypeUnknown must be specified");
				}

			}
			if (j < (jsonVariables[i].size - 1))
			{
				printf(", \n");
			}
			else
			{
				printf("\n");
			}
		}
		printf("\t\t\t],\n");

		printf("\t\t\t\"stdValues\": [\n");
		for (size_t j = 0; j < jsonVariables[i].size; j++)
		{
			switch (jsonVariables[i].type)
			{
				case kJSONVariableTypeDouble:
				{
					printf(
						"\t\t\t\t% " SignaloidParticleModifierTemp "f",
						UxHwDoubleNthMoment(jsonVariables[i].values.asDouble[j], 2));
					break;
				}
				case kJSONVariableTypeFloat:
				{
					printf(
						"\t\t\t\t% " SignaloidParticleModifierTemp "f",
						UxHwFloatNthMoment(jsonVariables[i].values.asFloat[j], 2));
					break;
				}
				case kJSONVariableTypeFloatParticle:
				case kJSONVariableTypeDoubleParticle:
				{
					printf("\t\t\t\t% " SignaloidParticleModifierTemp "f", 0.0);
					break;
				}
				case kJSONVariableTypeUnknown:
				default:
				{
					fatal("kJSONvariableTypeUnknown must be specified");
				}
			}
			if (j < (jsonVariables[i].size - 1))
			{
				printf(", \n");
			}
			else
			{
				printf("\n");
			}
		}
		printf("\t\t\t]\n");

		printf("\t\t}");
		if (i < count - 1)
		{
			printf(",");
		}
		printf("\n");
	}

	printf("\t]\n");
	printf("}\n");
}

