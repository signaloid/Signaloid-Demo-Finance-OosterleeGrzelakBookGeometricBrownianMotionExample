[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-dark-latest.png#gh-dark-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/signaloid/Signaloid-Demo-Finance-OosterleeGrzelakBookGeometricBrownianMotionExample#gh-dark-mode-only)
[<img src="https://assets.signaloid.io/add-to-signaloid-cloud-logo-light-latest.png#gh-light-mode-only" alt="[Add to signaloid.io]" height="30">](https://signaloid.io/repositories?connect=https://github.com/signaloid/Signaloid-Demo-Finance-OosterleeGrzelakBookGeometricBrownianMotionExample#gh-light-mode-only)

# Geometric Brownian Motion Application from Oosterlee-Grzelak Book

A financial simulation application implementing geometric Brownian motion (GBM) for stock price evolution, option pricing, and risk analysis.

## Overview

Modeling the price evolution of a financial instrument in the presence of fluctuating market conditions is a critical task for many financial institutions. This application implements a numerical solution of the stochastic differential equation (SDE) for a geometric Brownian motion process, based on the implementation from the Oosterlee-Grzelak Book[^1].

Traditional Monte Carlo simulations of GBM typically use thousands to hundreds of thousands of paths across multiple time steps (e.g., 252 trading days per year). When running on the Signaloid Cloud Compute Engine, the application can replace individual samples from distributions for each path with direct computation on probability distribution representations. This allows the Signaloid platform to compute the same distribution in a single pass through the time steps, achieving results equivalent to thousands of Monte Carlo iterations.

### Risk-Neutral Measure

This application models stock price dynamics under the **risk-neutral measure** (also known as the risk-free measure or Q-measure). Under this measure, the drift of the stock price process is the risk-free rate rather than the asset's expected return. This is the standard approach for pricing derivatives because, under the risk-neutral measure, the fair price of any derivative is the discounted expected payoff. The risk-free rate parameter (`--risk-free-rate`) represents this drift term in the GBM stochastic differential equation.

### Features

This application can calculate:
- **Stock price at maturity**: Distribution of asset prices after simulation period (e.g., 252 trading days)
- **European call option payoff**: Distribution of call option values
- **European put option payoff**: Distribution of put option values
- **Value at Risk (VaR)**: Risk metric at specified quantile (default: 5%)
- **Simulated returns**: Portfolio return distributions

## Getting Started

### Cloning the Repository

The correct way to clone this repository to get the submodules is:
```sh
git clone --recursive git@github.com:signaloid/Signaloid-Demo-Finance-OosterleeGrzelakBookGeometricBrownianMotionExample.git
```

If you forgot to clone with `--recursive` and end up with empty submodule directories, you can remedy this with:
```sh
git submodule update --init
```

## Running on Signaloid Cloud Developer Platform

To run this application on the [Signaloid Cloud Developer Platform](https://signaloid.io), you need a Signaloid account. You can sign up using [this link](https://get.signaloid.io).

Once you have an account, click the "add to signaloid.io" button at the top of this README to connect this repository to the platform and run the application.

## Running Locally

You can compile and run this application locally as a native Monte Carlo implementation using the GNU Scientific Library[^2] to generate samples for different input distributions.

### Prerequisites

The native build needs GNU Make, a C compiler, and the GNU Scientific Library (GSL).

On macOS, install the Xcode Command Line Tools (which provide `make` and the C
compiler) and then install GSL with [Homebrew](https://brew.sh):
```bash
xcode-select --install
brew install gsl
```

On Linux (Debian/Ubuntu):
```bash
sudo apt-get install -y build-essential libgsl-dev
```

The top-level `Makefile` detects the GSL install location automatically, covering
Homebrew on Apple Silicon (`/opt/homebrew`), Homebrew on Intel (`/usr/local`) and
MacPorts (`/opt/local`), so no further configuration is needed on macOS.

### Compilation

From the repository root:
```bash
make local-build
```

This builds the native Monte Carlo executable `demo-native-mc` at the repository root, against the UxHw compatibility shim in `submodules/compat`.

### Execution

Run with Monte Carlo mode (required for local execution):
```bash
./demo-native-mc -S 0 -M 10000
```

This runs 10,000 Monte Carlo iterations to calculate the stock price at maturity. The results are stored in `data.out` where the first line contains execution time in microseconds (μs), and subsequent lines contain output sample values.

View the results:
```bash
cat data.out
```

## Command-Line Options

### Common Options

| Short | Long | Type | Default | Description |
|-------|------|------|---------|-------------|
| `-o` | `--output` | string | - | Path to output CSV file |
| `-S` | `--select-output` | int | 5 | Select specific output (0-5): 0=stock price, 1=call option, 2=put option, 3=VaR, 4=simulated returns, 5=all outputs |
| `-M` | `--multiple-executions` | int | 1 | Number of Monte Carlo iterations (only needed for local execution) |
| `-T` | `--time` | flag | false | Enable timing mode |
| `-b` | `--benchmarking` | flag | false | Enable benchmarking output format |
| `-j` | `--json` | flag | false | Output results in JSON format |
| `-h` | `--help` | flag | false | Display help message |

### Financial Parameters

| Long Option | Type | Default | Description |
|------------|------|---------|-------------|
| `--rate`, `--risk-free-rate` | double | 0.05 | Risk free rate (annualized) |
| `--initial-value`, `--initial-portfolio-value` | double | 100.0 | Initial portfolio/stock value |
| `--volatility`, `--periodic-volatility` | double | 0.4 | Periodic volatility (annualized) |
| `--strike`, `--strike-price` | double | 90.0 | Strike price for options |
| `--quantile`, `--quantile-probability` | double | 0.05 | Quantile probability for Value at Risk |
| `--maturity-time`, `--maturity-time-years` | double | 1.0 | Simulation time horizon in years |
| `--frequency`, `--frequency-index` | int | 0 | Time step frequency: 0=daily (252 steps/year), 1=monthly (12 steps/year), 2=yearly (1 step/year) |

**Note**: When running on Signaloid's platform, you don't need to set `-M` greater than 1. The platform's uncertainty tracking provides distribution results in a single execution.

## Output Types

Use the `-S` or `--select-output` option to choose which output to calculate:

### `-S 0`: Stock Price at Maturity

Calculates the distribution of asset prices after the simulation period (default: 252 trading days for 1 year).

Example output using Signaloid's C0Pro-S core:

![Price At Maturity example output plot](./docs/plots/outputDistributions[0]-C0-S.png)

### `-S 1`: Call Option Payoff

Calculates the European call option payoff distribution: `max(S - K, 0)` where S is the stock price at maturity and K is the strike price.

Example output using Signaloid's C0Pro-S core:

![Call option example output plot](./docs/plots/outputDistributions[1]-C0-S.png)

### `-S 2`: Put Option Payoff

Calculates the European put option payoff distribution: `max(K - S, 0)` where K is the strike price and S is the stock price at maturity.

Example output using Signaloid's C0Pro-S core:

![Put option example output plot](./docs/plots/outputDistributions[2]-C0-S.png)

### `-S 3`: Value at Risk (VaR)

Calculates the Value at Risk at the specified quantile (default: 5%). VaR represents the maximum expected loss over the time horizon at a given confidence level.

### `-S 4`: Simulated Returns

Calculates the portfolio returns: final portfolio value minus initial value.

### `-S 5`: All Outputs (Default)

Calculates all available outputs: stock price at maturity, call option payoff, put option payoff, Value at Risk, and simulated returns.


---

## References

[^1]: Oosterlee, C.W. and Grzelak, L.A. 2019. Mathematical Modeling And Computation In Finance: With Exercises And Python And Matlab Computer Codes. World Scientific Publishing Company. https://books.google.com/books?id=TsPKDwAAQBAJ - [Python Implementation](https://github.com/LechGrzelak/QuantFinanceBook/blob/master/PythonCodes/Chapter%2014/Fig14_07.py#L32)

[^2]: [GNU Scientific Library](https://www.gnu.org/software/gsl/)

