define i32 @main() null_pointer_is_valid {
entry:
    %x.0 = freeze i32 undef
    %t1 = icmp eq i32 %x.0, %x.0
    %t2 = zext i1 %t1 to i32
    ret i32 %t2

after.ret.1:
    ret i32 0
}
