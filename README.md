## 公历日期转阴历日期
将公历日期转换为对应的阴历日期(对应农历年份应该在1900到2049之间)

### 用法
```sh
make  # 编译生成main和liblunar.so
./main -d ${year}-${month}-${day} -c  # -c为可选，可输出当月月历
```