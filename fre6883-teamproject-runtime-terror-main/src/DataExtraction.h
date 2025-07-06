#ifndef DATA_EXTRACTION_H
#define DATA_EXTRACTION_H

#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <curl/curl.h>
#include "Stocks.h"

using namespace std;
using namespace std::chrono;
typedef vector<string> String;
string API_KEY = "68073c9c2642b3.55509782";

namespace fre {

    // Callback function for writing received data from cURL
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp)
    {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    string ConvertDateFormat(const string& date)
    {
        int month, day, year;
        sscanf(date.c_str(), "%d/%d/%d", &month, &day, &year);
        std::ostringstream out;

        out << std::setfill('0') << std::setw(4) << year << "-"
            << std::setw(2) << month << "-"
            << std::setw(2) << day;

        return out.str();
    }

    // Retrieve stock price data using API
    bool FetchStockData(map<string, Stocks>& stock_group, String& tickers, const string& api_key, string dataStartDate, string dataEndDate, int maxN)
    {
        CURL* curl;
        CURLcode res;
        string readBuffer;
        string earningsDate;
        
        vector<string> dateList;
        vector<double> priceList;
        int earnings_date_index = -1;
        int start_index = -1;
        int end_index = -1;
        
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        
        if (!curl) {
            cerr << "cURL initialization failed" << endl;
            return false;
        }
        
        for (auto it = tickers.begin(); it!=tickers.end(); ) {
            const string& ticker = *it;
            if (stock_group.find(ticker) == stock_group.end()) {
                it++;
                continue;
            }

            //clearing data from previous iterations
            start_index = earnings_date_index = end_index = -1;
            dateList.clear();
            priceList.clear();
            stock_group[ticker].ClearData();

            earningsDate = stock_group[ticker].GetEarningsDate();
            Stocks& stock = stock_group[ticker];
            // cout<<stock.GetTicker()<<" Earnings Date : "<<earningsDate<<endl;
            // string start_date = stock.GetStartDate();
            // string end_date = stock.GetEndDate();
            string start_date = dataStartDate;
            string end_date = dataEndDate;

            // cout<<start_date<< " "<<end_date<<endl;
            if (start_date.empty() || end_date.empty()) {
                cerr << "Warning: Missing date range for " << ticker << endl;
                it++;
                continue;
            }

            
            // Remove fmt=json parameter to get CSV format instead
            string url = "https://eodhistoricaldata.com/api/eod/" + 
                        ticker + ".US?" +
                        "from=" + start_date + 
                        "&to=" + end_date + 
                        "&api_token=" + api_key + 
                        "&period=d";
            
            readBuffer.clear();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            
            // Add these options from the reference code
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
            
            res = curl_easy_perform(curl);
            
            if (res != CURLE_OK) {
                cerr << "cURL error for " << ticker << ": " << curl_easy_strerror(res) << endl;
                return false;
            }
            
            // Parse CSV response
            istringstream iss(readBuffer);
            string line;
            
            // Skip header line
            getline(iss, line);
            
            // Check if we got any data
            string data_line;
            // bool has_data = false;
            while (getline(iss, data_line)) {
                stringstream ss(data_line);
                string date, open, high, low, close, adjusted_close, volume;
                
                getline(ss, date, ',');
                dateList.push_back(date);
                if (date == earningsDate){
                    earnings_date_index = dateList.size()-1;
                } else if (date >  earningsDate && earnings_date_index == -1){
                    earnings_date_index = dateList.size()-1;
                }
                getline(ss, open, ',');
                getline(ss, high, ',');
                getline(ss, low, ',');
                getline(ss, close, ',');
                getline(ss, adjusted_close, ',');
                priceList.push_back(stod(adjusted_close));
                getline(ss, volume, ',');
            }

            if (earnings_date_index ==-1){
                cout<<ticker<<"Earnings Date Not Found!!!"<<endl;
            }
            start_index = earnings_date_index - maxN;
            end_index = earnings_date_index + maxN;
            // cout<<ticker<<" Earnings Date: "<<earningsDate<<" Found Date: "<<dateList[earnings_date_index]<<" Start Index: "<<start_index<<" End Index: "<<end_index<<endl;
            
            //handle missing data for 7 tickers 
            // if (ticker  == "AMTM" || ticker == "BYRN" ||ticker == "KLC" || ticker == "INGM" ||ticker== "NRIX" || ticker == "ECG" || ticker =="UPB"){
            //     continue;
            // }
            if (start_index< 0 || end_index > dateList.size() || dateList.size()!= priceList.size()){
                cout<<"Warning!! Not sufficient Data for ticker "<<ticker<<" removing it from the dataset"<<endl;
                stock_group.erase(ticker);
                it = tickers.erase(it);
                continue;
                // return false;
            }
            if (ticker == "IWV"){
                start_index = 0;
                end_index = dateList.size() - 1;
            }
            stock.SetStartDate(dateList[start_index]);
            stock.SetEndDate(dateList[end_index]);
            for(int idx = start_index;  idx<=end_index; idx++){
                // cout<<dateList[idx]<<" "<<priceList[idx]<<endl;
                // cout<<"Adding data at index "<<idx<<endl;
                // stock.AddPrice(priceList[idx]);
                // stock.AddDate(dateList[idx]);
                stock.AddPriceData(dateList[idx], priceList[idx]);
            }
            // stock.CalculateLogReturns();
            it++;
        }
        
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        
        return true;
    }

    // Retrieve IWV price data using API
    bool FetchIWVData(Stocks& IWV ,string dataStartDate, string dataEndDate, string api_key)
    {
        string ticker = "IWV";
        CURL* curl;
        CURLcode res;
        string readBuffer;
        string earningsDate;
        
        vector<string> dateList;
        vector<double> priceList;
        int earnings_date_index = -1;
        int start_index = -1;
        int end_index = -1;
        IWV.SetTicker("IWV");
        
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();
        
        if (!curl) {
            cerr << "cURL initialization failed" << endl;
            return false;
        }

        //clearing data from previous iterations
        start_index = earnings_date_index = end_index = -1;
        dateList.clear();
        priceList.clear();

        // earningsDate = stock_group[ticker].GetEarningsDate();
        // Stocks& stock = stock_group[ticker];
        // cout<<stock.GetTicker()<<" Earnings Date : "<<earningsDate<<endl;
        // string start_date = stock.GetStartDate();
        // string end_date = stock.GetEndDate();
        string start_date = dataStartDate;
        string end_date = dataEndDate;

        // cout<<start_date<< " "<<end_date<<endl;
        if (start_date.empty() || end_date.empty()) {
            cerr << "Warning: Missing date range for " << ticker << endl;
            return false;
        }
        
        
        
        // Remove fmt=json parameter to get CSV format instead
        string url = "https://eodhistoricaldata.com/api/eod/" + 
                        ticker + ".US?" +
                        "from=" + start_date + 
                        "&to=" + end_date + 
                        "&api_token=" + api_key + 
                        "&period=d";
        
        readBuffer.clear();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Add these options from the reference code
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
        
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            cerr << "cURL error for " << ticker << ": " << curl_easy_strerror(res) << endl;
            return false;
        }
        
        // Parse CSV response
        istringstream iss(readBuffer);
        string line;
        
        // Skip header line
        getline(iss, line);
        
        // Check if we got any data
        string data_line;
        // bool has_data = false;
        while (getline(iss, data_line)) {
            stringstream ss(data_line);
            string date, open, high, low, close, adjusted_close, volume;
            
            getline(ss, date, ',');
            dateList.push_back(date);
            if (date == earningsDate){
                earnings_date_index = dateList.size()-1;
            } else if (date >  earningsDate && earnings_date_index == -1){
                earnings_date_index = dateList.size()-1;
            }
            getline(ss, open, ',');
            getline(ss, high, ',');
            getline(ss, low, ',');
            getline(ss, close, ',');
            getline(ss, adjusted_close, ',');
            priceList.push_back(stod(adjusted_close));
            getline(ss, volume, ',');
        }

        if (earnings_date_index ==-1){
            cout<<ticker<<" Earnings Date Not Found!!!"<<endl;
            return false;
        }
        start_index = 0;
        end_index = dateList.size()-1;
        IWV.SetStartDate(dateList[start_index]);
        IWV.SetEndDate(dateList[end_index]);
        for(int idx = start_index;  idx<=end_index; idx++){
            // cout<<dateList[idx]<<" "<<priceList[idx]<<endl;
            // cout<<"Adding data at index "<<idx<<endl;
            // stock.AddPrice(priceList[idx]);
            // stock.AddDate(dateList[idx]);
            IWV.AddPriceData(dateList[idx], priceList[idx]);
        }
        // IWV.CalculateLogReturns();
        
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        
        return true;
    }

    // Parse CSV file to extract earnings data
    void ParseCSV(map<string, Stocks>& beat_group, map<string, Stocks>& meet_group, map<string, Stocks>& miss_group, const string& csv_path, string& max_announcement_date, string& min_announcement_date)
    {
        ifstream file(csv_path);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << csv_path << endl;
            return;
        }
        
        string line;
        // Skip header line
        getline(file, line);
        
        // First, collect all stocks in a vector
        vector<pair<string, Stocks>> all_stocks;
        // string max_announcement_date, min_announcement_date;
        bool flag = 1;
        
        while (getline(file, line)) {
            stringstream ss(line);
            string ticker, announcement_date, period_ending, est_eps_str, rep_eps_str, surp_eps_str, surp_pct_str;
            
            // Parse CSV fields
            getline(ss, ticker, ',');
            getline(ss, announcement_date, ',');
            getline(ss, period_ending, ',');
            getline(ss, est_eps_str, ',');
            getline(ss, rep_eps_str, ',');
            getline(ss, surp_eps_str, ',');
            getline(ss, surp_pct_str, ',');
            
            // Convert dates to YYYY-MM-DD format

            string formatted_announcement_date = ConvertDateFormat(announcement_date);
            string formatted_period_ending = ConvertDateFormat(period_ending);
            if (flag)
            {
                max_announcement_date = formatted_announcement_date;
                min_announcement_date = formatted_announcement_date;
                flag = 0;
            }

            if (max_announcement_date < formatted_announcement_date)
            {
                max_announcement_date = formatted_announcement_date;
            }
            if (min_announcement_date > formatted_announcement_date)
            {
                min_announcement_date = formatted_announcement_date;
            }
            
            // Convert string to double
            double est_eps = stod(est_eps_str);
            double rep_eps = stod(rep_eps_str);
            double surp_eps = stod(surp_eps_str);
            double surp_pct = stod(surp_pct_str);
            
            // Create a stock object
            Stocks stock(ticker, formatted_announcement_date, formatted_period_ending, est_eps, rep_eps, surp_eps, surp_pct);
            
            // Add to collection of all stocks
            all_stocks.push_back(make_pair(ticker, stock));
        }
        
        file.close();
        
        // Sort all stocks by surprise percent (from lowest to highest)
        sort(all_stocks.begin(), all_stocks.end(), 
            [](const pair<string, Stocks>& a, const pair<string, Stocks>& b) {
                return a.second.GetSurprisePercent() < b.second.GetSurprisePercent();
            });
        
        // Divide into three approximately equal groups
        size_t total = all_stocks.size();
        size_t one_third = total / 3;
        
        // Add lowest third to Miss group
        for (size_t i = 0; i < one_third; i++) {
            all_stocks[i].second.SetGroup("miss");
            miss_group[all_stocks[i].first] = all_stocks[i].second;
            
        }
        
        // Add middle third to Meet group
        for (size_t i = one_third; i < 2 * one_third; i++) {
            all_stocks[i].second.SetGroup("meet");
            meet_group[all_stocks[i].first] = all_stocks[i].second;
        }
        
        // Add highest third to Beat group
        for (size_t i = 2 * one_third; i < total; i++) {
            all_stocks[i].second.SetGroup("beat");
            beat_group[all_stocks[i].first] = all_stocks[i].second;
        }
    }

    String GetTickers(map<string, Stocks>& stock_map)
    {
        String tickers;
        // string startdate;
        map<string, Stocks>:: iterator itr = stock_map.begin();
        for(;itr!= stock_map.end(); itr++){
            tickers.push_back(itr->first);
            // break;
        }

        return tickers;
    }

    void ParseData(map<string, Stocks>& beat_third, map<string, Stocks>&  meet_third, map<string, Stocks>&  miss_third,  String& beat_tickers, String& meet_tickers, String& miss_tickers, string& max_date, string& min_date)
    {
        cout << "Parsing CSV file..." << endl;
        
        ParseCSV(beat_third, meet_third, miss_third, "../data/Russell3000EarningsAnnouncements.csv", max_date, min_date);
        max_date = "2025-05-07";
        min_date = "2024-05-07";
        cout<<"\nmax date "<<max_date<<" min date "<<min_date<<endl;

        cout << "\nBeat Group Size: " << beat_third.size() << endl;
        cout << "Meet Group Size: " << meet_third.size() << endl; 
        cout << "Miss Group Size: " << miss_third.size() << endl;

        miss_tickers = GetTickers(miss_third);
        beat_tickers = GetTickers(beat_third);
        meet_tickers = GetTickers(meet_third);

        return;
    }

    bool ParseAndFetchData(map<string, Stocks>& beat_third, map<string, Stocks>&  meet_third, map<string, Stocks>&  miss_third, Stocks& IWV,  String& beat_tickers, String& meet_tickers, String& miss_tickers, int N, string max_date, string min_date)
    {
   
        ParseData(beat_third, meet_third, miss_third, beat_tickers, meet_tickers, miss_tickers, max_date, min_date);
        FetchIWVData(IWV, min_date, max_date, API_KEY);
        cout<<"Data fetched for market. Ticker of the market is = "<<IWV.GetTicker()<<endl;

        auto start = high_resolution_clock::now();
        std::atomic<bool> done(false);
        bool successBeat, successMeet, successMiss;

        thread timer_thread([&]() {
            int last_logged = 0;
            while (!done.load()) {
                std::this_thread::sleep_for(seconds(1));
                auto now = high_resolution_clock::now();
                int elapsed = duration_cast<seconds>(now-start).count();
                if (elapsed%30 ==0 && elapsed!= last_logged) {
                    cout << "[Data Fetching in Progress] Elapsed time: " << elapsed << " seconds" << endl;
                    last_logged = elapsed;
                }
            }
        });

        thread t1([&](){
            successBeat = FetchStockData(beat_third, beat_tickers, API_KEY, min_date, max_date, N);
            // cout<<"Thread Beat Done"<<endl;
        });

        thread t2([&](){
            successMeet = FetchStockData(meet_third, meet_tickers, API_KEY, min_date, max_date, N);
            // cout<<"Thread Meet Done"<<endl;
        });

        thread t3([&](){
            successMiss = FetchStockData(miss_third, miss_tickers, API_KEY, min_date, max_date, N);
            // cout<<"Thread Miss Done"<<endl;
        });

        t1.join();
        t2.join();
        t3.join();

        done.store(true);
        timer_thread.join();

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<seconds>(stop - start);
        cout<<"Fetched data in seconds: "<<duration.count()<<endl;

        if (!successBeat || !successMeet || !successMiss){
            cout << "Data fetching failed for one or more groups. Terminating Program." << endl;
            return false;
        }

        return true;
    }

} // namespace fre

#endif 