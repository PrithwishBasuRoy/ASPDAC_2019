for file_name in 'delay'
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
$exec$new_fileName $bmark/aes_128.hgr $bmark/aes_128.type $bmark/lvt_gate_footmap $bmark/hvt_gate_footmap $bmark/lvt_cell_rise_rec $bmark/lvt_cell_fall_rec $bmark/lvt_rise_transition_rec $bmark/lvt_fall_transition_rec $bmark/hvt_cell_rise_rec  $bmark/hvt_cell_fall_rec  $bmark/hvt_rise_transition_rec  $bmark/hvt_fall_transition_rec $bmark/capacitance_rev $bmark/lvt_parity_rec > $dump/$file_name$current_time.dump 
done
#./delay aes_128.hgr aes_128.type lvt_gate_footmap hvt_gate_footmap lvt_cell_rise_rec lvt_cell_fall_rec lvt_rise_transition_rec lvt_fall_transition_rec hvt_cell_rise_rec  hvt_cell_fall_rec  hvt_rise_transition_rec  hvt_fall_transition_rec capacitance_rev lvt_parity_rec > delay_dump

