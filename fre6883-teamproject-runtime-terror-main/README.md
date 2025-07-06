# Earnings Release Analysis on Stock Price Movement

## Project Overview
This project analyzes the impact of earnings announcements on stock price movements. It categorizes stocks based on their earnings performance (Beat, Meet, or Miss expectations) and calculates Average Abnormal Returns (AAR) and Cumulative Average Abnormal Returns (CAAR) using the bootstrap methodology. The results are visualized using GNUPlot to help understand market reactions to earnings surprises.

## Team Members
- **Sachin Adlakha** (Net ID: sa9082)
- **Manush Shah** (Net ID: mns9943)
- **Rashil Shah** (Net ID: rs9186)
- **Parshva Maniar** (Net ID: pm3882)

_Note on collaboration: Our team worked collaboratively by sitting together in the library and dividing tasks. While commits may appear ambiguous, all team members contributed equally to the project through pair programming, code review, and testing. Some commits may have been made under a single user's name despite collaborative work._

## Directory Structure
```
fre6883-teamproject-runtime-terror/
├── src/                      # Source code
│   ├── Calculation.cpp       # Implementation of financial calculations
│   ├── Calculation.h         # Definition of calculation classes
│   ├── DataExtraction.h      # Data retrieval and parsing
│   ├── Display.cpp           # Display utilities
│   ├── Display.h             # Display function declarations
│   ├── Group.h               # Stock group management
│   ├── Makefile              # Build configuration
│   ├── Menu.cpp              # Main program with user interface
│   ├── Stocks.h              # Stock class definition
│   ├── gnuplot.h             # Plotting utilities
│   ├── utils.cpp             # Helper functions
│   └── utils.h               # Utility declarations
├── data/                     # Input data
│   ├── iShares-Russell-3000-ETF_fund.xlsx   # Russell 3000 ETF data
│   └── Russell3000EarningsAnnouncements.csv # Earnings announcement data
└── documentation/            # Project documentation
```

## Prerequisites
The following libraries and tools are required to build and run the project:

1. **C++ Compiler** (g++ with C++11 support)
2. **libcurl** for API requests
3. **pthread** for multi-threading
4. **GNUPlot** for visualization
5. **XQuartz** (on macOS) for displaying plots

### Installation Instructions

#### On macOS:
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required packages
brew install gcc
brew install curl
brew install gnuplot
brew install --cask xquartz

# After installing XQuartz, you need to log out and log back in
```

#### On Ubuntu/Debian:
```bash
sudo apt update
sudo apt install g++
sudo apt install libcurl4-openssl-dev
sudo apt install gnuplot
sudo apt install libpthread-stubs0-dev
```

## Building the Project
To build the project, navigate to the `src` directory and run:
```bash
cd src
make
```

This will create an executable named `EarningsAnalysis`.

## Running the Application
After building, run the application with:
```bash
./EarningsAnalysis
```

The main menu provides the following options:
```
 Main Menu 
==============================================
1. Enter N to fetch data and group stocks
2. Display details of one stock from a group
3. Calculate AAR and CAAR for each group
4. Display Plots
5. Exit
Please enter your choice (1-5):
```

## Code Components

### Stocks.h
Defines the `Stocks` class that represents individual stock data with properties like:
- Ticker symbol
- Earnings announcement date
- Price data
- EPS estimates and actual values
- Surprise percentage

The class encapsulates stock data and provides methods for data manipulation, demonstrating **encapsulation** as an OOP principle.

### DataExtraction.h
Handles data retrieval and parsing, including:
- CSV file parsing to categorize stocks
- API calls to fetch historical price data
- Multi-threaded data fetching to improve performance

This component uses **abstraction** by hiding the complex details of data retrieval behind simple interfaces.

### Calculation.h & Calculation.cpp
Implements financial calculations:
- Base `Calculation` class (abstract)
- `Bootstrapping` derived class
- Methods for calculating returns, abnormal returns, AAR, and CAAR

This demonstrates **polymorphism** and **inheritance** with the abstract base class and derived implementation.

### Group.h
Manages stock groups (Beat/Meet/Miss) with functionality for:
- Storing stocks in respective categories
- Managing statistics for each group

### gnuplot.h
Provides an interface to GNUPlot for visualization:
- Plotting CAAR for all groups
- Customizing plot appearance

### Display.h & Display.cpp
Implements user interface display functions:
- Stock data display
- Metrics presentation

### Menu.cpp
The main entry point that:
- Presents the user interface
- Manages program flow
- Coordinates between components
- Implements multi-threading for efficiency

### utils.h & utils.cpp
Utility functions for:
- Data validation
- User input handling
- Helper functions

## Object-Oriented Programming Features

The project demonstrates numerous OOP principles:

1. **Encapsulation**: Data and methods are encapsulated in classes like `Stocks` that provide controlled access to internal data.

2. **Abstraction**: Complex algorithms are hidden behind clean interfaces, particularly in the `Calculation` and `DataExtraction` components.

3. **Inheritance**: The `Bootstrapping` class inherits from the base `Calculation` class, extending its functionality.

4. **Polymorphism**: Virtual functions in the `Calculation` class enable dynamic binding for derived classes.

5. **Operator Overloading**: Used for comparison and arithmetic operations on financial data.

## Multi-threading

The project utilizes multi-threading to significantly improve performance:
- The `ParseAndFetchData` function uses parallel threads to fetch stock data for different stock groups simultaneously
- Threading is synchronized using atomic variables and proper join semantics

This implementation achieves almost 3x speedup compared to sequential processing on a quad-core processor.

## API Integration

The project integrates with the EOD Historical Data API to fetch historical stock prices. An API key is required for this functionality. For testing purposes, the application can run with synthetic data only by setting `USE_SYNTHETIC_DATA_ONLY = true` in Menu.cpp.

## Future Improvements

Potential enhancements include:
- Adding more sophisticated statistical tests
- Implementing additional visualization options
- Extending the analysis to consider other market factors
- Enhancing error handling and data validation 