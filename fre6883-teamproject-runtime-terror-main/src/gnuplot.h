#ifndef GNUPLOT_H
#define GNUPLOT_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;
typedef vector<double> Vector;

namespace fre {

class Gnuplot {
private:
    string tmp_path;
    FILE* gnuplot_pipe;
    
public:
    Gnuplot() {
        gnuplot_pipe = popen("gnuplot", "w");
        if (!gnuplot_pipe) {
            cerr << "Error: Could not open pipe to gnuplot" << endl;
            exit(EXIT_FAILURE);
        }
        tmp_path = "/tmp/gnuplot_data.txt";
    }

    ~Gnuplot() {
        if (gnuplot_pipe) {
            fprintf(gnuplot_pipe, "exit\n");
            pclose(gnuplot_pipe);
        }
    }

    void PlotCAAR(const Vector& beat_caar, const Vector& meet_caar, const Vector& miss_caar, int N) {
        ofstream tmp_file(tmp_path);
        if (!tmp_file.is_open()) {
            cerr << "Error: Could not open temp file for plotting" << endl;
            return;
        }

        for (int i = -N; i <= N; i++) {
            int idx = i + N;
            if (idx < beat_caar.size())
                tmp_file << i << " " << beat_caar[idx]*100 << " " << meet_caar[idx]*100 << " " << miss_caar[idx]*100 << endl;
        }
        tmp_file.close();

        fprintf(gnuplot_pipe, "set terminal qt size 800,600\n");
        fprintf(gnuplot_pipe, "set terminal qt font 'Helvetica,10'\n");
        fprintf(gnuplot_pipe, "set title 'CAAR for Beat, Meet, and Miss Groups'\n");
        fprintf(gnuplot_pipe, "set xlabel 'Days Relative to Earnings Announcement'\n");
        fprintf(gnuplot_pipe, "set ylabel 'Cumulative Average Abnormal Return (CAAR %%)'\n");
        fprintf(gnuplot_pipe, "set arrow from 0, graph 0 to 0, graph 1 nohead lw 1 lc rgb 'black'\n");
        fprintf(gnuplot_pipe, "set grid\n");
        fprintf(gnuplot_pipe, "plot '%s' using 1:2 with lines linecolor rgb 'red' title 'Beat', \
                       '%s' using 1:3 with lines linecolor rgb 'blue' title 'Meet', \
                       '%s' using 1:4 with lines linecolor rgb 'green' title 'Miss'\n",
        tmp_path.c_str(), tmp_path.c_str(), tmp_path.c_str());
        
        fprintf(gnuplot_pipe, "set key left top\n");
        fflush(gnuplot_pipe);

    }
};

} // namespace fre

#endif