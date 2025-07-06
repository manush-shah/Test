#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <cmath>
#include <sstream>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <random>

#include "Calculation.h"
#include "Stocks.h"
#include "DataExtraction.h"
#include "gnuplot.h"
#include "utils.h"
#include "Display.h"

using namespace std;
using namespace fre;

typedef vector<string> String;
typedef vector<double> Vector;
typedef vector<Vector> Matrix;
typedef vector<Matrix> MatrixList;


int main(){
    srand(time(NULL));
    
    string val; // Menu input value to select from options
    
    const int maxN = 60;
    const int minN = 30;
    
    map<string, Stocks> beat_third;
    map<string, Stocks> meet_third;
    map<string, Stocks> miss_third;
    
    String miss_tickers;
    String beat_tickers;
    String meet_tickers;
    
    string max_date, min_date;
    Stocks IWV;
    
    // Bootstrapping beatBootstrap(40);
    // Bootstrapping meetBootstrap(40);
    // Bootstrapping missBootstrap(40);
    Calculation* beatCalculation = new Bootstrapping(40);
    Calculation* meetCalculation = new Bootstrapping(40);
    Calculation* missCalculation = new Bootstrapping(40);

    bool beatBootstrapFlag = false;
    bool meetBootstrapFlag = false;
    bool missBootstrapFlag = false;

    bool dataLoaded = false;
    
    int N = 0;
    int prev_N = -1;
    
    // Fetched data once with maxN. ONLY keep this or the function call below!!
    // ParseAndFetchData(beat_third, meet_third, miss_third, IWV, beat_tickers, meet_tickers, miss_tickers, maxN, max_date, min_date);

    
    while(true){
        system("clear");
        cout << endl;
        cout << " Main Menu " << endl;
        cout << "==============================================\n";
        cout << "1. Enter N to fetch data and group stocks\n";
        cout << "2. Display details of one stock from a group\n";
        cout << "3. Calculate AAR and CAAR for each group\n";
        cout << "4. Display Plots\n";
        cout << "5. Exit\n";
        cout << "Please enter your choice (1-5): ";
        cin >> val;
        
        if (val == "1") {
            system("clear");
            dataLoaded = true;
            N = FetchNumberOfDays(maxN, minN);

            if (prev_N == N){
                cout<<"Data is already loaded for N = "<<N<<endl;
            }
            else{
                cout << "\nPlease wait while we process the data..." << endl;

                //Fetces data everytime N is updated. ONLY keep this or the function call above!!
                ParseAndFetchData(beat_third, meet_third, miss_third, IWV, beat_tickers, meet_tickers, miss_tickers, N, max_date, min_date);
                prev_N = N;
                beatBootstrapFlag = false;
                meetBootstrapFlag = false;
                missBootstrapFlag = false;
            }

            cout<<"\nVerifing Miss Group size "<<miss_tickers.size()<<" map size: "<<miss_third.size()<<endl;
            VerifyGroupData(miss_third, miss_tickers, N);
            cout<<"\nVerifing Beat Group size "<<beat_tickers.size()<<" map size: "<<beat_third.size()<<endl;
            VerifyGroupData(beat_third, beat_tickers,  N);
            cout<<"\nVerifing Meet Group size "<<meet_tickers.size()<<" map size: "<<meet_third.size()<<endl;
            VerifyGroupData(meet_third, meet_tickers, N);

            cout<<"\n\nData fetching and verification Complete!!"<<endl;
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();  // Waits for Enter key before exiting the menu option

        } else if (val == "5"){
            system("clear");
            cout << "Goodbye! Exiting Program" << endl;
            break;

        } else if (!dataLoaded){
            cout<<"Cannot proceed to other options without loading data. Please Press 1 and load data first"<<endl;
            cout<<"Press Enter to continue..."<<endl;
            cin.ignore();
            cin.get();

        } else if (val == "2") {
            
            system("clear");
            string ticker;
            cout<<"Enter Ticker of Stock: "<<endl;
            cin>>ticker;
            FindAndDisplayStock(ticker, beat_third,meet_third, miss_third);

            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();  // Waits for Enter key before exiting the menu option
            
        } else if (val == "3") {
           
            int choice;
            cout << "Select the group to analyze:\n";
            cout << "1: BEAT\n2: MEET\n3: MISS\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 1) {
                if (beatBootstrapFlag == false){
                    beatCalculation->run_bootstrap(beat_third, IWV);
                    beatBootstrapFlag = true;
                    cout << "\n\nBootstrapping Complete!!" << endl << endl;
                }
                cout<<"Metrics for Group Beat"<<endl;
                beatCalculation->summarize();
            } else if (choice == 2) {
                if (meetBootstrapFlag ==false){
                    meetCalculation->run_bootstrap(meet_third, IWV);
                    meetBootstrapFlag = true;
                    cout << "\n\nBootstrapping Complete!!" << endl << endl;
                }
                cout<<"Metrics for Group Meet"<<endl;
                meetCalculation->summarize();
            } else if (choice == 3) {
                if (missBootstrapFlag == false){
                    missCalculation->run_bootstrap(miss_third, IWV);
                    missBootstrapFlag = true;
                    cout << "\n\nBootstrapping Complete!!" << endl << endl;
                }
                cout<<"Metrics for Group Miss"<<endl;
                missCalculation->summarize();
            } else {
                cout << "Invalid choice. Please enter 1, 2, or 3." << endl;
            }

            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();  // Waits for Enter key before exiting the menu option
        
            
        }else if (val == "4"){
            system("clear");
            cout << "Displaying CAAR plot for all groups..." << endl;
        
            if (beatBootstrapFlag == false){
                beatCalculation->run_bootstrap(beat_third, IWV);
                beatBootstrapFlag = true;
            }
            Vector beat_caar = beatCalculation->GetCAAR();

            if (meetBootstrapFlag == false){
                meetCalculation->run_bootstrap(meet_third, IWV);
                meetBootstrapFlag = true;
            }
            Vector meet_caar = meetCalculation->GetCAAR();

            if(missBootstrapFlag == false){
                missCalculation->run_bootstrap(miss_third, IWV);
                missBootstrapFlag = true;
            }
            Vector miss_caar = missCalculation->GetCAAR();
        
            Gnuplot plotter;
            plotter.PlotCAAR(beat_caar, meet_caar, miss_caar, N);
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();  // Waits for Enter key before exiting the menu option
            
        } else {
            system("clear");
            cout << "Invalid Option. Try Again." << endl;
        }
    }

    delete beatCalculation;
    beatCalculation = nullptr;

    delete meetCalculation;
    meetCalculation = nullptr;

    delete missCalculation;
    missCalculation = nullptr;
    
    return 0;
} 
