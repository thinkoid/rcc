@.str.0 = private global [4 x i8] c"abc\00", align 1

define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %s.2 = alloca ptr, align 8
    store i32 %p.0, ptr %argc.1, align 4
    store ptr @.str.0, ptr %s.2, align 8
    %t1 = load ptr, ptr %s.2, align 8
    %t2 = freeze ptr %t1
    %t3 = getelementptr i8, ptr %t2, i64 0
    store i8 122, ptr %t3, align 1
    %t4 = load ptr, ptr %s.2, align 8
    %t5 = freeze ptr %t4
    %t6 = getelementptr i8, ptr %t5, i64 0
    %t7 = load i8, ptr %t6, align 1
    %t8 = freeze i8 %t7
    %t9 = sext i8 %t8 to i32
    ret i32 %t9

after.ret.1:
    ret i32 0
}
