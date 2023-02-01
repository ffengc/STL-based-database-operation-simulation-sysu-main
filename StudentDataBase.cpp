
#define _CRT_SECURE_NO_WARNINGS 1



#include"StudentDataBase.h"


//希望老师在批改作业之前可以简单看一下代码的文档 -- README.md 谢谢！
//所有函数的作用请见文档和声明
/*代码已按照要求实现了题目中所有功能，详情请见代码的文档*/


/* ----------------------------------------------- Student和CourseObj的打印 ----------------------------------------------- */
//给CourseObj重载一个通过指针打印内容的<<
void DisplayCourseInfo(ostream& out, const CourseObj* const& ptr)
{
	out << "CourseName: " << ptr->__course_name << endl;
	out << "Credits : " << ptr->__credits << endl;
	out << "Score : " << ptr->__score << endl;
	out << endl;
}
//给Student重载一个通过指针输出<<
void DisplayStudentByPtr(ostream& out, const Student* const& ptr)
{
	out << " -------------------- Student Info Begin -------------------- " << endl;
	out << "Name: " << ptr->__name << endl;
	out << "Id: " << ptr->__id << endl;
	out << "Sex: " << (ptr->__sex == FEMALE ? "Female" : "Male") << endl;
	out << "Birthday: " << ptr->__birthday << endl;
	out << "SchoolYear: " << ptr->__school_year << endl;
	out << "BirthPlace: " << ptr->__birth_place << endl;
	out << " ========= CourseInfo Begin ========= " << endl;
	for (const auto& e : ptr->__course_list)
	{
		//e -- CourseObj*
		DisplayCourseInfo(out, e);
	}
	out << "TotalScore: " << ptr->__total_score << endl;
	out << "AvgScore: " << ptr->__avg_score << endl;
	out << "TotalCredits : " << ptr->__total_credits << endl;
	out << " ========= CourseInfo End =========== " << endl;
	out << " -------------------- Student Info End ---------------------- " << endl;
}


/* ------------------------------------------------- 查找结果的交并集 ------------------------------------------------- */
set<size_t> operator|(const set<size_t>& s1, const set<size_t>& s2)
{
	vector<size_t>ret(CAP_MAX);
	set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), ret.begin());//并集
	//此时ret里面会有多出来的0 -- 学生的id不可能为0
	set<size_t>ans(ret.begin(), ret.end());
	ans.erase(0);//可以直接调用erase -- 首先：学生id肯定大于0，所以0位于红黑树的root -- 所以这里是O(1)的操作
	return ans;
}
set<size_t> operator&(const set<size_t>& s1, const set<size_t>& s2)
{
	vector<size_t>ret(CAP_MAX);
	set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), ret.begin());//交集
	set<size_t>ans(ret.begin(), ret.end());
	ans.erase(0);
	return ans;
}


/* --------------------------------------------- 数据库内部接口定义protected: --------------------------------------------- */
struct StudentDataBase::init_tools
{
	size_t __clean_the_str(string& str)
	{
		string ret;
		for (auto& e : str)
		{
			if (isdigit(e))
			{
				ret.push_back(e);
			}
		}
		return stoi(ret);
	}
	void __deal_the_sen(Student* ptr, string str)
	{
		vector<string>words;
		istringstream line(str);
		string word;
		while (line >> word)
		{
			words.push_back(word);
		}
		ptr->__id = stoi(words[0]);
		ptr->__name = words[1];
		ptr->__sex = (words[2] == "Female" ? FEMALE : MALE);
		ptr->__birthday = __clean_the_str(words[3]);
		ptr->__school_year = stoi(words[4]);
		ptr->__birth_place = words[5];
	}
	size_t get_line_id(string& str)
	{
		istringstream line(str);
		string word;
		line >> word;
		return stoi(word);
	}
	void __init_course_list(string& str, vector<CourseObj*>& v)
	{
		//先把所有单词提取出来
		istringstream line(str);
		vector<string>words;
		string word;
		while (line >> word)
		{
			words.push_back(word);
		}
		//这个words数组[0]位置的元素不用用，因为我们已经确定现在对哪个vector进行操作了
		string course_name;
		//把课程名字找到
		size_t i = 0;
		for (i = 1;; i++)
		{
			if (isdigit(words[i][0]))break;
			course_name += words[i];
			course_name += ' ';
		}
		//此时的course_name会多一个空格
		course_name.pop_back();
		double cre = stod(words[i++]);
		double s = stod(words[i]);
		CourseObj* pobj = new CourseObj(course_name, cre, s);
		v.push_back(pobj);
	}
	template<class iterator>
	iterator my_find_if(iterator start, iterator end, const string& name)
	{
		while (start != end)
		{
			if ((*start)->__course_name == name)return start;
			start++;
		}
		return end;
	}
};
struct StudentDataBase::Cmp_SortByName
{
	bool operator()(Student* ptr1, Student* ptr2)
	{
		return ptr1->__name < ptr2->__name;
	}
};
struct StudentDataBase::Cmp_SortBySex
{
	bool operator()(Student* ptr1, Student* ptr2)
	{
		return ptr1->__sex < ptr2->__sex;
	}
};
struct StudentDataBase::Cmp_SortBySchoolYear
{
	bool operator()(Student* ptr1, Student* ptr2)
	{
		return ptr1->__school_year < ptr2->__school_year;
	}
};
struct StudentDataBase::Cmp_SortByBirthPlace
{
	bool operator()(Student* ptr1, Student* ptr2)
	{
		return ptr1->__birth_place < ptr2->__birth_place;
	}
};
struct StudentDataBase::Cmp_SortByTotalCredits
{
	bool operator()(Student* ptr1, Student* ptr2)
	{
		return ptr1->__total_credits < ptr2->__total_credits;
	}
};
void StudentDataBase::__init_hash_map(ifstream& file1, ifstream& file2)
{
	//把这些信息存到哈希表中
	string cur_sen;
	size_t line_number = 0;
	//处理第一个文件
	while (getline(file1, cur_sen))
	{
		line_number++;
		if (line_number == 1)continue;//跳过第一行
		Student* newStudentPtr = new Student;
		init_tools().__deal_the_sen(newStudentPtr, cur_sen);
		__hash_map.insert({ newStudentPtr->__id,newStudentPtr });
	}
	//处理第二个文件
	line_number = 0;
	while (getline(file2, cur_sen))
	{
		line_number++;
		if (line_number == 1)continue;//跳过第一行
		//先把每一行的id找到 -- 然后通过这个id在哈希表中找到已经new好的Student对象的指针
		size_t cur_id = init_tools().get_line_id(cur_sen);
		Student* cur_student_ptr = __hash_map[cur_id];
		//对这个指针里面的__course_list进行操作
		init_tools().__init_course_list(cur_sen, cur_student_ptr->__course_list);
	}
	//走到这里，所有学生的信息已经存好了
	//但是总分还没有处理好 -- 处理一下总分，还有总的学分
	for (auto& e : __hash_map)
	{
		e.second->__total_score = 0;
		e.second->__total_credits = 0;
		for (auto& ee : e.second->__course_list)
		{
			e.second->__total_score += ee->__score; //没考的直接算0分了
			e.second->__total_credits += ee->__credits;
		}
		e.second->__avg_score = e.second->__total_score /
			((double)e.second->__course_list.size());
	}
}
void StudentDataBase::__init_maps()
{
	//因为课程内容是固定的 -- 所以写死map的构造 -- 维护8个map，每个map对应相对应的字段
	__inner_maps.insert({ "total_score",new multimap<double, size_t,greater<double>> });
	__inner_maps.insert({ "avg_score",new multimap<double, size_t,greater<double>> });
	for (const auto& e : __CourseNames)
	{
		__inner_maps.insert({ e,new multimap<double, size_t,greater<double>> });
	}
	__inner_maps.insert({ "birthday",new multimap<double, size_t,greater<double>> });
	//0-total_score 1-cprog 2-Advanced math 3-College English
	//4-Control Theory 5-Python 6-birthday 8-avg_score
	cout << endl;
	for (const auto& e : __hash_map)
	{
		//e -- {id,ptr}
		Student* cur_ptr = e.second;
		auto& v = cur_ptr->__course_list;
		//把每一科的成绩找到
		for (const auto& ee : __CourseNames)
		{
			string name = ee;
			size_t id = e.first;
			auto it = init_tools().my_find_if(v.begin(), v.end(), name);
			if (it == v.end())
			{
				//找不到这门课的信息
				continue;
			}
			//找到了 -- 把这门课的分数插入到map里面去
			double score = (*it)->__score;
			__inner_maps[name]->insert({ score,id });
		}
		//现在已经处理好了5个map ，还有总分和平均分和birthday的
		__inner_maps["birthday"]->insert({ cur_ptr->__birthday,e.first });
		__inner_maps["total_score"]->insert({ cur_ptr->__total_score,e.first });
		__inner_maps["avg_score"]->insert({ cur_ptr->__avg_score,e.first });
	}
}
tuple<size_t, size_t, size_t> StudentDataBase::__get_date(const Student* ptr)
{
	size_t year;
	size_t month;
	size_t day;
	string str = to_string(ptr->__birthday);
	assert(str.size() == 8);//字符串的长度应该是8
	year = stoi(str.substr(0, 4));
	month = stoi(str.substr(4, 2));
	day = stoi(str.substr(6, 2));
	return make_tuple(year, month, day);
}

/* --------------------------------------------- 数据库内部接口定义public: --------------------------------------------- */
StudentDataBase::StudentDataBase(ifstream& file1, ifstream& file2)
{
	__init_hash_map(file1, file2);
	__init_maps();
}
StudentDataBase::~StudentDataBase()
{
	//循环释放8个multimap的指针
	for (const auto& e : __inner_maps)
	{
		//e -- {string,multimap*}
		delete e.second;
	}
	//循环释放8个Student*
	for (const auto& e : __hash_map)
	{
		delete e.second;
	}
}
//排序功能
void StudentDataBase::SortByName(ostream& output)
{
	//因为名字我们没有维护map，所以只能取出来进行排序
	vector<Student*>arr;
	for (const auto& e : __hash_map)
	{
		arr.push_back(e.second);
	}
	sort(arr.begin(), arr.end(), Cmp_SortByName());
	for (const auto& e : arr)
	{
		DisplayStudentByPtr(output, e);
	}
}
void StudentDataBase::SortByTotalScore(ostream& output)
{
	//按照总分排序并打印
	//总分我们是有维护map的 -- 所以直接打印就行了
	//这个map的关键字是 "total_score" 
	auto& usingMapPtr = __inner_maps["total_score"];
	//遍历这个map
	for (const auto& e : (*usingMapPtr))
	{
		size_t cur_id = e.second;
		Student* cur_stu_ptr = __hash_map[cur_id];
		DisplayStudentByPtr(output, cur_stu_ptr);
	}
}
void StudentDataBase::SortByKey(ostream& output, string sortKey)
{
	//这个函数通关传入关键字来调用 -- 告诉程序需要哪一门的成绩排序结果
	auto& usingMapPtr = __inner_maps[sortKey];
	//遍历这个map
	for (const auto& e : (*usingMapPtr))
	{
		size_t cur_id = e.second;
		Student* cur_stu_ptr = __hash_map[cur_id];
		DisplayStudentByPtr(output, cur_stu_ptr);
	}
}
void StudentDataBase::SortBySex(ostream& output)
{
	vector<Student*>arr;
	for (const auto& e : __hash_map)
	{
		arr.push_back(e.second);
	}
	sort(arr.begin(), arr.end(), Cmp_SortBySex());
	for (const auto& e : arr)
	{
		DisplayStudentByPtr(output, e);
	}
}
void StudentDataBase::SortBySchoolYear(ostream& output)
{
	vector<Student*>arr;
	for (const auto& e : __hash_map)
	{
		arr.push_back(e.second);
	}
	sort(arr.begin(), arr.end(), Cmp_SortBySchoolYear());
	for (const auto& e : arr)
	{
		DisplayStudentByPtr(output, e);
	}
}
void StudentDataBase::SortByBirthPlace(ostream& output)
{
	vector<Student*>arr;
	for (const auto& e : __hash_map)
	{
		arr.push_back(e.second);
	}
	sort(arr.begin(), arr.end(), Cmp_SortByBirthPlace());
	for (const auto& e : arr)
	{
		DisplayStudentByPtr(output, e);
	}
}
void StudentDataBase::SortByTotalCredits(ostream& output)
{
	vector<Student*>arr;
	for (const auto& e : __hash_map)
	{
		arr.push_back(e.second);
	}
	sort(arr.begin(), arr.end(), Cmp_SortByTotalCredits());
	for (const auto& e : arr)
	{
		DisplayStudentByPtr(output, e);
	}
}
//根据各个字段进行查找
set<size_t> StudentDataBase::FindById(size_t id)
{
	set<size_t>s;
	s.insert(id);
	return s;
}
set<size_t> StudentDataBase::FindByName(const string& name)
{
	//O(n)线性查找
	set<size_t>s;
	for (const auto& e : __hash_map)
	{
		if (e.second->__name == name)
		{
			s.insert(e.first);
		}
	}
	return s;
}
set<size_t> StudentDataBase::FindByBirthday(const tuple<size_t, size_t, size_t>& lower_limit,
	const tuple<size_t, size_t, size_t>& upper_limit)
{
	set<size_t>s;
	//和FindByScoreWithKey一样 -- 可以通过输入参数个数判断是单个查找还是范围查找
	if (get<0>(upper_limit) == 0 && get<1>(upper_limit) == 0 && get<2>(upper_limit) == 0)
	{
		//说明只输入了一个tuple  -- 代表单个查找
		size_t year = get<0>(lower_limit);
		size_t month = get<1>(lower_limit);
		size_t day = get<2>(lower_limit);
		for (const auto& e : __hash_map)
		{
			tuple<size_t, size_t, size_t>cur_date = __get_date(e.second);
			//判断cur_date和我们现在要查找的是否相等
			size_t cur_year = get<0>(cur_date);
			size_t cur_month = get<1>(cur_date);
			size_t cur_day = get<2>(cur_date);
			if ((cur_year == year || year == pinf_uint)
				&& (cur_month == month || month == pinf_uint)
				&& (cur_day == day || day == pinf_uint))
			{
				s.insert(e.first);
			}
		}
	}
	else
	{
		//范围查找
		//如果lower_limit全部都是pinf，说明查找范围只有最大值
		//如果upper_limit全部都是pinf，说明查找范围只有最小值
		//而且范围查找 -- 两个tuple都要传！

		//目前先只支持年份的范围查找
		size_t lower_limit_year = get<0>(lower_limit);
		size_t upper_limit_year = get<0>(upper_limit);
		for (const auto& e : __hash_map)
		{
			tuple<size_t, size_t, size_t>cur_date = __get_date(e.second);
			//判断cur_date和我们现在要查找的是否相等
			size_t cur_year = get<0>(cur_date);
			if (cur_year < upper_limit_year && cur_year >= lower_limit_year)
			{
				s.insert(e.first);
			}
		}
	}
	return s;
}
set<size_t> StudentDataBase::FindBySchoolYear(const size_t& sy)
{
	set<size_t>s;
	for (const auto& e : __hash_map)
	{
		if (e.second->__school_year == sy)
		{
			s.insert(e.first);
		}
	}
	return s;
}
set<size_t> StudentDataBase::FindByScoreWithKey(string key, double lower_limit, double upper_limit)
{
	/// 函数返回学生id的集合即可
	/*
	关键词有:
	"C Programming Language" ,"Advanced math", "College English",
	"Control Theory","Python",
	"total_score","avg_score"
	*/
	//key和lower_limit是必须传的 -- 如果只传了lower_limit的时候，代表单个查找，否则代表范围查找
	set<size_t>ans;
	if (upper_limit == DBL_MAX)
	{
		//单个查找
		auto& usingMap = *(__inner_maps[key]);
		//用multimap的equal_range去查找所有符合条件的stu
		//这样查找的效率是logn -- 效率非常高
		auto retPair = usingMap.equal_range(lower_limit);
		auto start = retPair.first;
		auto end = retPair.second;
		while (start != end)
		{
			ans.insert(start->second);
			start++;
		}
	}
	else
	{
		//区间查找
		auto& usingMap = *(__inner_maps[key]);
		//这样查找区间的效率就是logn -- 效率非常高！
		auto start = usingMap.upper_bound(upper_limit);//区间下限
		auto end = usingMap.lower_bound(lower_limit);//区间上限
		//start++;//因为lower_bound和upper_bound找到的是闭区间
		while (start != end && start != usingMap.end())
		{
			ans.insert(start->second);
			start++;
		}
	}
	return ans;
}
set<size_t> StudentDataBase::FindBySex(const Sex& sex)
{
	//线性O(n)查找
	set<size_t>s;
	for (const auto& e : __hash_map)
	{
		if (e.second->__sex == sex)
		{
			s.insert(e.first);
		}
	}
	return s;
}
set<size_t> StudentDataBase::CheckCredits(const double& cre)
{
	set<size_t>s;
	for (const auto& e : __hash_map)
	{
		if (e.second->__total_credits >= cre)
		{
			s.insert(e.first);
		}
	}
	return s;
}
set<size_t> StudentDataBase::ReturnAllStudents()
{
	set<size_t>s;
	for (const auto& e : __hash_map)
	{
		s.insert(e.first);
	}
	return s;
}
//根据id集合删除
void StudentDataBase::EraseInfoByIdSet(const set<size_t>& s)
{
	//现在哈希表中删除 -- 再在8个multimap中删除 -- 总复杂度为O(n*s.size())
	for (const auto& key : s)
	{
		__hash_map.erase(key);
	}
	//在multimap中删除
	for (const auto& key : s)
	{
		for (const auto& mapsPtr : __inner_maps) //这个循环共8次
		{
			for (const auto& e : *(mapsPtr.second))
			{
				if (e.second == key)
				{
					mapsPtr.second->erase(e.first);
					break;//因为一个map只需要做一次删除 -- 所以直接break
				}
			}
		}
	}
}
//根据set打印
void StudentDataBase::OutputByIdSet(const set<size_t>s, ostream& out)
{
	for (const auto& key : s)
	{
		DisplayStudentByPtr(out, __hash_map[key]);
	}
}