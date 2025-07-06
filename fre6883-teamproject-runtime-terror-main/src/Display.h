#ifndef DISPLAY_H
#define DISPLAY_H

#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <curl/curl.h>
#include "Stocks.h"

using namespace std;
using namespace std::chrono;
typedef vector<string> String;
// string API_KEY = "67929ea20eadb6.18593983"; 

namespace fre {
   
    void DisplayStock(const Stocks& stock);


    void FindAndDisplayStock(const string& ticker,
                            const map<string, Stocks>& beat,
                            const map<string, Stocks>& meet,
                            const map<string, Stocks>& miss) ;

    void DisplayMetrics(Vector& AAR, Vector& CAAR, Vector& AAR_SD, Vector& CAAR_SD, int N);

} // namespace fre

#endif 