
a_hit_onehot0: assert property (@(posedge clk) disable iff (!rst_n)
    $onehot0(hit_way))
    else $error("hit_way not one_hot_or_zero: %b", hit_way);



a_accounting: assert property (@(posedge clk) disable iff (!rst_n)
    n_hitsTEMP+n_missesTEMP + 32'(state==LOOKUP) == n_requestsTEMP)
    else $error("accounting: hits=%0d misses=%0d req=%0d state=%s",
                n_hitsTEMP,n_missesTEMP,n_requestsTEMP,state.name());



a_miss_memreq: assert property (@(posedge clk) disable iff (!rst_n)
            n_missesTEMP == n_mem_reqsTEMP)
            else $error("misses=%0d mem_reqs=%0d", n_missesTEMP, n_mem_reqsTEMP);
