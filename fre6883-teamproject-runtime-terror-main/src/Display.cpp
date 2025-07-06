#include "Display.h"
#include "Calculation.h"
#include <iomanip>

using namespace std;

namespace fre {
    void DisplayStock(const Stocks& stock)
    {

        Calculation* calculation = new Bootstrapping(40);

        Vector returns, temp_returns, cumulative_returns;
        returns.push_back(0);
        cumulative_returns.push_back(0);

        temp_returns = calculation->cal_DailyRet(stock.GetPrices());
        returns.insert(returns.end(), temp_returns.begin(), temp_returns.end());
        temp_returns.clear();

        temp_returns = calculation->cal_CumulativeRet(stock.GetPrices());
        cumulative_returns.insert(cumulative_returns.end(), temp_returns.begin(), temp_returns.end());

        

        cout<<"\nDisplaying Data for Stock "<<stock.GetTicker()<<endl;
        auto disp = stock.GetPriceData().begin();
        cout<<"\nPrice and Return Data :"<<endl;

        cout << left << setw(15) << "Date"
         << setw(15) << "Price"
         << setw(15) << "Return"
         << setw(15) << "Cumulative Return"
         << endl << endl;

        int idx = 0;
        while(true){
            if (disp == stock.GetPriceData().end()){
                break;
            }
            if (idx >=returns.size())
            {
                cout<<"Index out of range for Returns"<<endl;
                break;
            }
            cout<<left<<setw(15)<< disp->first
                <<setw(15)<<fixed<<setprecision(3) << disp->second
                <<setw(15)<<fixed<<setprecision(3) << returns[idx]
                <<setw(15)<<fixed<<setprecision(3) << cumulative_returns[idx]
                <<endl;
                
            idx++;
           
            // cout<<"Date: "<<disp->first<<" Price: "<<disp->second<<endl;
            
            disp++;
        }
        cout<<"\nStock "<<stock.GetTicker()<<" belongs to group "<<stock.GetGroup()<<endl;
        cout<<"\nEarnings Announcement Date: "<<stock.GetEarningsDate()<<endl;
        cout<<"\nPeriod Ending: "<<stock.GetPeriodEnding()<<endl;
        cout<<"\nEstimated Earnings: "<<stock.GetEstimatedEarnings()<<endl;
        cout<<"\nReported Earnings: "<<stock.GetReportedEarnings()<<endl;
        cout<<"\nSurprise: "<<stock.GetSurpriseEarnings()<<endl;
        cout<<"\nSurprise Percent: "<<stock.GetSurprisePercent()<<"%"<<endl;

        delete calculation;
        calculation = nullptr;

    }

    void FindAndDisplayStock(const string& ticker,
                            const map<string, Stocks>& beat,
                            const map<string, Stocks>& meet,
                            const map<string, Stocks>& miss) 
    {
       
        auto itr = beat.find(ticker);
        if (itr != beat.end()) {
            DisplayStock(itr->second);
            return;
        }

        itr = meet.find(ticker);
        if (itr != meet.end()) {
            DisplayStock(itr->second);
            return;
        }

        itr = miss.find(ticker);
        if (itr != miss.end()) {
            DisplayStock(itr->second);
            return;
        }

        cout << "Ticker " << ticker << " not found in any group." << endl;
    }

    void DisplayMetrics(Vector& AAR, Vector& CAAR, Vector& AAR_SD, Vector& CAAR_SD, int N){
        for (int i = 0; i <AAR.size(); i++){
            cout<<"Day Number: "<<i - N  + 1<<"\tAAR: "<<AAR[i]<<"\tCAAR"<<"\tAAR_SD"<<AAR_SD[i]<<"\tCAAR_SD"<<CAAR_SD[i]<<endl;
        }
    }

}