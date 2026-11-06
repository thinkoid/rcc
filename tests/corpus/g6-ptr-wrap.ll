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
    %t3 = sub i32 0, 1
    %t4 = sext i32 %t3 to i64
    store i64 %t4, ptr %ones.5, align 8
    %t5 = load i64, ptr %ones.5, align 8
    %t6 = freeze i64 %t5
    %t7 = load i32, ptr %argc.2, align 4
    %t8 = freeze i32 %t7
    %t9 = add i32 %t8, 2
    %t10 = sext i32 %t9 to i64
    %t11 = lshr i64 %t6, %t10
    store i64 %t11, ptr %n.6, align 8
    %t12 = load ptr, ptr %p.4, align 8
    %t13 = freeze ptr %t12
    %t14 = load ptr, ptr %p.4, align 8
    %t15 = freeze ptr %t14
    %t16 = load i64, ptr %n.6, align 8
    %t17 = freeze i64 %t16
    %t18 = getelementptr ptr, ptr %t15, i64 %t17
    %t19 = icmp ult ptr %t13, %t18
    %t20 = zext i1 %t19 to i32
    ret i32 %t20

after.ret.1:
    ret i32 0
}
