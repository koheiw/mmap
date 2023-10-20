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

void write_vector(std::string path, const Texts &texts) {
    
    std::ofstream file_out(path, std::ios::out | std::ofstream::binary);
    
    // Store size of the outer vector
    //int s1 = vec.size();
    //FILE.write(reinterpret_cast<const char *>(&s1), sizeof(s1));    
    
    // Now write each vector one by one
    for (auto text : texts) {         
        text.push_back(-1);
        file_out.write(reinterpret_cast<const char *>(&text[0]), text.size()*sizeof(float));
    }
    file_out.close();   
}

void read_vector(std::string path,  vector<vector<float> >& vec) {
    ifstream file_in(path, std::ios::in | std::ifstream::binary);
    
    while(!file_in.eof()) { // how can I find the end of the file?
        //for (size_t i = 0; i < 10; i++) {
        float f;
        file_in.read(reinterpret_cast<char *>(&f), sizeof(f));
        if (f > 0) {
            Rcout << f << " ";
        } else {
            Rcout << "\n";
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
lis <- list(c(1, 2, 3, 4), c(10:100))
test2(lis)
*/

