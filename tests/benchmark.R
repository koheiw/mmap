require(quanteda)
toks <- tokens(data_corpus_inaugural)
write_vector("tests/tokens.dat", unclass(toks))
writeLines(data_corpus_inaugural, "tests/corpus.txt")
saveRDS(data_corpus_inaugural, "tests/corpus.rds", compress = FALSE)

microbenchmark::microbenchmark(
    read_vector_memory(unclass(toks)),
    read_vector_file("tests/tokens.dat"),
    read_vector_mmap("tests/tokens.dat"),
    times = 10)
