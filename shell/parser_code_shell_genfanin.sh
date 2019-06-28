#:for file in  'b14' 'b15' 'b17' 'b18' 'b19'
#for file in 'cordic_fast' 'des_perf_fast' 'DMA_fast' 'edit_dist_fast' 'fft_fast' 'matrix_mult_fast' 'pci_bridge32_fast' 'usb_phy_fast' 'vga_lcd_fast'

for file in 'aes_128'
do
#    ./fanin.out $file.hgr
    echo $file | python ../python/fanin.py
#./delay.out $file.hgr $file.type 90nm_lvt.cellrise 90nm_lvt.cellfall 90nm_lvt.cellrisetransition 90nm_lvt.cellfalltransition 90nm_rvt.cellrise 90nm_rvt.cellfall 90nm_rvt.cellrisetransition 90nm_rvt.cellfalltransition   90nm_hvt.cellrise 90nm_hvt.cellfall 90nm_hvt.cellrisetransition 90nm_hvt.cellfalltransition 90nm_rvt.capacitance 90nm_rvt.parity

done
