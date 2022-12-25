@x = global i32 0, align 4
@y = global i32 0, align 4
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
  store i32 0, i32* @x, align 4
  store i32 2, i32* @y, align 4
  br label %body

body:                                                ; preds = %loop, %0
  %1 = load i32, i32* @x, align 4
  %2 = load i32, i32* @y, align 4
  %3 = mul nsw i32 %2, 3
  %4 = add nsw i32 %3, 1
  %5 = add nsw i32 %4, %1
  store i32 %5, i32* @x, align 4
  %6 = icmp sgt i32 %5, 15
  br i1 %6, label %end, label %loop

loop:                                                ; preds = %body
  br label %body

end:                                               ; preds = %body
  %7 = load i32, i32* @x, align 4
  %8 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 noundef %7)
  ret i32 0
}

declare i32 @printf(i8* noundef, ...) #1
