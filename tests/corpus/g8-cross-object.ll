define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %a.2 = alloca i32, align 4
    %b.3 = alloca i32, align 4
    %d.4 = alloca i64, align 8
    %pa.5 = alloca ptr, align 8
    store i32 %p.0, ptr %argc.1, align 4
    store i32 1, ptr %a.2, align 4
    store i32 2, ptr %b.3, align 4
    %t1 = ptrtoint ptr %b.3 to i64
    %t2 = ptrtoint ptr %a.2 to i64
    %t3 = sub i64 %t1, %t2
    store i64 %t3, ptr %d.4, align 8
    store ptr %a.2, ptr %pa.5, align 8
    %t4 = load ptr, ptr %pa.5, align 8
    %t5 = freeze ptr %t4
    %t6 = load i64, ptr %d.4, align 8
    %t7 = freeze i64 %t6
    %t8 = getelementptr i8, ptr %t5, i64 %t7
    store i32 3, ptr %t8, align 1
    %t9 = load i32, ptr %b.3, align 4
    %t10 = freeze i32 %t9
    ret i32 %t10

after.ret.1:
    ret i32 0
}
