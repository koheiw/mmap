#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <Rcpp.h>
using namespace Rcpp;

// https://unix.stackexchange.com/questions/735436/trying-to-understand-memory-mapping-with-c-in-linux-environment
// [[Rcpp::export]]
void test_modify() {
    // reading
    //int file_read = open("test.dat", O_RDONLY, 0);
    // writing
    int file_write = open("test.dat", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
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

// [[Rcpp::export]]
void test_read() {
    // reading
    int file_read = open("test.dat", O_RDONLY, 0);
    //Rcout << "file_write" << file_write << "\n";
    posix_fallocate(file_read, 0, 4096);
    
    int *buffer = (int *)reinterpret_cast<char*> (mmap(NULL, 4096, PROT_READ, MAP_SHARED, file_read, 0));
    
    if (buffer == MAP_FAILED) {
        perror("Error mmapping the file");
    }
    for (int i = 0; i < 4096; i++) {
        Rcout << "" << buffer[i] << "\n";
    }
}

// https://stackoverflow.com/questions/67039971/faster-way-to-write-an-integer-vector-to-a-binary-file-with-c
// [[Rcpp::export]]
void test_write() {
    std::vector< std::vector<int> > v = {{1, 2, 3, 4}, {11, 12, 13, 14}};
    std::ofstream out("test.dat", std::ios_base::binary);
    uint64_t size = v.size();
    //out.write(reinterpret_cast<char*>(&size), sizeof(size)); // header
    out.write(reinterpret_cast<char*>(v.data()), size * sizeof(int));
}

/*** R
#test_modify()
test_write()
test_read()
*/

