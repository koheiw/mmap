// https://stackoverflow.com/questions/17104951/mmap-reading-stale-array-of-integers-from-an-file
// https://mkguytone.github.io/allocator-navigatable/ch73.html
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <Rcpp.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace Rcpp;
using namespace std;

typedef std::vector<int> Text;
typedef std::vector<Text> Texts;

// [[Rcpp::export]]
void write_vector(std::string path, const List texts_) {
    
    Texts texts = Rcpp::as<Texts>(texts_);
    std::ofstream file_out(path, std::ios::out | std::ofstream::binary);
    
    // Now write each vector one by one
    for (auto text : texts) {         
        text.push_back(-1);
        file_out.write(reinterpret_cast<const char *>(&text[0]), text.size() * sizeof(int));
    }
    file_out.close();   
}

// [[Rcpp::export]]
void read_vector_memory(const List texts_) {
    
    for (size_t h = 0; h < texts_.size(); h++) {
        Text text_ = texts_[h];
        for (size_t i = 0; i < text_.size(); i++) {      
            if (text_[i] >= 0) {
                Rcout << text_[i] << " ";
            } else {
                Rcout << "\n";
            }
        }
    }
  
}

// [[Rcpp::export]]
void read_vector_file(std::string path) {
    
    ifstream file_in(path, std::ios::in | std::ifstream::binary);
    
    if (!file_in) 
        throw std::invalid_argument("Cannot find " + path);
    
    while (true) {

        int val;
        file_in.read(reinterpret_cast<char *>(&val), sizeof(int));
        if (file_in.eof())
            break;
            
        if (val >= 0) {
            Rcout << val << " ";
        } else {
            Rcout << "\n";
        }
    }
    file_in.close();   
}

// [[Rcpp::export]]
void read_vector_mmap(std::string path) {
    
    int file_in = open(path.c_str(), O_RDONLY, 0);
    //posix_fallocate(file_in, 0, 4096);
    
    if (!file_in) 
        throw std::invalid_argument("Cannot find " + path);
        
    struct stat st;
    fstat(file_in, &st);
    
    size_t len_page = getpagesize();
    //Rprintf("page size: %d\n", len_page);
    //Rprintf("file size: %d\n", st.st_size);

    int *buffer = (int *)reinterpret_cast<char *> (mmap(NULL, len_page, PROT_READ, MAP_PRIVATE, file_in, 0));

    if (buffer == MAP_FAILED) {
        throw logic_error("Error mmapping the file");
    }
    for (int i = 0; i < st.st_size / sizeof(int); i++) {
        if (buffer[i] >= 0) {
            Rcout << buffer[i] << " ";
        } else {
            Rcout << "\n";
        }
    }
    munmap(buffer, len_page);

}

/*** R
#lis <- rep(list(1:1000), 10000)
lis <- list(c(1, 2, 3, 4, 5))

write_vector("src/test.dat", lis)
system("od -td src/test.dat")
read_vector_file("src/test.dat")
read_vector_mmap("src/test.dat")
# microbenchmark::microbenchmark(
#     read_vector_memory(lis),
#     read_vector_file("src/test.dat"),
#     #read_vector_mmap("src/test.dat"),
#     times = 1)
*/

