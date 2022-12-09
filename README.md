# Instructions
cmake --build .
./cachesim

# Unit Tests
git clone https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
sudo cmake --build build/ --target install 

# Error Handling
in case of cmake error "Cmake Error: could not load cache", run "cmake --configure ."