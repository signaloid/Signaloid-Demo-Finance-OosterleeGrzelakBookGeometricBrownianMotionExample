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
#include <float.h>
#include <uxhw.h>
#include "geometric-brownian-motion-uxhw.h"
#include "kernel.h"

double
geometricBrownianMotionUxHw(
	size_t  numberOfSteps,
	double  periodicVolatility,
	double  maturityTime,
	double  riskFreeRate,
	double  initialPortfolioValue)
{
	return geometricBrownianMotionSinglePath(
		numberOfSteps,
		periodicVolatility,
		maturityTime,
		riskFreeRate,
		initialPortfolioValue
	);
}

double
callOptionPayoffUxHw(double stockPriceAtMaturity, double strikePrice, double zeroCouponValue)
{
	double  difference;
	double  truncated;
	double  strikeFraction;

	/*
	 *	We need to do two things here:
	 *	1.	Capture the part of the distribution that satisfies the strike price.
	 *		We achieve this by taking the part of the distribution with positive
	 *		support.
	 *	2.	Capture the fact that only a fraction of the paths satisfy the strike
	 *		price. We compute strikeFraction = UxHwDoubleProbabilityGT(difference, 0)
	 *		and either use that to make a mixture of strike-exceeding payoff and zero
	 *		payoff, or multiply the mean of the truncated distribution by that.
	 *		Note: Because of the current semantics of UxHwDoubleProbabilityGT(),
	 *		we have to make sure the distribution value is in the first argument.
	 *
	 *	Finally, multiply the mean payoff by the present value of a
	 *	zero-coupon bond (see An Introduction to Quantitative Finance, Blyth,
	 *	page 4, first paragraph just below Figure 1.1).
	 */
	difference      = stockPriceAtMaturity - strikePrice;
	strikeFraction  = UxHwDoubleProbabilityGT(difference, 0);

	/*
	 *	When no part of the distribution is in the money there is nothing to
	 *	truncate to: `UxHwDoubleLimitDistributionSupport()` would return an
	 *	empty distribution, whose particles are NaN, and a mixture weight that
	 *	rounds up to a single particle at small representation sizes would then
	 *	surface those NaNs in the output. Return a hard zero payoff instead.
	 *
	 *	Written as a negated `>` so that a NaN `strikeFraction` also returns
	 *	zero rather than falling through.
	 */
	if (!(strikeFraction > 0.0))
	{
		return 0.0;
	}

	truncated = UxHwDoubleLimitDistributionSupport(difference, 0, DBL_MAX);

	return UxHwDoubleMixture(truncated, 0, strikeFraction) * zeroCouponValue;
}

double
putOptionPayoffUxHw(double stockPriceAtMaturity, double strikePrice, double zeroCouponValue)
{
	double  difference;
	double  truncated;
	double  strikeFraction;

	/*
	 *	See `callOptionPayoffUxHw` for the rationale behind the distribution
	 *	truncation, mixture pattern, and zero-coupon discounting.
	 */
	difference      = strikePrice - stockPriceAtMaturity;
	strikeFraction  = UxHwDoubleProbabilityGT(difference, 0);

	/*
	 *	See `callOptionPayoffUxHw` for the rationale behind computing
	 *	`strikeFraction` first and returning a hard zero payoff when no part
	 *	of the distribution is in the money, instead of unconditionally
	 *	truncating to an empty distribution.
	 */
	if (!(strikeFraction > 0.0))
	{
		return 0.0;
	}

	truncated = UxHwDoubleLimitDistributionSupport(difference, 0, DBL_MAX);

	return UxHwDoubleMixture(truncated, 0, strikeFraction) * zeroCouponValue;
}

double
computeSimulatedReturnsUxHw(double stockPriceAtMaturity, double initialPortfolioValue)
{
	return stockPriceAtMaturity - initialPortfolioValue;
}

double
computeValueAtRiskUxHw(double simulatedReturns, double quantileProbability)
{
	return UxHwDoubleQuantile(simulatedReturns, quantileProbability);
}
