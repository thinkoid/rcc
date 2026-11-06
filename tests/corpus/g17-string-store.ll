@.str.0 = private global [4 x i8] c"abc\00", align 1

define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %s.2 = alloca ptr, align 8
    store i32 %p.0, ptr %argc.1, align 4
    store ptr @.str.0, ptr %s.2, align 8
    %t1 = load ptr, ptr %s.2, align 8
    %t2 = freeze ptr %t1
    %t3 = sext i32 0 to i64
    %t4 = getelementptr i8, ptr %t2, i64 %t3
    %t5 = trunc i32 122 to i8
    store i8 %t5, ptr %t4, align 1
    %t6 = load ptr, ptr %s.2, align 8
    %t7 = freeze ptr %t6
    %t8 = sext i32 0 to i64
    %t9 = getelementptr i8, ptr %t7, i64 %t8
    %t10 = load i8, ptr %t9, align 1
    %t11 = freeze i8 %t10
    %t12 = sext i8 %t11 to i32
    ret i32 %t12

after.ret.1:
    ret i32 0
}
