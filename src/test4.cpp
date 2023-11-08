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
        file_out.write(reinterpret_cast<const char *>(&text[0]), text.size()*sizeof(int));
    }
    file_out.close();   
}

// [[Rcpp::export]]
void read_vector(std::string path) {
    
    ifstream file_in(path, std::ios::in | std::ifstream::binary);
    
    while (!file_in.eof()) {
        int val;
        file_in.read(reinterpret_cast<char *>(&val), sizeof(val));
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
    
    struct stat st;
    fstat(file_in, &st);
    
    size_t len = getpagesize();
    Rprintf("page size: %d\n", len);
    Rprintf("file size: %d\n", st.st_size);

    int *buffer = (int *)reinterpret_cast<char *> (mmap(NULL, len, PROT_READ, MAP_PRIVATE, file_in, 0));
    //char *buffer = (char *)reinterpret_cast<char *> (mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, file_in, 0));
    //int *buffer = (int *)mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, file_in, 0);
    
    if (buffer == MAP_FAILED) {
        perror("Error mmapping the file");
    }
    for (int i = 0; i < st.st_size / sizeof(int); i++) {
        if (buffer[i] >= 0) {
            Rcout << buffer[i] << " ";
        } else {
            Rcout << "\n";
        }
    }
    munmap(buffer, len);

}

/*** R
lis <- list(c(1, 2, 3, 4), c(10:100))
#lis <- list(c(1, 2, 3, 4, 5))
write_vector("test.dat", lis)
read_vector("test.dat")
read_vector_mmap("test.dat")
*/

