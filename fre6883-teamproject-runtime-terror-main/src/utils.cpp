#include "utils.h"

using namespace std;

namespace fre{

    int FetchNumberOfDays(const int maxN, const int minN){
        int N = -1;
        int attempts = 1;
        const int maxAttempts = 3;
        while (attempts <=maxAttempts){        
            cout << "\nEnter number of days N for pre and post announcement dates (N must be between " << minN << " and " << maxN << " ): ";
            cin >> N;

            if (N>=minN && N <= maxN){
                break;
            }
            else{
                if (attempts >= 3){
                    throw std::invalid_argument("\nN must be between 30 and 60. Exiting Program\n");
                }
                cout<<"N must be between 30 and 60. Try again. Remaining attempts = "<<maxAttempts - attempts;
                attempts++;
            }
        }
    
        return N;
    }

    void VerifyGroupData(map<string, Stocks>& third, String& tickers, int N)
    {
        cout<<"Stocks with prices less then 2N+1 in the given group:"<<endl;

        for(int i =0; i < tickers.size(); i++){
            auto itr = third.find(tickers[i]);
            if (itr == third.end()){
                cout<<"Data not found for ticker "<<tickers[i]<<endl;
            }
            if (itr->second.GetPriceData().size() != ((2*N) +1))
            {
                cout<<itr->second.GetTicker()<< " size = "<<itr->second.GetPriceData().size()<<endl;
            }
            if (itr->second.GetPrices().size()!= itr->second.GetDates().size()){
                cout<<"Mismatch in lengths of price and date list for "<<itr->second.GetTicker()<<endl;
            }
        }
    }

}