CXX = g++
CXXFLAGS = -Wall -std=c++11 -g
LDFLAGS = -lcurl -pthread

SOURCES = Menu.cpp Calculation.cpp utils.cpp Display.cpp
HEADERS = Stocks.h Calculation.h DataExtraction.h gnuplot.h utils.h Display.h

TARGET = EarningsAnalysis

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDFLAGS)

# Add verification target
clean:
	rm -f $(TARGET)

.PHONY: all clean 