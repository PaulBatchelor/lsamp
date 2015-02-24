cp combine_test.smp.bk combine_test.smp
cp combine_test.smp.tmp.bk combine_test.smp.tmp
./lsamp_combine
if [ "$(diff combine_test.smp combine_test_ref.smp)" != "" ]
then
    echo "FAILED: the files are different!"
else
    echo "PASSED: the files are the same!"
fi
rm -f combine_test.smp.tmp combine_test.smp
