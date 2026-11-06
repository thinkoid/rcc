@.str.0 = private global [4 x i8] c"%p\0A\00", align 1
@.str.1 = private global [4 x i8] c"%p\0A\00", align 1

declare i32 @printf(ptr, ...)

define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    %p.2 = alloca ptr, align 8
    %x.3 = alloca i32, align 4
    %y.4 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    %t1 = load i32, ptr %argc.1, align 4
    %t2 = freeze i32 %t1
    %t3 = add i32 4, %t2
    store i32 %t3, ptr %x.3, align 4
    store ptr %x.3, ptr %p.2, align 8
    %t4 = load ptr, ptr %p.2, align 8
    %t5 = freeze ptr %t4
    %t6 = call i32 (ptr, ...) @printf(ptr @.str.0, ptr %t5)
    store i32 9, ptr %y.4, align 4
    %t7 = call i32 (ptr, ...) @printf(ptr @.str.1, ptr %y.4)
    %t8 = load ptr, ptr %p.2, align 8
    %t9 = freeze ptr %t8
    %t10 = load i32, ptr %t9, align 1
    %t11 = freeze i32 %t10
    ret i32 %t11

after.ret.1:
    ret i32 0
}
