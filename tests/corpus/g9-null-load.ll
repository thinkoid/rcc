define i32 @main(i32 %p.0) null_pointer_is_valid {
entry:
    %argc.1 = alloca i32, align 4
    store i32 %p.0, ptr %argc.1, align 4
    %t1 = load i32, ptr %argc.1, align 4
    %t2 = freeze i32 %t1
    %t3 = icmp eq i32 %t2, 1
    %t4 = zext i1 %t3 to i32
    %t5 = icmp ne i32 %t4, 0
    br i1 %t5, label %if.then.1, label %if.end.1

if.then.1:
    %t6 = load i32, ptr null, align 1
    %t7 = freeze i32 %t6
    ret i32 %t7

after.ret.2:
    br label %if.end.1

if.end.1:
    ret i32 5

after.ret.3:
    ret i32 0
}
