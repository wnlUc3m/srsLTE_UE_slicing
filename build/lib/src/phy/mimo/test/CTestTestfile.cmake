# CMake generated Testfile for 
# Source directory: /home/ggarcia/srsLTE_multipleUE/lib/src/phy/mimo/test
# Build directory: /home/ggarcia/srsLTE_multipleUE/build/lib/src/phy/mimo/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(layermap_single "layermap_test" "-n" "1000" "-m" "single" "-c" "1" "-l" "1")
ADD_TEST(layermap_diversity_2 "layermap_test" "-n" "1000" "-m" "diversity" "-c" "1" "-l" "2")
ADD_TEST(layermap_diversity_4 "layermap_test" "-n" "1000" "-m" "diversity" "-c" "1" "-l" "4")
ADD_TEST(layermap_multiplex_11 "layermap_test" "-n" "1000" "-m" "multiplex" "-c" "1" "-l" "1")
ADD_TEST(layermap_multiplex_12 "layermap_test" "-n" "1000" "-m" "multiplex" "-c" "1" "-l" "2")
ADD_TEST(layermap_multiplex_13 "layermap_test" "-n" "1002" "-m" "multiplex" "-c" "1" "-l" "3")
ADD_TEST(layermap_multiplex_14 "layermap_test" "-n" "1000" "-m" "multiplex" "-c" "1" "-l" "4")
ADD_TEST(layermap_multiplex_22 "layermap_test" "-n" "1000" "-m" "multiplex" "-c" "2" "-l" "2")
ADD_TEST(layermap_multiplex_23 "layermap_test" "-n" "1002" "-m" "multiplex" "-c" "2" "-l" "3")
ADD_TEST(layermap_multiplex_24 "layermap_test" "-n" "1000" "-m" "multiplex" "-c" "2" "-l" "4")
ADD_TEST(precoding_single "precoding_test" "-n" "1000" "-m" "single")
ADD_TEST(precoding_diversity2 "precoding_test" "-n" "1000" "-m" "diversity" "-l" "2" "-p" "2")
ADD_TEST(precoding_diversity4 "precoding_test" "-n" "1024" "-m" "diversity" "-l" "4" "-p" "4")
