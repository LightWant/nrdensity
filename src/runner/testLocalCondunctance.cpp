#include "../graph/graph.h"
#include "../tools/getArgs.hpp"
#include "../tools/types.hpp"
// #include "../lads/ladsDinic.h"
// #include "../lads/ladsHLPP.h"
// #include "../lads/lads.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <set>
#include <cassert>

void printUsage() {
    std::cout << "-f graph file directory(edge.bin & idx.bin)" << std::endl;
    std::cout << "-f_txt graph file text file, each edge exists one time" << std::endl;
    std::cout << "-f_txtD graph file text file, each edge exists two times" << std::endl;
}

#include <sys/types.h>
#include <dirent.h>
#include <string>
using std::string;
using std::vector;

bool endsWith(const string & s, const string & sub){
    if(s.length() < sub.length()) return false;
    if(sub.length() == 0) return true;
    int i = s.length()-1, j = sub.length()-1;
    while(j >= 0 && sub[j] == s[i]) {
        --i; --j;
    }
    return j == -1;
}

void loadImagePath(string imgDirPath,
    vector<string> &vimgPath, const string & ed)
{
	DIR *pDir;
    struct dirent* ptr;
    if(!(pDir = opendir(imgDirPath.c_str())))
    {
        std::cout<<"Folder doesn't Exist!"<<std::endl;
        return;
    }

    string prefix = "/";
    if(imgDirPath[imgDirPath.length()-1] == '/') {
        prefix = "";
    }
// printf("here\n");fflush(stdout);
    while((ptr = readdir(pDir))!=0) 
    {
        if(!endsWith(ptr->d_name, ed)) continue;
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
        {
            vimgPath.push_back(imgDirPath + prefix + ptr->d_name);
    	}
    }
    sort(vimgPath.begin(),vimgPath.end());
// printf("here\n");fflush(stdout);
    closedir(pDir);
}

char s[1000000];
void getS(string & fp, vector<ui> & S) {
    FILE * f = fopen(fp.c_str(), "r");
    
    auto readline = [&]()->int {
        ui l = 0;
        while(true) {
            s[l] = fgetc(f);
            if(s[l] == '\n') {
                s[l] = '\0';
                return l;
            }
            if(s[l] == EOF) return EOF;
            l++;
        }
    };
    while(readline() != EOF) {
        // printf("%s\n", s);
        if(s[0] == 'd' && s[1] == 'e' && s[3] == ':') break;
    }
    fclose(f);
    // std::vector<ui> S;
    S.clear();
    for(ui i = 4, j = 0; s[i]; i++) {
        if('0' <= s[i] && s[i] <= '9') {
            j = j * 10 + s[i]-'0';
        }
        else if(s[i] == ' '){
            S.push_back(j);
            j = 0;
        }
    }
}

void getR(string & frp, vector<ui> & R) {
    std::fstream finR(frp, std::ios::in);
// std::cout << "R_Path:" << ac["-R"] << std::endl;
    ui u;
    R.clear();
    while(finR >> u) {
        // R.insert(u);
        R.push_back(u);
    }
    finR.close();
    std::sort(R.begin(), R.end());
}


int main(int argc, char * argv[]) {
    argsController ac(argc, argv);

// das = [ "act", "ama", "ema", "ski", "you"]
// dataDir = "data/"
// logDir = "logs/exp_raw/"
// algs = ["adsRunner", "localDensity"]

    std::vector<string> logNas = {
        "ama","dbl","epi","wik","pok","you","gow","ber","ski","lj","ork"
    };

    string logDir = "logs/exp_raw/";
    if(ac.exist("-log")) logDir = ac["-log"];
    vector<string> algs = {"adsRunner",  "flowseedsRunner","betalocalDensity", "exactLocalDensity"};

    vector<ui> S, R;

       
    string seedDir = ac["-seed"];

    vector<string> fr;
    loadImagePath(seedDir, fr, ".R");

    Graph g;
    if(ac.exist("-f_txt"))
        g.readFromText(ac["-f_txt"], ac.exist("noUVM"));
    else
        g.readFromBin(ac["-f"]);

// for(ui i = g.pIdx[u]; i < g.pIdx[u+1]; i++) {
//     ui v = g.pEdge[i];
//     printf("%u\n", v);
// }

    std::vector<bool> inVs(g.n);


    // for(auto & da : logNas) {
    string da = ac["-da"];
std::cout << da << std::endl;
        for(auto & alg : algs) {
std::cout << alg << ' ';fflush(stdout);
            string logDirAlg = logDir + alg + "/" + da;
            vector<string> fs;
            loadImagePath(logDirAlg, fs, ".txt");

// assert(fs.size() == fr.size());
            double sumL = 0;
            int cntL  = 0;

            double sumF1 = 0.0;
            int cntF1 = 0;

            double sumRecall = 0.0;
            int cntR = 0;

            double sumC = 0;
            int cntC = 0;
// printf("here\n");fflush(stdout);
// ui j = 0;
            for(ui i = 0; i < fs.size(); i++) {
                string & f = fs[i];
                string frp = fr[i];
auto check = [](const string &f, const string& frp) {
ui ia = 0, ib = 0;
ui ic = 0, id = 0;
ui j = 0;
while(j < f.length()) {
    if(f[j] == '/') ia = j;
    if(f[j] == '.') ib = j;
    j++;
}
j = 0;
while(j < frp.length()) {
    if(frp[j] == '/') ic = j;
    if(frp[j] == '.') id = j;
    j++;
}
if(ib - ia != id - ic) return false;
return f.substr(ia, ib-ia)==frp.substr(ic, id-ic);
};
ui j = 0;
                while(!check(f, frp)) {
                    frp = fr[j++];
                }


                getR(frp, R);
                std::set<ui> Rset(R.begin(), R.end());
                
                auto inR = [&](ui u) {
                    return Rset.find(u) != Rset.end();
                };

                getS(f, S);
                std::fill(inVs.begin(), inVs.end(), false);
                for(ui u : S) inVs[u] = true;
// if(i == 2) {
//     std::cout << f << std::endl;
//     std::cout << frp << std::endl;
//     std::cout << "R:";
//     for(ui u : R) std:: cout << u << ' ';
//     std::cout << std::endl;
//     std::cout << "S:";
//     for(ui u : S) std:: cout << u << ' ';
//     std::cout << std::endl;
// }

//F1 score
                double SInterR = 0;
                for(ui u  : R) if(inVs[u]) SInterR++;
                
                sumF1 +=  SInterR / (R.size() + S.size() - SInterR);
                sumRecall += SInterR / R.size();
                cntR++;
// printf("%f %u %u\n", 2.0 * SInterR, R.size(), S.size());
                cntF1 ++;
//local conduntance, epsilon=0.1, pu=0
                double ESShat = 0;
                double volRS = 0;
                double volSRhat = 0;
                double volS = 0;
// printf("here2\n");fflush(stdout);
                for(ui u : S) {
// printf("here4 %u %u %u\n", u, g.n, g.degree(u));fflush(stdout);
                    for(ui i = g.pIdx[u]; i < g.pIdx[u+1]; i++) {
                        ui v = g.pEdge[i];

                        if(!inVs[v]) {
                            ESShat++;
                        }
                    }
// printf("here5\n");fflush(stdout);
                    if(inR(u)) volRS += g.degree(u);
                    else volSRhat += g.degree(u);

                    volS += g.degree(u);
                }
// printf("here3\n");fflush(stdout);
// printf("localCondunctance:%f\n", 1.0*ESShat/(volRS - 0.1*volSRhat));        
                if(std::abs(volRS - 1.0*volSRhat) < 1e-5) {
// std::cout << f << ' ' << frp << std::endl;
    // printf("fs %lu,", fs.size());
    // printf("fr %lu,", fr.size());
// printf("%f %f %f %f\n", ESShat, volRS, volSRhat, volS);
                    // sumL += ESShat;
                }
                else {
                    sumL += 1.0*ESShat/(volRS - 1.0*volSRhat);
                    cntL ++;
                }
                
                if(volS == 0) continue;
                sumC += ESShat / std::min(volS, g.m - volS);
                cntC++;
// std::cout << i << ' ' <<  ESShat / std::min(volS, g.m - volS) << '\n';
            }
// printf("here\n");fflush(stdout);
if(cntL == 0 || cntF1 == 0 || cntC == 0) {
printf("\nerror\n");
}
std::cout << sumL / cntL << ' ' << sumF1 / cntF1 << ' '<< sumC/cntC << ' '<<sumRecall/cntR << '\n';
// std::cout << sumC/cntC << '\n';
            // printf("localCondunctance:%f\n", sumL / cntL);
        }
std::cout << std::endl;
    // }
    // for(auto & f : fs) std::cout << f << std::endl;


    

    
    return 0;
} 