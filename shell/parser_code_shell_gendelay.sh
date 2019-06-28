#for file in 'cordic_fast' 'cordic_slow' 'des_perf_fast' 'des_perf' 'des_perf_slow' 'edit_dist_fast' 'edit_dist_slow' 'fft_fast' 'fft_slow' 'matrix_mult_fast' 'matrix_mult_slow' 'netcard' 'simple' 'usb_phy' 'netcard_fast' 'netcard_slow' 'pci_bridge32_fast' 'pci_bridge32' 'pci_bridge32_slow' 'usb_phy_fast' 'usb_phy_slow'
for file in 'c17'  'usb_phy_fast'  'cordic_fast' 'des_perf_fast' 'DMA_fast' 'edit_dist_fast' 'fft_fast' 'matrix_mult_fast' 'pci_bridge32_fast' 'usb_phy_fast' 'vga_lcd_fast'
do
    echo $file
   ./delay.out $file.hgr $file.type $file.fanout $file.fanin contest_lvt.cellrise contest_lvt.cellfall contest_lvt.cellrisetransition contest_lvt.cellfalltransition contest_rvt.cellrise contest_rvt.cellfall contest_rvt.cellrisetransition contest_rvt.cellfalltransition   contest_hvt.cellrise contest_hvt.cellfall contest_hvt.cellrisetransition contest_hvt.cellfalltransition contest_rvt.capacitance contest_rvt.parity >& $file.delaydump

done
