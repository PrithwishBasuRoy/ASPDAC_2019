for file in '../benchmarks/aes_128_synthesized.v'
#'netcard_fast_gate_list.v' 'netcard_slow_gate_list.v' 'pci_bridge32_fast_gate_list.v' 'pci_bridge32_gate_list.v' 'pci_bridge32_slow_gate_list.v' 'usb_phy_fast_gate_list.v' 'usb_phy_slow_gate_list.v'
#for file in 'cordic_fast_gate_list.v' 'cordic_slow_gate_list.v' 'des_perf_fast_gate_list.v' 'des_perf_gate_list.v' 'des_perf_slow_gate_list.v' 'edit_dist_fast_gate_list.v' 'edit_dist_slow_gate_list.v' 'fft_fast_gate_list.v' 'fft_slow_gate_list.v' 'matrix_mult_fast_gate_list.v' 'matrix_mult_slow_gate_list.v' 'netcard_gate_list.v' 'simple_gate_list.v' 'usb_phy_gate_list.v'
do
    ./parser_code_shell_rewrite.sh $file
done

