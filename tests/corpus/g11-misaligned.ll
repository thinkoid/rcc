define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %c.2 = alloca [8 x i8], align 1
    %i.3 = alloca i32, align 4
    %v.4 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    store i32 0, ptr %i.3, align 4
    br label %for.cond.1

for.cond.1:
    %t1 = load i32, ptr %i.3, align 4
    %t2 = freeze i32 %t1
    %t3 = icmp slt i32 %t2, 8
    %t4 = zext i1 %t3 to i32
    %t5 = icmp ne i32 %t4, 0
    br i1 %t5, label %for.body.1, label %for.end.1

for.body.1:
    %t6 = load i32, ptr %i.3, align 4
    %t7 = freeze i32 %t6
    %t8 = sext i32 %t7 to i64
    %t9 = getelementptr i8, ptr %c.2, i64 %t8
    %t10 = load i32, ptr %i.3, align 4
    %t11 = freeze i32 %t10
    %t12 = load i32, ptr %argc.1, align 4
    %t13 = freeze i32 %t12
    %t14 = add i32 %t11, %t13
    %t15 = trunc i32 %t14 to i8
    store i8 %t15, ptr %t9, align 1
    br label %for.step.1

for.step.1:
    %t16 = load i32, ptr %i.3, align 4
    %t17 = freeze i32 %t16
    %t18 = add i32 %t17, 1
    store i32 %t18, ptr %i.3, align 4
    br label %for.cond.1

for.end.1:
    %t19 = getelementptr i8, ptr %c.2, i64 1
    %t20 = load i32, ptr %t19, align 1
    %t21 = freeze i32 %t20
    store i32 %t21, ptr %v.4, align 4
    %t22 = load i32, ptr %v.4, align 4
    %t23 = freeze i32 %t22
    %t24 = ashr i32 %t23, 24
    %t25 = mul i32 %t24, 10
    %t26 = load i32, ptr %v.4, align 4
    %t27 = freeze i32 %t26
    %t28 = and i32 %t27, 255
    %t29 = add i32 %t25, %t28
    ret i32 %t29

after.ret.2:
    ret i32 0
}
