// https://stackoverflow.com/questions/43230542/write-vectorvectorfloat-to-binary-file
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <Rcpp.h>

using namespace Rcpp;
using namespace std;

typedef std::vector<float> Text;
typedef std::vector<Text> Texts;

void write_vector(std::string path, const vector<vector<float> >& vec)
{
    std::ofstream FILE(path, std::ios::out | std::ofstream::binary);
    
    // Store size of the outer vector
    int s1 = vec.size();
    FILE.write(reinterpret_cast<const char *>(&s1), sizeof(s1));    
    
    // Now write each vector one by one
    for (auto& v : vec) {         
        // Store its size
        int size = v.size();
        FILE.write(reinterpret_cast<const char *>(&size), sizeof(size));
        
        // Store its contents
        FILE.write(reinterpret_cast<const char *>(&v[0]), v.size()*sizeof(float));
    }
    FILE.close();   
}

void read_vector(std::string path,  vector<vector<float> >& vec)
{
    ifstream FILE(path, std::ios::in | std::ifstream::binary);
    
    int size = 0;
    FILE.read(reinterpret_cast<char *>(&size), sizeof(size));
    vec.resize(size);
    for (int n = 0; n < size; ++n) {
        int size2 = 0;
        FILE.read(reinterpret_cast<char *>(&size2), sizeof(size2));
        float f;        
        for ( int k = 0; k < size2; ++k ) {
            FILE.read(reinterpret_cast<char *>(&f), sizeof(f));
            vec[n].push_back(f);   
        }
    }
}

// [[Rcpp::export]]
List test2(List in_) {
    
    Texts toks_in = Rcpp::as<Texts>(in_);
    write_vector("test2.dat", toks_in);
    
    std::vector<std::vector<float>> toks_out;
    read_vector("test2.dat", toks_out);
    
    List out_ = Rcpp::wrap(toks_out);
    return(out_);
}

/*** R
lis <- list(c(1, 2, 3, 4), c(10, 11, 12, 13))
test2(lis)
*/

