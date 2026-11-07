define i32 @main() null_pointer_is_valid {
entry:
    %x.0 = alloca i32, align 4
    %t1 = load i32, ptr %x.0, align 4
    %t2 = freeze i32 %t1
    %t3 = freeze i32 %t2
    %t4 = icmp eq i32 %t2, %t3
    %t5 = zext i1 %t4 to i32
    ret i32 %t5

after.ret.1:
    ret i32 0
}
