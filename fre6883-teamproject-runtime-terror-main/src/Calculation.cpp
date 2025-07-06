#include "Calculation.h"
#include "Stocks.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <random>

// Constructor
Calculation::Calculation() : AAR(), CAAR(), daily_returns(), cumulative_returns(), abnormal_returns() {}

// ----------- Operator Overloads -----------

Vector& operator+=(Vector& lhs, const Vector& rhs) {
    if (lhs.size() != rhs.size())
        throw invalid_argument("Vector sizes must match for operator+=.");
    for (size_t i = 0; i < lhs.size(); ++i)
        lhs[i] += rhs[i];
    return lhs;
}

Vector operator+(const Vector& lhs, const Vector& rhs) {
    if (lhs.size() != rhs.size())
        throw  invalid_argument("Vector sizes must match for operator+.");
    Vector result(lhs.size());
    for (size_t i = 0; i < lhs.size(); ++i)
        result[i] = lhs[i] + rhs[i];
    return result;
}

Vector operator-(const Vector& lhs, const Vector& rhs) {
    if (lhs.size() != rhs.size())
        throw  invalid_argument("Vector sizes must match for operator-.");
    Vector result(lhs.size());
    for (size_t i = 0; i < lhs.size(); ++i)
        result[i] = lhs[i] - rhs[i];
    return result;
}

// ----------- Calculation Methods -----------

Vector Calculation::cal_DailyRet(const Vector& prices) {
    daily_returns.clear();
    for (size_t i = 1; i < prices.size(); ++i) {
        double ret = log(prices[i] / prices[i - 1]);
        daily_returns.push_back(ret);
    }
    return daily_returns;
}

Vector Calculation::cal_CumulativeRet(const Vector& prices) {
    cumulative_returns.clear();
    daily_returns = cal_DailyRet(prices);
    double cumulative = 0.0;
    for (size_t i = 0; i < daily_returns.size(); ++i) {
        cumulative += daily_returns[i];
        cumulative_returns.push_back(cumulative);
    }
    return cumulative_returns;
}

Vector Calculation::cal_ABRet(const Vector& stock_prices, const Vector& market_prices) {
    Vector stock_returns = cal_DailyRet(stock_prices);
    Vector market_returns = cal_DailyRet(market_prices);
    abnormal_returns = stock_returns - market_returns;
    return abnormal_returns;
}

Vector Calculation::cal_AAR(const Matrix& abnormal_returns) {
    if (abnormal_returns.empty()) return {};

    size_t num_days = abnormal_returns[0].size();
    AAR.assign(num_days, 0.0);

    for (size_t i = 0; i < abnormal_returns.size(); ++i) {
        const Vector& row = abnormal_returns[i];
        if (row.size() != num_days) {
            // cout<<"row size is "<<row.size()<<" num days "<<num_days<<endl;
            throw runtime_error("Row size mismatch in abnormal_returns.");
        }
        AAR += row;
    }

    for (size_t j = 0; j < num_days; ++j) {
        AAR[j] /= abnormal_returns.size();
    }

    return AAR;
}

Vector Calculation::cal_CAAR(const Vector& aar) {
    CAAR.clear();
    if (aar.empty()) return CAAR;

    double cumulative = 0.0;
    for (size_t i = 0; i < aar.size(); ++i) {
        cumulative += aar[i];
        CAAR.push_back(cumulative);
    }

    return CAAR;
}

Vector Calculation::cal_STD_PerDay(const Matrix& data_matrix) {
    if (data_matrix.empty()) return {};
    size_t num_days = data_matrix[0].size();
    size_t num_samples = data_matrix.size();
    Vector std_per_day(num_days, 0.0);

    for (size_t j = 0; j < num_days; ++j) {
        double sum = 0.0;
        for (size_t i = 0; i < num_samples; ++i) {
            sum += data_matrix[i][j];
        }
        double mean = sum / num_samples;

        double sq_diff_sum = 0.0;
        for (size_t i = 0; i < num_samples; ++i) {
            double diff = data_matrix[i][j] - mean;
            sq_diff_sum += diff * diff;
        }

        std_per_day[j] = sqrt(sq_diff_sum / (num_samples - 1));
    }

    return std_per_day;
}

Vector Calculation::GetMarketPrice(const string& ticker, const map<string, Stocks>& stock_group, const Stocks& IWV) {
    const auto stock_itr = stock_group.find(ticker);
    if (stock_itr == stock_group.end()) {
        throw invalid_argument("Ticker not found in the group.");
    }

    const map<string, double>& stock_prices = stock_itr->second.GetPriceData();
    const map<string, double>& iwv_prices = IWV.GetPriceData();
    
    Vector market_prices;
    for (auto it = stock_prices.begin(); it != stock_prices.end(); ++it) {
        const string& date = it->first;
        auto iwv_it = iwv_prices.find(date);
        if (iwv_it != iwv_prices.end()) {
            market_prices.push_back(iwv_it->second);
        }
    }
    return market_prices;
}

// ----------- Bootstrapping (Overrides) -----------

void Bootstrapping::run_bootstrap(const map<string, Stocks>& beat_third, const Stocks& IWV) {
    cout.clear();
    ClearData();
    cout << "Work in Progress - Running " << num_samples << " simulations...\n";

    Vector sampleAAR, sampleCAAR;

    for (int run = 0; run < num_samples; ++run) {
        tickers.clear();
        for (const auto& pair : beat_third) {
            tickers.push_back(pair.first);
        }

        random_device rd;
        mt19937 gen(rd());
        shuffle(tickers.begin(), tickers.end(), gen);

        vector<string> filtered_tickers;
        for (const auto& ticker : tickers) {
            filtered_tickers.push_back(ticker);
        }

        int sample_size = 30;
        vector<string> sample_tickers(filtered_tickers.begin(), filtered_tickers.begin() + sample_size);

        abnormal_matrix.clear();
        for (const string& ticker : sample_tickers) {
            auto stock_itr = beat_third.find(ticker);
            if (stock_itr == beat_third.end()) continue;

            Vector stock_prices = stock_itr->second.GetPrices();
            Vector market_prices = GetMarketPrice(ticker, beat_third, IWV);

            if (stock_prices.size() != market_prices.size()) {
                
                continue;
            }

            abnormal_returns = cal_ABRet(stock_prices, market_prices);
            abnormal_matrix.push_back(abnormal_returns);
        }

        sampleAAR = cal_AAR(abnormal_matrix);
        sampleCAAR = cal_CAAR(AAR);

        AAR_matrix.push_back(sampleAAR);
        CAAR_matrix.push_back(sampleCAAR);
    }
    AAR = cal_AAR(this->AAR_matrix);
    CAAR = cal_CAAR(AAR);
}

void Bootstrapping::summarize() {
    Vector AAR_std = cal_STD_PerDay(this->AAR_matrix);
    Vector CAAR_std = cal_STD_PerDay(this->CAAR_matrix);

    cout << "\nDetailed Final Metrics:\n\n";
    cout << left << setw(10) << "Day"
         << setw(15) << "AAR"
         << setw(15) << "AAR STD"
         << setw(15) << "CAAR"
         << setw(15) << "CAAR STD"
         << endl << endl;

    int N = AAR.size() / 2;  // assuming symmetric window
    for (int i = 0; i < AAR.size(); ++i) {
        cout << left << setw(10) << (i - N + 1)
             << setw(15) << fixed << setprecision(5) << AAR[i]
             << setw(15) << fixed << setprecision(5) << AAR_std[i]
             << setw(15) << fixed << setprecision(5) << CAAR[i]
             << setw(15) << fixed << setprecision(5) << CAAR_std[i]
             << endl;
    }
}
