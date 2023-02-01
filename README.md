
老师要求用ofstream输出的结果在 result.txt 中
其余接口的部分测试结果在 result_test.txt 中 （同样也是使用ofstream输出的，其中部分解释是手动输入的）

## 21312780 俞沣城
## 题目要求在代码中或文档的体现索引：
1. 使用stl存储两张表的信息 -- 见 前期结构准备
2. 要求效率高 -- 维护多棵红黑树 -- 保证查找效率 -- 见 前期结构准备
3. 如果多个条目，默认用id的顺序 -- 查找中返回set<~size_t~>类型（key是id） -- 默认按照id排好序
4. 根据每个字段进行排序 -- Student中的每个字段都维护了排序方法 -- 见代码，其中SortByKey接口维护了多种排序，用传入SortKey控制
5. 支持范围查询 -- 见 查找功能 中的第4点
6. 支持各个字段进行删除 -- 结合查找使用该接口 -- 查找支持各个字段，删除通过利用查找的结果进行删除
7. 支持结果的交并 -- 见&和|的重载
8. 代码已经尽我最大可能把查询的效率维持在O(logn)了


## StudentDataBase的实现思路和总体框架
### 前期结构准备
1. 首先通过enum枚举性别 -- 加强代码的可读性 enum Sex;

2. 通过定义struct CourseObj; 和struct Student;存储Student每一个学生的信息
	等题目提供的两个txt文件读取完之后，数据全部统一由struct Student结构进行存储
	
3. 所有学生的信息统一存储到一个**哈希表**中
	其中哈希表中的结构为 unordered_map<~size_t, Student*~>__hash_map;
	key为学生的id
	value为指向学生信息的指针
	在本次数据库结构中 -- 全部围绕学生的Id进行操作
	**通过哈希表存储学生信息，并以id作为key -- 可以做到通过key找到学生信息的复杂度为O(1)**

4. 题目要求通过多个字段查找学生，还需要高效率
	因此我们维护多个multimap来同时存储学生的信息 -- 以便保证查询操作的效率可以达到O(logn)
	其中，每个multimap的key为相对应的字段，value对应于学生的id -- (结合id可以O(1)找到学生的指针，可以完成高效的查询)
	我们一共维护了8个multimap(降序multimap,通过仿函数greater<~double~>)
	这8个multimap我们用一个哈希表封装好
	unordered_map<~string, multimap<~double, size_t, greater<~double~>~>*~>__inner_maps;
	这个哈希表的key则为8个multimap所对应的标签，分别为：
	"C Programming Language" ,"Advanced math", "College English",
		"Control Theory","Python","total_score", "avg_score","birthday"
	通过该哈希表中的key 我们可以找到对应的multimap

### 两个txt文件数据的录入
1. 数据的录入通过内部__init_hash_map()接口和__init_maps()完成
	这两个接口中所需的一些工具接口，被封装到了内部类struct init_tools;中

2. __init_hash_map()接口通过读取两个txt文件的每一行，做字符串分析后，new出Student对象，并插入到哈希表中

3. __init_maps()接口，通过读取两个txt文件中的每一行，找到对应的id后，在__hash_map中找到对应的已经new好的Student对象，进行数据的录入

### 排序功能
1. 在所有排序接口中 -- 如果要排序的字段已经维护了一棵红黑树
	则直接遍历红黑树打印
	复杂度:O(n)
2. 在所有排序接口中 -- 如果要排序的字段没有被维护红黑树
	则取出来 -- 排序后打印
	于此同时，需要提供sort的仿函数，例如：Cmp_SortByName
	复杂度:O(nlogn)

### 查找功能 
1. 查找功能统一返回set<~size_t~>类型，为符合条件的id的集合 --- 好处：当我们打印多个条目时候 -- 因为set的特性，自动按照id排好序了
	这样做的优势：方便交并运算 
	通过id找到学生信息效率很高 -- 为O(1)
2. 若查找的字段已经维护了红黑树 -- 直接采用multimap的内部接口进行查找：如multimap::upper_bound,multimap::equal_range
3. 若查找的字段没有维护红黑树 -- 则遍历查找

4. 支持范围查找的接口有 FindByBirthday() -- 通过生日查找
					  FindByScoreWithKey() -- 通过分数查找
	其中：FindByBirthday()接口：
		FindByBirthday()接口传入两个元组类型数据结构 (tuple<~size_t,size_t,size_t~>)
		set<~size_t~> FindByBirthday(const tuple<~size_t, size_t, size_t~>& lower_limit,
			const tuple<~size_t, size_t, size_t>& upper_limit = tuple<~size_t, size_t, size_t~>())
		其中，第二个元组提供缺省参数
	使用方法：
		当第二个参数不传的时候，即FindByBirthday()只传入一个tuple -- 表示单个条件查询
		即查询生日符合第一个元组的学生
		当传入两个参数的时候：分为三种情况：
			1. 如果lower_limit为(ninf_uint,ninf_uint,ninf_uint) -- 表示无下限查找 -- 表示只要生日在upper_limit之前的都符合条件
			2. 如果upper_limit为(pinf_uint,pinf_uint,pinf_uint) -- 表示无上限查找
			3. lower_limit,upper_limit中都是其余整数 -- 表示区间查找

	其中FindByScoreWithKey()接口：
		set<~size_t~> FindByScoreWithKey(string key, double lower_limit, double upper_limit = INT_MAX)
		通过key的传入表示要查找的字段，key的选择有：
		"C Programming Language" ,"Advanced math", "College English",
		"Control Theory","Python","total_score", "avg_score"
	使用方法：
		和FindByBirthday()接口一致
		通过区分传入参数个数和值，分别表示 单值查找，无上限查找，无下限查找和区间查找

### 删除功能
	通过传入的set<~size_t~> -- 即学生的id集合进行查找
	遍历哈希表（O(1)）和8个map (O(n))对指定id的学生进行删除操作
	总的复杂度：O(n)

### 交并功能
	可以调用stl的set_union等函数对set完成交并功能，具体请见代码注释

### 析构
	循环释放哈希表和8个multimap的指针
	其中，Student需要手动实现析构函数析构CourseObj*

### 其它接口的功能
	通过注释的形式展示