for file in 'cordic_fast' 'des_perf_fast' 'DMA_fast' 'edit_dist_fast' 'fft_fast' 'matrix_mult_fast' 'pci_bridge32_fast' 'usb_phy_fast' 'vga_lcd_fast'
do
    mv $file.adj $file.hgr
    mv $file.typ $file.type
done
