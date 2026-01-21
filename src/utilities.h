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

#include "common.h"
#define SignaloidParticleModifierTemp	"P"

typedef enum
{
	kOutputVariableIndexStockPriceAtMaturity		= 0,
	kOutputVariableIndexCallOptionPrice			= 1,
	kOutputVariableIndexPutOptionPrice			= 2,
	kOutputVariableIndexValueAtRisk				= 3,
	kOutputVariableIndexSimulatedReturns			= 4,
	kOutputVariableIndexMax,
} OutputVariableIndex;

typedef enum
{
	kGeometricBrownianMotionConfigFrequencyIndexDays 	= 0,
	kGeometricBrownianMotionConfigFrequencyIndexMonths	= 1,
	kGeometricBrownianMotionConfigFrequencyIndexYears	= 2,
	kGeometricBrownianMotionConfigFrequencyIndexMax,
} kGeometricBrownianMotionConfigFrequencyIndex;


typedef enum
{
	kGeometricBrownianMotionConfigFrequencyDays 	= 252,
	kGeometricBrownianMotionConfigFrequencyMonths	= 12,
	kGeometricBrownianMotionConfigFrequencyYears	= 1,
} kGeometricBrownianMotionConfigFrequency;

typedef enum
{
	kGeometricBrownianMotionConfigDefaultNumberOfPaths	= 25,
} GeometricBrownianMotionConfigDefault;

#define kGeometricBrownianMotionConfigDefaultPeriodicMeanReturn		(0.05)
#define kGeometricBrownianMotionConfigDefaultInitialPortfolioValue	(100.0)
#define kGeometricBrownianMotionConfigDefaultPeriodicVolatility		(0.4)
#define kGeometricBrownianMotionConfigDefaultStrikePrice		(90.0)
#define kGeometricBrownianMotionConfigDefaultQuantileProbability	(0.05)
#define kGeometricBrownianMotionConfigDefaultRiskFreeInterestRate	(0.05)
#define kGeometricBrownianMotionConfigDefaultMaturityTime		(1.0)
#define kGeometricBrownianMotionConfigDefaultStartDate			(0.0)

typedef struct CommandLineArguments
{
	CommonCommandLineArguments	common;
	double				periodicMeanReturn;
	unsigned int			frequencyIndex;
	double				initialPortfolioValue;
	double				periodicVolatility;
	double				strikePrice;
	double				quantileProbability;
	double				maturityTime;
} CommandLineArguments;

/**
 *	@brief	Print out command-line usage.
 */
void	printUsage(void);

/**
 *	@brief	Get command-line arguments.
 *
 *	@param	argc		: argument count from `main()`.
 *	@param	argv		: argument vector from `main()`.
 *	@param	arguments	: Pointer to struct to store arguments.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful,
 *					else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments);

/**
 * 	@brief  Populates a JSONVariable struct
 *
 *	@param  jsonVariable				: Pointer to the `JSONVariable` struct to modify.
 *	@param  outputVariableValues			: The array of values for the output variable from which the JSON struct will take values.
 *	@param  outputVariableDescription		: The array of descriptions of output variables from which the JSON struct will take descriptions.
 *	@param  outputSelect				: An index to the `outputDistributions`. Chooses which value will be selected.
 *	@param  numberOfOutputVariableValues		: The number of values in `outputVariableValues`.
 */
void	populateJSONVariableStruct(
		JSONVariable *		jsonVariable,
		double *		outputVariableValues,
		const char *		outputVariableDescription,
		OutputVariableIndex	outputSelect,
		size_t			numberOfOutputVariableValues);

/**
 *	@brief  Prints output distributions in JSON format. Based on command-line arguments will either print
 *		a single value or all values stored in `outputDistributions`.
 *
 *	@param  arguments			: The command-line arguments, specifying which outputs will be printed.
 *	@param  monteCarloOutputSamples		: The array of data samples of Monte Carlo.
 *	@param  outputDistributions 		: The array that stores the distributions to be printed.
 *	@param  outputVariableDescriptions	: An array of strings containing the descriptions of the variables to be printed.
 */
void	printJSONFormattedOutput(
		CommandLineArguments *	arguments,
		double *		monteCarloOutputSamples,
		double *		outputDistributions,
		const char **		outputVariableDescriptions);


/**
 *	@brief	Print Ux-valued data to `stdout` in JSON format.
 *
 *	@param	jsonVariables	array of JSONVariable items
 *	@param	count		number of JSONVariable items
 *	@param	description	JSON description
 */
void
printJSONVariablesTemp(
	JSONVariable *	jsonVariables,
	size_t		count,
	const char *	description);

