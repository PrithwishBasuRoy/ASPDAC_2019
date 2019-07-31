g++ -g -std=c++0x vt_gate_replacemet_ista.cpp -o a.out
./a.out aes_128.hgr aes_128.type aes_128.fanout aes_128.fanin lvt_gate_lookup hvt_gate_lookup aes_128.rise_m_02_02_5_5 aes_128.fall_m_02_02_5_5 hvt_cell_leakage_power lvt_cell_leakage_power parity_combined > dump

