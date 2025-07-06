#ifndef CALCULATION_H
#define CALCULATION_H

#include "Stocks.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

typedef vector<double> Vector;
typedef vector<Vector> Matrix;

class Calculation {
protected:
    Vector AAR;
    Vector CAAR;
    Vector daily_returns;
    Vector cumulative_returns;
    Vector abnormal_returns;

public:
    Calculation();

    virtual ~Calculation() {}  // virtual destructor for safe polymorphism

    Vector cal_DailyRet(const Vector& prices);
    Vector cal_CumulativeRet(const Vector& prices);
    Vector cal_ABRet(const Vector& stock_prices, const Vector& market_prices);
    Vector cal_AAR(const Matrix& abnormal_returns);
    Vector cal_CAAR(const Vector& aar);
    Vector cal_STD_PerDay(const Matrix& data_matrix);
    
    Vector GetMarketPrice(const string& ticker, const map<string, Stocks>& stock_group, const Stocks& IWV);
    Vector GetCAAR()const{return CAAR;}
    Vector GetAAR()const{return AAR;}

    // New: polymorphic interface (optional in base)
    virtual void run_bootstrap(const map<string, Stocks>& beat_third, const Stocks& IWV) = 0;
    virtual void summarize() = 0;
    void ClearData(){
        // cout<<"Clearing Calculation"<<endl;
        AAR.clear();
        CAAR.clear();
        daily_returns.clear();
        cumulative_returns.clear();
        abnormal_returns.clear();
    }
};

class Bootstrapping : public Calculation {
protected:
    int num_samples;
    Matrix AAR_matrix;
    Matrix CAAR_matrix;
    vector<string> tickers;
    Matrix abnormal_matrix;

public:
    Bootstrapping(int num_samples_) : num_samples(num_samples_) {}

    void run_bootstrap(const map<string, Stocks>& beat_third, const Stocks& IWV);
    void summarize();
    // Vector GetCAAR()const{return CAAR;}
    // Vector GetAAR()const{return AAR;}
    // Vector GetAAR_SD()const{return AAR_SD;}
    // Vector GetCAAR_SD()const{return CAAR_SD;}

    void ClearData() {
        // cout<<"Clearing Bootstrap"<<endl;
        AAR_matrix.clear();
        CAAR_matrix.clear();
        tickers.clear();
        abnormal_matrix.clear();
        // abnormal_returns.clear();
        Calculation::ClearData();
    }
};

#endif // CALCULATION_H
