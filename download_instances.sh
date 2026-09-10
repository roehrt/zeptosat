wget --content-disposition -nc -i instances.uri -P tests
for file in tests/*.tar.gz; do
    tar -xvzf $file -C tests
done