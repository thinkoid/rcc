define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %a.2 = alloca i32, align 4
    %b.3 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    %t1 = sext i32 1 to i64
    %t2 = getelementptr i32, ptr %a.2, i64 %t1
    %t3 = icmp eq ptr %t2, %b.3
    %t4 = zext i1 %t3 to i32
    %t5 = sext i32 1 to i64
    %t6 = getelementptr i32, ptr %b.3, i64 %t5
    %t7 = icmp eq ptr %t6, %a.2
    %t8 = zext i1 %t7 to i32
    %t9 = add i32 %t4, %t8
    ret i32 %t9

after.ret.1:
    ret i32 0
}
