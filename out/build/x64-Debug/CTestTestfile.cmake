# CMake generated Testfile for 
# Source directory: C:/Users/Lenovo/Desktop/17373494_杨智茹_02/17373494_杨智茹_mini
# Build directory: C:/Users/Lenovo/Desktop/17373494_杨智茹_02/17373494_杨智茹_mini/out/build/x64-Debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(all_test "miniplc0_test")
set_tests_properties(all_test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Lenovo/Desktop/17373494_杨智茹_02/17373494_杨智茹_mini/CMakeLists.txt;72;add_test;C:/Users/Lenovo/Desktop/17373494_杨智茹_02/17373494_杨智茹_mini/CMakeLists.txt;0;")
subdirs("3rd_party/argparse")
subdirs("3rd_party/fmt")
subdirs("3rd_party/catch2")
