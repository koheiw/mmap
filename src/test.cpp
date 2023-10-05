#include <Rcpp.h>
using namespace Rcpp;

#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <iostream>

// https://unix.stackexchange.com/questions/735436/trying-to-understand-memory-mapping-with-c-in-linux-environment
// [[Rcpp::export]]
void test() {
    // reading
    int file_read = open("test.dat", O_RDONLY, 0);
    // writing
    int file_write = open("test.dat", O_CREAT | O_RDWR,
                          S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    //Rcout << "file_write" << file_write << "\n";
    posix_fallocate(file_write, 0, 4096);
    
    char *buffer = reinterpret_cast<char*> (mmap(NULL, 4096, PROT_WRITE, MAP_SHARED, file_write, 0));
    
    for (int i = 0; i < 4096; i++) {
        // Change each upper-case A to lowercase in the file
        //Rcout << "" << buffer[i] << "\n";
        if (buffer[i] == 'A') {
            buffer[i] = 'a';
        }
    }
}

/*** R
test()
*/

