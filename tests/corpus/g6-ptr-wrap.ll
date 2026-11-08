define i32 @main(i32 %p.0, ptr %p.1) null_pointer_is_valid {
entry:
    %argc.2 = alloca i32, align 4
    %argv.3 = alloca ptr, align 8
    %p.4 = alloca ptr, align 8
    %ones.5 = alloca i64, align 8
    %n.6 = alloca i64, align 8
    store i32 %p.0, ptr %argc.2, align 4
    store ptr %p.1, ptr %argv.3, align 8
    %t1 = load ptr, ptr %argv.3, align 8
    %t2 = freeze ptr %t1
    store ptr %t2, ptr %p.4, align 8
    store i64 -1, ptr %ones.5, align 8
    %t3 = load i64, ptr %ones.5, align 8
    %t4 = freeze i64 %t3
    %t5 = load i32, ptr %argc.2, align 4
    %t6 = freeze i32 %t5
    %t7 = add i32 %t6, 2
    %t8 = sext i32 %t7 to i64
    %t9 = lshr i64 %t4, %t8
    store i64 %t9, ptr %n.6, align 8
    %t10 = load ptr, ptr %p.4, align 8
    %t11 = freeze ptr %t10
    %t12 = load ptr, ptr %p.4, align 8
    %t13 = freeze ptr %t12
    %t14 = load i64, ptr %n.6, align 8
    %t15 = freeze i64 %t14
    %t16 = getelementptr ptr, ptr %t13, i64 %t15
    %t17 = icmp ult ptr %t11, %t16
    %t18 = zext i1 %t17 to i32
    ret i32 %t18

after.ret.1:
    ret i32 0
}
