
#define _CRT_SECURE_NO_WARNINGS 1

//希望老师在批改作业之前可以简单看一下代码的文档 -- README.md 谢谢！
/*代码已按照要求实现了题目中所有功能，详情请见代码的文档*/

using namespace std;


#if 1
#include"StudentDataBase.h"
/*
	std::array<string, 5> __CourseNames = { "C Programming Language" ,"Advanced math", "College English",
		"Control Theory","Python" };
*/
#define pinf_double 999.99 //定义正无穷小数
#define ninf_double -999.99 //定义负无穷小数
#define pinf_uint 99999 //定义正无穷整数
#define ninf_uint 0 //定义负无穷整数
void Run(ifstream& file1, ifstream& file2)
{
	StudentDataBase database(file1, file2);
	ofstream ofs("result_test.txt");//把测试结果都输出到这里
	ofs << "希望老师在批改作业之前可以简单看一下代码的文档 -- README.md 谢谢！" << endl << endl;
	ofs << " --------------------- 测试排序的功能 --------------------- " << endl;
	ofs << "用名字排序" << endl;
	database.SortByName(ofs); ofs << endl << endl;
	ofs << "用Python的成绩排序" << endl;
	database.SortByKey(ofs, "Python"); //传入key -- 作为排序的关键字
	/*
	关键词有:
	"C Programming Language" ,"Advanced math", "College English",
	"Control Theory","Python",
	"total_score","avg_score"
	*/
	ofs << endl << endl;
	ofs << " --------------------- 测试查找的功能 --------------------- " << endl << endl;
	/*
		除了key之外传一个参数 -- 表示单值查找
		如果除了key穿了两个参数 -- 代表区间查找
		例如：
			(key,30,pinf_double) 代表大于30的
			(key,ninf_double,30) 代表小于30的
		详细使用方法请见代码的文档，谢谢
	*/
	ofs << "查找C语言大于70分小于80分的同学" << endl;
	set<size_t>ret1 = database.FindByScoreWithKey("C Programming Language", 70, 80);//查找C语言大于70分小于80分的同学
	database.OutputByIdSet(ret1, ofs);
	ofs << endl << endl;

	//测试交并集
	ofs << "删除学生中 女同学且C语言不足70分，并上学年为2021级的同学" << endl;
	set<size_t>ret2 = (database.FindBySex(FEMALE)
		& database.FindByScoreWithKey("C Programming Language", ninf_double, 70))
		| database.FindBySchoolYear(2021);
	//删除学生中 女同学且C语言不足70分，并上学年为2021级的同学
	database.EraseInfoByIdSet(ret2);
	database.SortByName(ofs);
}
void Test(ifstream& file1, ifstream& file2)
{
	ofstream ofs("result.txt");
	ofs << "希望老师在批改作业之前可以简单看一下代码的文档 -- README.md 谢谢！" << endl << endl;
	StudentDataBase database(file1, file2);
	ofs << " ---------------------------------------- 打印2020级选修C语言且成绩小于60分的学生 ---------------------------------------- " << endl;
	database.OutputByIdSet(database.FindByScoreWithKey("C Programming Language", ninf_double, 60), ofs);
	ofs << endl << endl;
	ofs << " ---------------------------------------- 统计课程平均分大于80的学生个人信息并输出 ---------------------------------------- " << endl;
	database.OutputByIdSet(database.FindByScoreWithKey("avg_score", 80, pinf_double), ofs);
	ofs << endl << endl;
	ofs << " ----------------------------------------------- 查询每个学生是否修满20学分 ---------------------------------------------- " << endl;
	set<size_t>valid = database.CheckCredits(20);
	set<size_t>all = database.ReturnAllStudents();
	ofs << " ------ 以下学生修满了20个学分 ------ " << endl;
	for (const auto& e : valid)
	{
		ofs << e << endl;
	}
	ofs << " ------ 以下学生没有修满20个学分 ------ " << endl;
	vector<size_t>ans(CAP_MAX);
	set_difference(all.begin(), all.end(), valid.begin(), valid.end(), ans.begin());//求差集
	set<size_t>ret(ans.begin(), ans.end()); ret.erase(0);
	for (const auto& e : ret)
	{
		ofs << e << endl;
	}
}
int main()
{
	ifstream file1("StudentInfo.txt");
	ifstream file2("StudentCourse.txt");
	Run(file1, file2);
	//Test(file1, file2);
	return 0;
}
#endif