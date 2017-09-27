
I have run this code on gcc version - gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.3).

Argument Parameters Name:
1). -train <train_srt_file_name> : (required) It needs srt file path of training data. Example: /home/bhaskar/work/LanguageModel/Proteomes/Nomascus_leucogenys/Nomascus_leucogenys.fa.srt

2). -test <test_srt_file_name> : (required) It needs srt file path of testing data. Example: /home/bhaskar/work/LanguageModel/Proteomes/Nomascus_leucogenys/Nomascus_leucogenys.fa.srt

3). -nGram <nGram_Value>: (required) It needs nGramValue. Example: -nGram 4 

4).-trainNgram <output_train_nGram_File_Name>: (optional) It needs text file path for printing all ngrams of train data. Example: /home/bhaskar/work/LanguageModel/Proteomes/Bactrocera_oleae/ngramout.txt

5).-testNgram <output_test_nGram_File_Name>: (optional) It needs text file path for printing all ngrams of test data. Example: /home/bhaskar/work/LanguageModel/Proteomes/Bactrocera_oleae/ngramout.txt


 Command(Compile) :  gcc <file_name> -lm
For example:  gcc lm.c -lm


 Command(Run) :  ./a.out -test <test_srt_file_name> -train <train_srt_file_name> -nGram 4 -trainNgram <output_train_nGram_File_Name> -testNgram <output_test_nGram_File_Name>

Note - -trainNgram <output_train_nGram_File_Name> -testNgram <output_test_nGram_File_Name> are optional parameter. It needs to be given only if nGram output is desired.

For example:  ./a.out -test /home/bhaskar/Bhaskar/work/LanguageModel/Proteomes/Nomascus_leucogenys/Nomascus_leucogenys.fa.srt -train /home/bhaskar/Bhaskar/work/LanguageModel/Proteomes/Bactrocera_oleae/Bactrocera_oleae.protein.fa.srt -nGram 4 -trainNgram /home/bhaskar/Bhaskar/work/LanguageModel/Proteomes/Bactrocera_oleae/ngramout.txt -testNgram /home/bhaskar/Bhaskar/work/LanguageModel/Proteomes/Nomascus_leucogenys/nGramout.txt



Acknowledgment:
I would like to thank Dr.M.Ganapathiraju for her guidance in completing the project.

References:
1. https://bmcbioinformatics.biomedcentral.com/articles/10.1186/1471-2105-12-12
2. https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3602881/