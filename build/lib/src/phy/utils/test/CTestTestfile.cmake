# CMake generated Testfile for 
# Source directory: /home/ggarcia/srsLTE_multipleUE/lib/src/phy/utils/test
# Build directory: /home/ggarcia/srsLTE_multipleUE/build/lib/src/phy/utils/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(dft_test "dft_test")
ADD_TEST(dft_reverse "dft_test" "-b")
ADD_TEST(dft_mirror "dft_test" "-m")
ADD_TEST(dft_norm "dft_test" "-n")
ADD_TEST(dft_dc "dft_test" "-b" "-d")
ADD_TEST(dft_odd "dft_test" "-N" "255")
ADD_TEST(dft_odd_dc "dft_test" "-N" "255" "-b" "-d")
