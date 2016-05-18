#include <io.h>
#include <cctype>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <algorithm>
#include <ctime> 
using namespace std;

struct Student {
    string id;
    double grade_gender, grade_will, grade_basis, grade_hw;
    double grade;
	Student() {
		id = "None";
		grade_gender = grade_will = grade_basis = grade_hw = 0;
	}
    Student(string _id, double _grade) {
		id = _id; grade = _grade; grade_hw = 0;
    }
};
struct Greater {  // 重载小于号
    bool operator () (const Student* s1, const Student* s2) const {
        return s1->grade > s2->grade;
    }
};

/* Homework记录的前几名数量可根据需要更改。
  如可保存前10、前20等等。 */
struct Homework {
    set<string> top30Stu; // 保存前十名，key为学号
	set<string> top30Time; // 保存前十位提交的时间，key为时间
	map<string, string> time_stuId; // 保存提交时间和学号的映射，key为时间，value为学号
	string firstTime;  // 记录最早时间，用来记录是程设I还是程设II
    Homework() {
		firstTime = "";
		top30Stu.clear();
		top30Time.clear();
		time_stuId.clear();
    }
};

typedef struct Homework homework;
typedef struct Student student;


const int topNum = 30;
student stu1[500], stu2[500];
map<string, student*> name_stu1; // key为学生ID
map<string, student*> name_stu2;
vector<string> class_name; // key为class对应编号
map<string, homework> homeworks;  // key为homework id，value为对应homework实体
vector<student*> class_stu[10]; // 每个班的所有学生，用student结构体表示 
map<string, int> map_gender, map_will, map_basis;
float per_gender, per_will, per_basis, per_hw;
char * fileStudents = "G://BigData//score_prediction_2014//students";
char * fileClass = "G://BigData//classes";
char * fileSubmission = "D://BigData//submissions";

void preProcess() {
    per_gender = 2;
    per_will = 15;
    per_basis = 6;
	per_hw = 1.5;
    
    char* path1 = "G://BigData//students//24";
    char* path2 = "G://BigData//students//1161";
    char* path3 = "G://BigData//students//5566";
    ifstream _in;
    _in.open(path1);
    if (!_in) {
        cout << "Preprocess 1 failed!!" << endl;
    }
    string content;
    map_gender["male"] = 1;
    map_gender["female"] = 0;
    getline(_in, content, '\n');
    getline(_in, content, '\n'); // 第一志愿
    map_will[content] = 0; // 非第一志愿 
    getline(_in, content, '\n'); // 第一志愿
    map_basis[content] = 0; // 没有基础 
    _in.close();
    
    _in.open(path2);
    if (!_in) {
        cout << "Preprocess 2 failed!!" << endl;
    }
    getline(_in, content, '\n');
    getline(_in, content, '\n'); // 第一志愿
    map_will[content] = 1; // 是第一志愿 
    getline(_in, content, '\n'); // 第一志愿
    map_basis[content] = 1; // 没有参加过竞赛但接触过编程
    _in.close();
    
    _in.open(path3);
    if (!_in) {
        cout << "Preprocess 3 failed!!" << endl;
    }
    getline(_in, content, '\n');
    getline(_in, content, '\n'); // 第一志愿
    getline(_in, content, '\n'); // 第一志愿
    map_basis[content] = 6; // 参加过编程竞赛
    _in.close();
}

void getFiles( string path, vector<string>& files) {
    //文件句柄  
    long hFile = 0;  
    //文件信息  
    struct _finddata_t fileinfo;  
    string p;  
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //如果是目录,迭代之  
            //如果不是,加入列表
            if((fileinfo.attrib &  _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles( p.assign(path).append("\\").append(fileinfo.name), files);  
            } else {
                files.push_back(fileinfo.name);
            }
        } while (_findnext(hFile, &fileinfo) == 0);  
        _findclose(hFile);  
    }
}

// 从students文件夹中获取基本信息 
void getBasicInfo() {
    vector<string> files;
    string path(fileStudents);
    // 获取该路径下的所有文件  
    getFiles(path, files);
    string content;
    int size = files.size();
    ifstream in;
    for (int i = 0; i < size; i++) { // 访问每个学生基本信息文件 
        //in.open(files[i].c_str));
        in.open((path+"//"+files[i]).c_str());
		if (isdigit(files[i][0])) {
			stu1[i].id = files[i];
			stu2[i].id = files[i];
		}
        else
            continue;
        name_stu1[stu1[i].id] = &stu1[i]; // 关联程设I学生id与其实体 
		name_stu2[stu2[i].id] = &stu2[i]; // 关联程设II学生id与其实体 
        //cout << stu[i].id << " : ";
        if (!in) {
            cout << "Open Failed!!" << endl;
        }
        
        getline(in, content, '\n');
        stu1[i].grade_gender = map_gender[content];
		stu2[i].grade_gender = stu1[i].grade_gender;
        getline(in, content, '\n');
        stu1[i].grade_will = map_will[content];
		stu2[i].grade_will = stu1[i].grade_will;
        getline(in, content, '\n');
        stu1[i].grade_basis = map_basis[content];
		stu2[i].grade_basis = stu1[i].grade_basis;

        stu1[i].grade = per_gender*stu1[i].grade_gender + per_will*stu1[i].grade_will + per_basis*stu1[i].grade_basis;
		stu2[i].grade = stu1[i].grade;
        //cout << stu[i].grade << endl;
        
        in.close();
        content.clear();
        //cout<<files[i].c_str()<<endl;
    }
    files.clear();
}

// 从提供的预测文件中获取学生所属班级信息
void getInfoFromClass() {
    vector<string> files;
    string path(fileClass);
    // 获取该路径下的所有文件  
    getFiles(path, files);
    string content;
    int size = files.size();
    ifstream in;
    for (int i = 0; i < size; i++) { // 访问每个班级文件 
        //in.open(files[i].c_str));
        in.open((path+"//"+files[i]).c_str());
        if (!in) {
            cout << "Open Failed!!" << endl;
        }
        // test
        //cout << files[i] << endl;
        /*
        跳过前三行信息，分别为： 
        课程名
        homework ID（作业安排，对应homework目录内容）
        exam IDs（考试安排，一行内多个，对应exams目录内容）
        */
        /*
        getline(in, content, '\n');
        getline(in, content, '\n');
        getline(in, content, '\n'); 
        */
        class_name.push_back(files[i]);
		// 程序设计I
		if (files[i] == "58588" || files[i] == "37508" || files[i] == "90971" || files[i] == "67547" || files[i] == "62934") {
			while (getline(in, content, '\n')) {
				if (name_stu1[content] != NULL) {
					class_stu[i].push_back(name_stu1[content]);
				}
				else { // 该生为重修生 
					student* temp = new student(content, 0);
					class_stu[i].push_back(temp);
				}
			}
		}
		else {
			while (getline(in, content, '\n')) {
				if (name_stu2[content] != NULL) {
					class_stu[i].push_back(name_stu2[content]);
				}
				else { // 该生为重修生 
					student* temp = new student(content, 0);
					class_stu[i].push_back(temp);
				}
			}
		}
        
        //cout << files[i]<<" : "<<class_stu[i].size() << endl;
        
        in.close();
        content.clear();
        //cout<<files[i].c_str()<<endl;
    }
    files.clear();
}

// 用于Debug 
void checkId() {
    char * path1 = "G://BigData//prediction//16832";
    char * path2 = "G://BigData//16832";
    ifstream in;
    in.open(path1);
    if (!in) {
        cout << "Debug file 1 open failed!" << endl;
    }
    string content;
    map<string, int> m;
    while (getline(in, content, '\n')) {
        m[content] = 1;
    }
    in.close();
    
    in.open(path2);
    if (!in) {
        cout << "Debug file 2 open failed!" << endl;
    }
    while (getline(in, content, '\n')) {
        if (m.find(content) == m.end()) {
            cout << content << endl;
        } else {
            m.erase(content);
        }
    }
    if (m.size() != 0) {
        map<string, int>::iterator it;
        it = m.begin();
        cout << it->first << endl;
    }
    
    in.close();
}
vector<string> submissionFiles;
void processSubmission() {
    string path(fileSubmission);
    // 获取该路径下的所有文件  
    getFiles(path, submissionFiles);
    int size = submissionFiles.size();
	cout << "size = " << size << endl;
    string stuId, hwId, Time, grade;
    ifstream in;
	for (int i = 0; i < size; i++) { // 访问每个班级文件 
		if (i % 1000 == 0) cout << i << endl;
        in.open((path+"//"+submissionFiles[i]).c_str());
		//cout << (path + "//" + submissionFiles[i]) << endl;
        if (!in) {
            cout << "Open Failed!!" << endl;
        }
        /*
        前四行信息，分别为： 
        提交学生ID（对应students目录内容）
        作业ID（对应assignments目录内容）
        提交时间
        分数
        */
        
        getline(in, stuId, '\n');
        getline(in, hwId, '\n');
        getline(in, Time, '\n');
        getline(in, grade, '\n');
   //     if (grade != "100") {
   //         in.close();
			//// 删掉该文件！！
			//remove((path + "//" + submissionFiles[i]).c_str());
   //         continue;
   //     }
          
		if (homeworks[hwId].top30Stu.size() < topNum) {
			homeworks[hwId].top30Stu.insert(stuId); // 就将其加入前30
			homeworks[hwId].top30Time.insert(Time);
			homeworks[hwId].time_stuId[Time] = stuId;
			homeworks[hwId].firstTime = Time;
		}
		else { // 已经有了前30名，需要比他们还早
			set<string, int>::reverse_iterator it = homeworks[hwId].top30Time.rbegin(); // it指向最晚一次提交
			if (Time <= *it) { // 比暂时保存的最后提交时间早
				homeworks[hwId].top30Stu.erase( homeworks[hwId].time_stuId[*it] );  // 删掉最晚时间对应的学生
				homeworks[hwId].top30Time.erase(*it);  // 删掉最晚时间
				homeworks[hwId].top30Time.insert(Time);  // 插入新的时间
				homeworks[hwId].top30Stu.insert(stuId);  // 插入新的学号
				homeworks[hwId].time_stuId[Time] = stuId;
				homeworks[hwId].firstTime = Time;
			}
			// 若比第30名还晚，则不操作
		}
        
        in.close();
        //cout<<files[i].c_str()<<endl;
    }
    
    map<string, homework>::iterator it = homeworks.begin();
    for (; it != homeworks.end(); it++) { // 遍历每次作业
		set<string>::iterator i = (it->second).top30Stu.begin();
		//int len = (it->second).top10Stu.size();
		
		if ((it->second).firstTime <= "2015-03-10") { // 说明是程设I
			float cnt = 3.5;
			for (; i != (it->second).top30Stu.end(); i++) { // 对每次作业的前30学生分别给予加分
				if (name_stu1[*i] != NULL) {
					name_stu1[*i]->grade_hw += cnt;
					cnt -= 0.1; // 采用一元一次线性模型，分数依次递减
				}
			}
		} else { // 说明是程设2
			float cnt = 3.5;
			for (; i != (it->second).top30Stu.end(); i++) { // 对每次作业的前30学生分别给予加分
				if (name_stu2[*i] != NULL) {
					name_stu2[*i]->grade_hw += cnt;
					cnt -= 0.1;
				}
			}
        }
    }
	srand(time(NULL));
	for (int i = 0; i < 500; i++) {
		stu1[i].grade += per_hw*stu1[i].grade_hw + rand() % 1;
		stu2[i].grade += per_hw*stu2[i].grade_hw + rand() % 1;
		//cout << stu1[i].id << " : " << stu1[i].grade_hw << endl;
		//cout << stu[i].id << " : " << stu[i].grade_hw << endl;
	}

    submissionFiles.clear();
}

int main() {
    preProcess();  // 把学生基本信息对应内容写入内存，便于下一步的计算分数
    getBasicInfo();  // 从Students文件中获取信息
    getInfoFromClass();  // 学生按班级划分
    processSubmission();  // 从Submissions文件中获取信息
	string resultPath = "prediction//";
    for (int i = 0; i < 10; i++) {
		// 每个班的学生按分数降序排列
        sort(class_stu[i].begin(), class_stu[i].end(), Greater());
        ofstream out;
		out.open((resultPath+class_name[i]).c_str());
        if (!out) {
            cout << "Write file open failed." << endl;
        }
		// 写出结果
        for (int j = 0; j < class_stu[i].size(); j++) {
            out << class_stu[i][j]->id << endl;
        }
        out.close();
    }
	system("pause");
	return 0;
}
