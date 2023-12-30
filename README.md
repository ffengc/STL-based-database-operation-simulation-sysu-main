# 数据库操作的模拟

**实验要求用ofstream输出的结果在 result.txt 中**
**其余接口的部分测试结果在 result_test.txt 中 （同样也是使用ofstream输出的，其中部分解释是手动输入的）**

## 项目需求

模拟数据库的操作，主要读取`StudentInfo.txt`和`StudentCourse.txt`两张表，其中`StudentInfo.txt` 为学生个人信息表，包括学号 id、姓名 name、性 别 sex、出生年月 birthday、学年 schoolYear、籍贯 birthplace，StudentCourse.txt 为学生选课信息表，包括学号 id、选课 course、学分 credits、分数 score。

1. 读入两个文本文件 StudentInfo.txt 和 StudentCourse.txt，使用合适的 STL 数据 结构存储两张表的信息，要求以下的操作效率要高，不建议临时做线性遍历 (即 n 个元素逐一访问)。查询结果需要联表打印出相关信息;如果有多个 条目，默认按照 id 排序。

1. 提供排序操作，根据各个字段进行排序，比如 SortByName, SortByScore(单 项课程分数)，SortByTotalScore(总分数)，支持范围查询，比如查询某门 课分数在 80-90 之间的学生，注意合理设计接口，尽量做到代码简洁，少冗余。
2. 提供查询和删除操作，根据各个字段进行条件删除。支持多种条件比如`sex == ‘M’ && (birthday.year > 2019 || score < 80)`(不用解析这个字符串表达式)，注 意删除要保持两个表格的一致性。
3. 提供 Test 函数，具体测试以下用例，将结果**通过** **ofstream** **写入**到 result.txt 中
   1. a)  打印 2020 级选修 C 语言且成绩小于 60 分的学生
   2. b)  统计课程平均分大于 80 的学生个人信息并输出
   3. c)  查询每个学生是否修满 20 学分

**StudentCourse.txt 见文件所示。**

## 项目需求在代码中或文档的体现索引：

1. 使用stl存储两张表的信息 -- 见前期结构准备
2. 要求效率高 -- 维护多棵红黑树 -- 保证查找效率 -- 见 前期结构准备
3. 如果多个条目，默认用id的顺序 -- 查找中返回`set<size_t>`类型（key是id） -- 默认按照id排好序
4. 根据每个字段进行排序 -- `Student`中的每个字段都维护了排序方法 -- 见代码，其中`SortByKey`接口维护了多种排序，用传入`SortKey`控制
5. 支持范围查询 -- 见查找功能 中的第4点
6. 支持各个字段进行删除 -- 结合查找使用该接口 -- 查找支持各个字段，删除通过利用查找的结果进行删除
7. 支持结果的交并 -- 见`&`和`|`的重载
8. 代码已经尽我最大可能把查询的效率维持在`O(logn)`了


## StudentDataBase的实现思路和总体框架
### 前期结构准备
1. 首先通过`enum`枚举性别 -- 加强代码的可读性`enum Sex;`

2. 通过定义`struct CourseObj;`和`struct Student;`存储`Student`每一个学生的信息
	等题目提供的两个txt文件读取完之后，数据全部统一由`struct Student`结构进行存储
	
3. 所有学生的信息统一存储到一个**哈希表**中
	其中哈希表中的结构为`unordered_map<size_t, Student*>__hash_map;`
	`key`为学生的`id`
	`value`为指向学生信息的指针
	在本次数据库结构中 -- 全部围绕学生的Id进行操作
	**通过哈希表存储学生信息，并以id作为key -- 可以做到通过key找到学生信息的复杂度为O(1)**

4. 题目要求通过多个字段查找学生，还需要高效率
	因此我们维护多个`multimap`来同时存储学生的信息 -- 以便保证查询操作的效率可以达到`O(logn)`
	其中，每个`multimap`的`key`为相对应的字段，`value`对应于学生的`id` -- (结合id可以O(1)找到学生的指针，可以完成高效的查询)
	我们一共维护了8个`multimap`(降序`multimap`,通过仿函数`greater<double>`)
	这8个`multimap`我们用一个哈希表封装好
	**`unordered_map<string, multimap<double, size_t, greater<double>>>__inner_maps;`**
	这个哈希表的key则为8个`multimap`所对应的标签，分别为：
	`"C Programming Language" ,"Advanced math", "College English",
		"Control Theory","Python","total_score", "avg_score","birthday"`
	通过该哈希表中的`key`我们可以找到对应的`multimap`

### 两个txt文件数据的录入
1. 数据的录入通过内部`__init_hash_map()`接口和`__init_maps()`完成
	这两个接口中所需的一些工具接口，被封装到了内部类`struct init_tools;`中

2. `__init_hash_map()`接口通过读取两个txt文件的每一行，做字符串分析后，`new`出`Student`对象，并插入到哈希表中

3. `__init_maps()`接口，通过读取两个txt文件中的每一行，找到对应的id后，在`__hash_map`中找到对应的已经`new`好的`Student`对象，进行数据的录入

### 排序功能
1. 在所有排序接口中 -- 如果要排序的字段已经维护了一棵红黑树，则直接遍历红黑树打印
	**复杂度:O(n)**
2. 在所有排序接口中 -- 如果要排序的字段没有被维护红黑树则取出来 -- 排序后打印。与此同时，需要提供sort的仿函数，例如：`Cmp_SortByName`
	**复杂度:O(nlogn)**

### 查找功能 
1. 查找功能统一返回`set<size_t>`类型，为符合条件的`id`的集合 --- 好处：当我们打印多个条目时候 -- 因为`set`的特性，自动按照id排好序了
	**这样做的优势：方便交并运算** 
	**通过id找到学生信息效率很高 -- 为O(1)**
2. 若查找的字段已经维护了红黑树 -- 直接采用`multimap`的内部接口进行查找：如`multimap::upper_bound,multimap::equal_range`
3. 若查找的字段没有维护红黑树 -- 则遍历查找

4. 支持范围查找的接口有`FindByBirthday()` -- 通过生日查找
					 `FindByScoreWithKey()`-- 通过分数查找
	其中：`FindByBirthday()`接口：
		`FindByBirthday()`接口传入两个元组类型数据结构` (tuple<size_t,size_t,size_t>)
		set<size_t> FindByBirthday(const tuple<size_t, size_t, size_t>& lower_limit,
			const tuple<size_t, size_t, size_t>& upper_limit = tuple<size_t, size_t, size_t>())`
		**其中，第二个元组提供缺省参数**
	**使用方法：**
		当第二个参数不传的时候，即`FindByBirthday()`只传入一个tuple -- **表示单个条件查询**
		即查询生日符合第一个元组的学生
		当传入两个参数的时候：分为三种情况：
			1. 如果`lower_limit为(ninf_uint,ninf_uint,ninf_uint)` -- 表示无下限查找 -- 表示只要生日在`upper_limit`之前的都符合条件
			2. 如果`upper_limit为(pinf_uint,pinf_uint,pinf_uint)` -- 表示无上限查找
			3. `lower_limit,upper_limit`中都是其余整数 -- 表示区间查找

	其中`FindByScoreWithKey()`接口：
		`set<size_t> FindByScoreWithKey(string key, double lower_limit, double upper_limit = INT_MAX)`
		通过key的传入表示要查找的字段，key的选择有：
		`"C Programming Language" ,"Advanced math", "College English",
		"Control Theory","Python","total_score", "avg_score"`
	**使用方法：**
		和`FindByBirthday()`接口一致
		通过区分传入参数个数和值，分别表示 单值查找，无上限查找，无下限查找和区间查找

### 删除功能
通过传入的`set<size_t>` -- 即学生的id集合进行查找。遍历哈希表（O(1)）和8个map (O(n))对指定id的学生进行删除操作。**总的复杂度：O(n)**

### 交并功能
**可以调用stl的`set_union`等函数对set完成交并功能，具体请见代码注释。**

### 析构
循环释放哈希表和8个multimap的指针。其中，`Student`需要手动实现析构函数析构`CourseObj*`

### 其它接口的功能
通过注释的形式展示.
