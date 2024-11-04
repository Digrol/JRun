
rm ~buildX -r -f
mkdir ~buildX

pushd ~buildX
cmake -S ..
cmake --build .
popd
