#pragma once


//希望老师在批改作业之前可以简单看一下代码的文档 -- README.md 谢谢！
/*代码已按照要求实现了题目中所有功能，详情请见代码的文档*/



#include<set>
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<unordered_map>
#include <sstream>
#include<array>
#include<algorithm>
#include<climits>
#include<tuple>
#include<assert.h>
using namespace std;


#define pinf_double 999.99 //定义正无穷小数
#define ninf_double -999.99 //定义负无穷小数
#define pinf_uint 99999 //定义正无穷整数
#define ninf_uint 0 //定义负无穷整数
enum Sex
{
	MALE,FEMALE
};
//课程
struct CourseObj
{
public:
	string __course_name;
	double __credits;
	double __score;
public:
	CourseObj(string name, double cre, double s)
		:__course_name(name), __credits(cre), __score(s) {}
};
//给CourseObj重载一个通过指针打印内容的<<
void DisplayCourseInfo(ostream& out, const CourseObj* const& ptr);
struct Student
{
public:
	size_t __id;
	string __name;
	Sex __sex;
	size_t __birthday;
	size_t __school_year; //第几学年
	string __birth_place;
	vector<CourseObj*>__course_list;
	double __total_score;
	double __avg_score;
	double __total_credits;
public:
	~Student()
	{
		for (auto& e : __course_list)
		{
			delete e;
		}
	}
};
//给Student重载一个通过指针输出<<
void DisplayStudentByPtr(ostream& out, const Student* const& ptr);


struct StudentDataBase
{
protected:
	//因为题目要求需要多关键字查询 -- 还需要效率高 -- 只能维护多棵红黑树
	//多棵红黑树用哈希表存
	unordered_map<string, multimap<double, size_t, greater<double>>*>__inner_maps;//因为都是分数，所以降序
	unordered_map<size_t, Student*>__hash_map; //用一个哈希表存所有的学生信息 -- key是id
	std::array<string, 5> __CourseNames = { "C Programming Language" ,"Advanced math", "College English",
		"Control Theory","Python" }; //课程名称
protected:
	struct init_tools;//封装初始化哈希表和map所需要的一些工具结构
	tuple<size_t, size_t, size_t> __get_date(const Student* ptr);
protected:
	//一些排序要用到的仿函数
	struct Cmp_SortByName;
	struct Cmp_SortBySex;
	struct Cmp_SortBySchoolYear;
	struct Cmp_SortByBirthPlace;
	struct Cmp_SortByTotalCredits;
protected:
	//初始化
	void __init_hash_map(ifstream& file1, ifstream& file2);
	void __init_maps();
public:
	StudentDataBase(ifstream& file1, ifstream& file2);//构造函数
	~StudentDataBase();//析构函数
	StudentDataBase(const StudentDataBase&) = delete;//数据库不允许进行拷贝构造
	StudentDataBase& operator=(const StudentDataBase&) = delete;//不允许赋值行为
public:
	//排序功能
	void SortByName(ostream& output = cout);//按照名字排序
	void SortByTotalScore(ostream& output = cout);//按照总分排序
	void SortByKey(ostream& output = cout, string sortKey = "C Programming Language"); // 这里面不止一个排序
	/* 按照各种分数排序（包括生日）
	* SortByScoreWithKey()接口里面提供和8种字段的排序 -- 由sortKey参数控制
	* 详细的使用方法见 README.md文档
	*/
	void SortBySex(ostream& output = cout);//按照性别排序
	void SortBySchoolYear(ostream& output = cout);//按照学年排序
	void SortByBirthPlace(ostream& output = cout);//按照出生地排序
	void SortByTotalCredits(ostream& output = cout);//按照总学分排序 
	//以上已经提供了Student类种所有字段的排序
	// 
	
	//根据各个字段进行查找
	set<size_t> FindById(size_t id);//按照id查找
	set<size_t> FindByName(const string& name);//按照名字查找
	set<size_t> FindByBirthday(const tuple<size_t, size_t, size_t>& lower_limit,
		const tuple<size_t, size_t, size_t>& upper_limit = tuple<size_t, size_t, size_t>());//按照生日查找(支持区间)
	set<size_t> FindBySchoolYear(const size_t& sy);//按照学年查找
	set<size_t> FindByScoreWithKey(string key, double lower_limit, double upper_limit = INT_MAX);
	//按照分数查找 --- 这里面不止一种查找
	//支持8种关键字查找 -- 由key控制
	//同时也支持范围查找
	//具体使用方法见 README.md文档
	set<size_t> FindBySex(const Sex& sex);//按照性别查找
	set<size_t> CheckCredits(const double& cre);//判断学分是否满cre分
	set<size_t> ReturnAllStudents();//返回所有学生的id集合
	//根据id集合删除
	void EraseInfoByIdSet(const set<size_t>& s);
	//根据id集合打印
	void OutputByIdSet(const set<size_t>s, ostream& out = cout);
};
//查找结果的交并
#define CAP_MAX 999 //定义数据库容量最大大小
set<size_t> operator|(const set<size_t>& s1, const set<size_t>& s2);
set<size_t> operator&(const set<size_t>& s1, const set<size_t>& s2);