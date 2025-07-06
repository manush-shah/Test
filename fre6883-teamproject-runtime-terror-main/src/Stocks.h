#ifndef STOCKS_H
#define STOCKS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>

using namespace std;
typedef vector<string> String;
typedef vector<double> Vector;

class Stocks
{
private:
    string ticker;
    string announcement_date;
    string period_ending;
    double estimated_eps;
    double reported_eps;
    double surprise_eps;
    double surprise_percent;
    string group;
    
    string start_date;
    string end_date;
    
    map<string, double> price_data;
    Vector adj_close_prices;
    String dates;
    bool is_sorted;
public:
    // Default constructor
    Stocks() : estimated_eps(0), reported_eps(0), surprise_eps(0), surprise_percent(0), is_sorted(false) {}
    
    // Parameterized constructor
    Stocks(const string& tick, const string& ann_date, const string& per_end,
           double est_eps, double rep_eps, double surp_eps, double surp_pct) :
        ticker(tick), announcement_date(ann_date), period_ending(per_end),
        estimated_eps(est_eps), reported_eps(rep_eps), surprise_eps(surp_eps),
        surprise_percent(surp_pct), is_sorted(false) {}
    
    // Setters
    void SetTicker(const string& tick) { ticker = tick; }
    void SetEarningsDate(const string& date) { announcement_date = date; }
    void SetEarningsPeriod(const string& period) { period_ending = period; }
    void SetEstimatedEarnings(double eps) { estimated_eps = eps; }
    void SetReportedEarnings(double eps) { reported_eps = eps; }
    void SetSurpriseEarnings(double surp) { surprise_eps = surp; }
    void SetSurprisePercent(double pct) { surprise_percent = pct; }
    void SetStartDate(const string& date) { start_date = date; }
    void SetEndDate(const string& date) { end_date = date; }
    void AddPrice(const double& price){ adj_close_prices.push_back(price);}
    void AddDate(const string& date){ dates.push_back(date);}
    void SetGroup(const string& stock_group){group = stock_group;}
    
    // Getters
    string GetTicker() const { return ticker; }
    string GetEarningsDate() const { return announcement_date; }
    string GetPeriodEnding() const { return period_ending; }
    double GetEstimatedEarnings() const { return estimated_eps; }
    double GetReportedEarnings() const { return reported_eps; }
    double GetSurpriseEarnings() const { return surprise_eps; }
    double GetSurprisePercent() const { return surprise_percent; }
    string GetStartDate() const { return start_date; }
    string GetEndDate() const { return end_date; }
    const map<string, double>& GetPriceData() const { return price_data; }
    string GetGroup()const {return group;}
    String GetDates() const{ 
        String dates;
        for (auto it = price_data.begin(); it != price_data.end(); ++it)
            dates.push_back(it->first);

        return dates;
    }
    Vector GetPrices() const {
        Vector prices;
        for (auto it = price_data.begin(); it != price_data.end(); ++it)
            prices.push_back(it->second);
        
        return prices;
    }
    
   
    
    // Methods for price data
    void AddPriceData(const string& date, double price) {
        // Check if date already exists to prevent duplicates

        if (price_data.find(date) != price_data.end()) {
            // Update existing price
            price_data[date] = price;
            
            // Update the vector as well by finding the index
            auto it = find(dates.begin(), dates.end(), date);
            if (it != dates.end()) {
                int index = distance(dates.begin(), it);
                if (index >= 0 && index < (int)adj_close_prices.size()) {
                    adj_close_prices[index] = price;
                }
            }
        } else {
            // Add new price
            price_data[date] = price;
            dates.push_back(date);
            adj_close_prices.push_back(price);
            is_sorted = false;
        }
    }
    
    // Make sure dates and prices are in chronological order
    void SortData() {
        if (!is_sorted && !dates.empty()) {
            // Create a vector of pairs with date and price
            vector<pair<string, double>> sorted_data;
            for (size_t i = 0; i < dates.size(); i++) {
                sorted_data.push_back(make_pair(dates[i], adj_close_prices[i]));
            }
            
            // Sort by date
            sort(sorted_data.begin(), sorted_data.end(), 
                 [](const pair<string, double>& a, const pair<string, double>& b) {
                     return a.first < b.first;
                 });
            
            // Update the vectors
            dates.clear();
            adj_close_prices.clear();
            for (const auto& pair : sorted_data) {
                dates.push_back(pair.first);
                adj_close_prices.push_back(pair.second);
            }
            
            is_sorted = true;
        }
    }
    
   
    // map<string, double>& GetPrices() { return price_data; }
    
    const Vector& GetACP() const { 
        // Ensure data is sorted before returning
        const_cast<Stocks*>(this)->SortData();
        return adj_close_prices;
    }
    
    const String& GetDate() const {
        // Ensure data is sorted before returning
        const_cast<Stocks*>(this)->SortData();
        return dates;
    }
    
    void ClearData() {
        price_data.clear();
        adj_close_prices.clear();
        dates.clear();
        is_sorted = false;
    }



};

#endif 