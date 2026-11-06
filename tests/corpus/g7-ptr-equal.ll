define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %a.2 = alloca i32, align 4
    %b.3 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    %t1 = getelementptr i32, ptr %a.2, i64 1
    %t2 = icmp eq ptr %t1, %b.3
    %t3 = zext i1 %t2 to i32
    %t4 = getelementptr i32, ptr %b.3, i64 1
    %t5 = icmp eq ptr %t4, %a.2
    %t6 = zext i1 %t5 to i32
    %t7 = add i32 %t3, %t6
    ret i32 %t7

after.ret.1:
    ret i32 0
}
