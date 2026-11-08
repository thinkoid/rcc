define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %a.2 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    %t1 = load i32, ptr %argc.1, align 4
    %t2 = freeze i32 %t1
    %t3 = add i32 2147483646, %t2
    store i32 %t3, ptr %a.2, align 4
    %t4 = load i32, ptr %a.2, align 4
    %t5 = freeze i32 %t4
    %t6 = add i32 %t5, 1
    %t7 = load i32, ptr %a.2, align 4
    %t8 = freeze i32 %t7
    %t9 = icmp sgt i32 %t6, %t8
    %t10 = zext i1 %t9 to i32
    ret i32 %t10

after.ret.1:
    ret i32 0
}
