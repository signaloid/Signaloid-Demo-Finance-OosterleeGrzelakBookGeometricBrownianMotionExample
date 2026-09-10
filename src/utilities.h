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

typedef enum
{
	kOutputVariableIndexStockPriceAtMaturity    = 0,
	kOutputVariableIndexCallOptionPrice         = 1,
	kOutputVariableIndexPutOptionPrice          = 2,
	kOutputVariableIndexValueAtRisk             = 3,
	kOutputVariableIndexSimulatedReturns        = 4,
	kOutputVariableIndexMax,
} OutputVariableIndex;

typedef enum
{
	kGeometricBrownianMotionConfigFrequencyIndexDays    = 0,
	kGeometricBrownianMotionConfigFrequencyIndexMonths  = 1,
	kGeometricBrownianMotionConfigFrequencyIndexYears   = 2,
	kGeometricBrownianMotionConfigFrequencyIndexMax,
} kGeometricBrownianMotionConfigFrequencyIndex;


typedef enum
{
	kGeometricBrownianMotionConfigFrequencyDays     = 252,
	kGeometricBrownianMotionConfigFrequencyMonths   = 12,
	kGeometricBrownianMotionConfigFrequencyYears    = 1,
} kGeometricBrownianMotionConfigFrequency;

#define kGeometricBrownianMotionConfigDefaultRiskFreeRate           (0.05)
#define kGeometricBrownianMotionConfigDefaultInitialPortfolioValue  (100.0)
#define kGeometricBrownianMotionConfigDefaultPeriodicVolatility     (0.4)
#define kGeometricBrownianMotionConfigDefaultStrikePrice            (90.0)
#define kGeometricBrownianMotionConfigDefaultQuantileProbability    (0.05)
#define kGeometricBrownianMotionConfigDefaultRiskFreeInterestRate   (0.05)
#define kGeometricBrownianMotionConfigDefaultMaturityTime           (1.0)
#define kGeometricBrownianMotionConfigDefaultStartDate              (0.0)

/*
 *	Maturity time used by the no-OS build, where command-line arguments are not
 *	available. Expressed in trading days and converted to the years unit that
 *	`CommandLineArguments.maturityTime` carries, using the same 252-trading-day
 *	year as `kGeometricBrownianMotionConfigFrequencyDays`.
 */
#define kGeometricBrownianMotionConfigNoOSMaturityTimeInDays (2.0)
#define kGeometricBrownianMotionConfigNoOSMaturityTime          \
		(kGeometricBrownianMotionConfigNoOSMaturityTimeInDays / \
		 (double) kGeometricBrownianMotionConfigFrequencyDays)

typedef struct CommandLineArguments
{
	CommonCommandLineArguments  common;
	double                      riskFreeRate;
	unsigned int                frequencyIndex;
	double                      initialPortfolioValue;
	double                      periodicVolatility;
	double                      strikePrice;
	double                      quantileProbability;
	double                      maturityTime;
} CommandLineArguments;

/**
 *	@brief	Print out command-line usage.
 */
void
printUsage(void);

/**
 *	@brief	Get command-line arguments.
 *
 *		In the no-OS build there is no command line: `argc` and `argv` are
 *		ignored and the hard-coded configuration set by
 *		`setNoOSCommandLineArguments()` is used instead.
 *
 *	@param	argc		: argument count from `main()`.
 *	@param	argv		: argument vector from `main()`.
 *	@param	arguments	: Pointer to struct to store arguments.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful,
 *					else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments);

#ifdef NO_OS_AVAILABLE

/**
 *	@brief	Set the hard-coded command-line arguments used by the no-OS build.
 *
 *		This is the single place to change the configuration that no-OS
 *		runs use, since those runs cannot be given command-line arguments.
 *		Fields not set here keep the values from
 *		`setDefaultCommandLineArguments()`.
 *
 *	@param	arguments	: command-line arguments pointer.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful,
 *					else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
setNoOSCommandLineArguments(CommandLineArguments * arguments);
#endif

/**
 *	@brief	Set the default values for the command-line arguments.
 *
 *	@param	arguments	: command-line arguments pointer.
 */
CommonConstantReturnType
setDefaultCommandLineArguments(CommandLineArguments * arguments);
