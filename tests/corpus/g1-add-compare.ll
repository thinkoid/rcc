define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %a.2 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    %t1 = sub i32 2147483647, 1
    %t2 = load i32, ptr %argc.1, align 4
    %t3 = freeze i32 %t2
    %t4 = add i32 %t1, %t3
    store i32 %t4, ptr %a.2, align 4
    %t5 = load i32, ptr %a.2, align 4
    %t6 = freeze i32 %t5
    %t7 = add i32 %t6, 1
    %t8 = load i32, ptr %a.2, align 4
    %t9 = freeze i32 %t8
    %t10 = icmp sgt i32 %t7, %t9
    %t11 = zext i1 %t10 to i32
    ret i32 %t11

after.ret.1:
    ret i32 0
}
