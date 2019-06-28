#for file in 'aes_128'
#do
#file=$1
#echo $file
#    ./gate_netlist.out verilog $file hgr
#done


for file_name in 'gate_netlist'
do
echo $file_name
file_to_compile='parameterized_input_slew.cpp'
current_time=$(date "+%Y_%m_%d_%H_%M_%S")
echo "Current Time : $current_time"
out='out'
cpp='../cpp/'
exec='../exec/'
bmark='../benchmarks'
dump='../dumps'
#f_name=''
temp="$exec$file_name$current_time"
new_fileName="$temp.$out" 
#cp $file_name $new_fileName
echo $new_fileName
comp="$cpp$file_to_compile"
echo $comp
#echo $bmark/lvt_gate_footmap
g++ -std=c++0x $comp -o $new_fileName

$exec$new_fileName verilog aes_128_synthesized.v hgr > $dump/$file_name$current_time.dump
done

