#include<bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
using namespace std;
/*38个城市的tsp问题
参数设定为:
s=250; %初始种群大小
t=38; %第 1～38 列表示某一路径依次经过的城市编号
selectprob=0.01; %选择概率
crossprob=0.08; %交叉概率
mutateprob=0.002; %变异概率
maxgen=50; %最大遗传代数

上述参数除了s外都为一个水平，一共有12个水平.
*/
double eps = 1e-9;
int a[300][40];//a[i][j]表示第i个个体的每一个城市值
double x[40];
double y[40];
double M[1000];//第i代的最小值
double A[1000];//第i代的平均值
double inf = (double)1e18;
int ans[40][40];//均匀设计表，为12 * 4的大小；
int prime[20];//用欧拉函数产生的与n互质的整数
int gcd(int x1, int x2)
{
    return x1 == 0 ? x2 : gcd(x2 % x1, x1);
}
double getDis(int i, int j)
{
    return sqrt((x[i] - x[j]) * (x[i] - x[j]) + (y[i] - y[j]) * (y[i] - y[j]));
}
int term[40];
vector<int> v1, v2, v3, v4, s;//用于交叉算子
struct node
{
    int id;//在数组中的序号
    double d;//该个体的适应值在这里表示路径长度值
} Node[300];
double selectprob = 0.01;//选择概率
int k;//每次从种群中选取适应值最大（路径长度最小的）k个代替最小的k个，k = s * selectprob
double crossprob = 0.08;//交叉概率
double mutateprob = 0.002;//变异概率
int maxgen = 50; //%最大遗传代数
bool cmp(node n1, node n2)//排序函数,按个体的适应值从小到达排序
{
    return n1.d < n2.d;
}
void select()//选择算子，用适应值最大的k个直接代替最小的k个
{
    sort(Node + 1, Node + 251, cmp);
    for(int i = 1, j = 250; i <= k; i++, j--)
    {
        Node[j] = Node[i];
    }
}
void cross(int x1, int x2)//交叉算子,从种群中随机选取两个双亲f1, f2进行交叉，得到两个子代s1 , s2
{
    /*
    已知两个父代
    p1(m11,m12, m13, ⋯, m1n)， p2 (m21, m22, m23, ⋯, m2n)，
    算法产生后代p1s和p2s的过程如下：
    （1）随机产生一个城市d作为交叉起点，把d作为p1s和
    p2s的起始点。
    （2）分别从p1和p2中找出d的右城市dr1和dr2，并计算
    (d,dr1)和(d,dr2)的距离j1和j2。
    （3）如果j1<j2 ，则把dr1作为p1s的第二个点，从p1和
    p2中删除d，并且把当前点改为dr1，转步骤(5)。
    （4）如果j1>j2 ，则把dr2作为p1s的第二个点，从p1和
    p2中删除d，并且把当前点改为dr2。
    （5）若此时p1和p2的个数为1，结束，否则回到(2)继续
    执行。
    同理，把(2)中的右城市改成左城市d11和d12，通过计算
    (d,dl1)和(d,d12)的距离并比较大小来确定子代p2s。
    */
    srand((unsigned)time(NULL)); //随机数种子
    int f1 = x1;
    int f2 = x2;
    int d = (rand() % 38) + 1;//随机产生交叉起始位置
    int dx = d;
    //先产生s1
    v1.clear();
    v2.clear();
    v3.clear();
    v4.clear();

    for(int i = 1; i <= 38; i++)
    {
        node nx = Node[f1];
        int id = nx.id;
        v1.push_back(a[id][i]);
        v3.push_back(a[id][i]);
    }

    for(int i = 1; i <= 38; i++)
    {
        node nx = Node[f2];
        int id = nx.id;
        v2.push_back(a[id][i]);
        v4.push_back(a[id][i]);
    }

    int loc1, loc2;//d分别在f1, f2中的位置
    int num = 0;
    while(v1.size() > 1)
    {
        //cout<<"v1.size() == "<<v1.size()<<endl;
        for(int i = 0; i < v1.size(); i++)
        {
            if(d == v1[i])
            {
                loc1 = i;
                break;
            }
        }
        for(int i = 0; i < v2.size(); i++)
        {
            if(d == v2[i])
            {
                loc2 = i;
                break;
            }
        }
        int last1, last2;
        if(loc1 == v1.size() - 1) last1 = 0;
        else last1 = loc1 + 1;
        if(loc2 == v2.size() - 1) last2 = 0;
        else last2 = loc2 + 1;
        term[++num] = d;
        double d1 = getDis(d, v1[last1]);
        double d2 = getDis(d, v2[last2]);
        if(d1 - d2 < eps)
        {
            d = v1[last1];
        }
        else d = v2[last2];
        v1.erase(v1.begin() + loc1);
        v2.erase(v2.begin() + loc2);
    }
    term[++num] = v1[0];
    for(int i = 1; i <= 38; i++)
    {
        node nx = Node[f1];
        int id = nx.id;
        a[id][i] = term[i];
    }
    double xx = 0;
    for(int i = 2; i <= 38; i++)
    {
        xx = xx + getDis(term[i], term[i - 1]);
    }
    xx = xx + getDis(term[38], term[1]);
    Node[f1].d = xx;
    //然后产生s2
    num = 0;
    d = dx;
    while(v3.size() > 1)
    {
        for(int i = 0; i < v3.size(); i++)
        {
            if(d == v3[i])
            {
                loc1 = i;
                break;
            }
        }
        for(int i = 0; i < v4.size(); i++)
        {
            if(d == v4[i])
            {
                loc2 = i;
                break;
            }
        }
        int last1, last2;
        if(loc1 == 0) last1 = v3.size() - 1;
        else last1 = loc1 - 1;
        if(loc2 == 0) last2 = v4.size() - 1;
        else last2 = loc2 - 1;
        term[++num] = d;
        double d1 = getDis(d, v3[last1]);
        double d2 = getDis(d, v4[last2]);
        if(d1 - d2 < eps)
        {
            d = v3[last1];
        }
        else d = v4[last2];
        v3.erase(v3.begin() + loc1);
        v4.erase(v4.begin() + loc2);
    }
    //cout<<"a"<<endl;
    term[++num] = v3[0];
    for(int i = 1; i <= 38; i++)
    {
        node nx = Node[f2];
        int id = nx.id;
        a[id][i] = term[i];
    }
    xx = 0;
    for(int i = 2; i <= 38; i++)
    {
        xx = xx + getDis(term[i], term[i - 1]);
    }
    xx = xx + getDis(term[38], term[1]);
    Node[f2].d = xx;
}
/*
采用一种贪婪倒位变异，首先确
定一个变异起始点c1，然后在除去起始点右侧的点的那些后
来的点中找到与起始点距离最短的点c2，最后逆序存放c1到
c2的那些城市编码顺序，完成变异。同时根据适应值来评估
变异结果，如果倒位后该个体适应值没有优化，则对该个体
不进行变异操作。倒位原理是：先随机选择父辈个体的两个
变异点，然后对一个点之间的城市编码逆序存放，产生变异
后的个体。例如，父代个体为pf(2 6 5 1 3 7 4)，随机选择
的变异点为6，通过距离表得到1、3、7、4中距离6最近的是7，
则把7作为第二变异点，交换它们之间的城市顺序，变异后的
个体为ps(2 6 7 3 1 5 4)。
*/
void mutate(int x1)//变异算子
{
    //cout<<"mutate()"<<endl;
    srand((unsigned)time(NULL)); //随机数种子
    int f = x1;
    v1.clear();
    int d = (rand() % 38 + 1);//变异起始位置
    int d1 = -1;//变异终止位置
    double Min = inf;
    node nx = Node[f];
    int id = nx.id;
    double w = nx.d;
    for(int i = d + 2; i <= 38; i++)
    {
        double ans = getDis(a[id][d], a[id][i]);
        if(ans < Min)
        {
            d1 = i;
            Min = ans;
        }
    }
    if(d1 != -1)
    {
        int res = 0;
        for(int i = 1; i <= d; i++)
        {
            term[i] = a[id][i];
        }
        for(int i = d1; i <= 38; i++)
        {
            term[i] = a[id][i];
        }
        for(int i = d1 - 1, j = d + 1; i > d; i--, j++)
        {
            term[j] = a[id][i];
        }
        double ww = 0;
        for(int i = 2; i <= 38; i++)
        {
            ww = ww + getDis(term[i], term[i - 1]);
        }
        ww = ww + getDis(term[38], term[1]);
        if(ww - w < eps)
        {
            Node[f].d = ww;
            for(int i = 1; i <= 38; i++)
            {
                a[id][i] = term[i];
            }
        }
    }
}
double ga(double se, double cro, double muta, int maxg, int type)
{
    //cout<<"maxg == "<<maxg<<endl;
    freopen("ga.txt", "r", stdin);
    //cout<<"ga"<<endl;
    //freopen("out.txt", "w", stdout);
    int xxx;
    for(int i = 1; i <= 38; i++)
    {
        scanf("%d/%lf/%lf", &xxx, &x[i], &y[i]);
        //printf("%d %.6lf %.6lf\n", xxx , x[i], y[i]);
    }


    k = 250 * se;
    //cout<<"k == "<<k<<endl;
    //k = 50;
    srand((unsigned)time(NULL)); //随机数种子
    for(int i = 1; i <= 38; i++)
    {
        term[i] = i;
    }

    int c[40];
    //初始化种群
    for(int i = 1; i <= 250; i++)
    {
        for(int j = 1; j <= 38; j++)
        {
            c[j] = term[j];
        }
        for(int j = 1; j <= 100; j++)
        {
            int ran1 = rand() % 38 + 1;
            int ran2 = rand() % 38 + 1;
            swap(c[ran1], c[ran2]);
        }
        for(int j = 1; j <= 38; j++)
        {
            a[i][j] = c[j];
        }
        Node[i].id = i;
        double h = 0;
        for(int j = 2; j <= 38; j++)
        {
            h = h + getDis(c[j], c[j - 1]);
            //cout<<"dis == "<<getDis(c[j], c[j - 1])<<endl;
        }
        h = h + getDis(c[38], c[1]);
        Node[i].d = h;
        //cout<<"Node[i].d == "<<Node[i].d<<endl;

    }
    int t = 1;//代数
    while(t <= maxg)
    {
        //cout<<"t == "<<t<<endl;
       int ran;
       //交叉
       s.clear();
       for(int i = 1; i <= 250; i++)
       {
           ran = (rand() % 100) + 1;
           double pp = (double)ran / 100.0;
           if(pp < cro)
           {
               s.push_back(i);
           }
       }
       while(s.size() >= 2)
       {
           int x1 = s[0];
           s.erase(s.begin());
           int Size = s.size();
           ran = rand() % Size;
           int x2 = s[ran];
           s.erase(s.begin() + ran);
           cross(x1, x2);

       }
       //变异
       s.clear();
       for(int i = 1; i <= 250; i++)
       {
           ran = rand() % 100 + 1;
           double pp = (double)pp / 100.0;
           if(pp <= muta)
           {
              s.push_back(i);
           }
       }
       for(int i = 0; i < s.size(); i++)
       {
          mutate(s[i]);
       }
       //选择
       ran = rand() % 100 + 1;
       double pp = (double)ran / 100.0;
       if(pp < se)
       {
           select();
       }
       double sum = 0;
       double Min = (double)inf;
       for(int i = 1; i <= 250; i++)
       {
           sum += Node[i].d;
           if(Node[i].d < Min) Min = Node[i].d;
       }
       M[t] = Min;
       A[t] = sum / 250;
       t++;
    }
    //cout<<"bbbbb"<<endl;
    int re = 1;
    double path = inf;
    for(int i = 1; i <= 250; i++)
    {
        //printf("Node[%d].d == %.3lf\n",i, Node[i].d);
        if(path > Node[i].d /*&& fabs(Node[i].d) > eps*/)
        {
            re = Node[i].id;
            path = Node[i].d;
        }
    }
    path = 0;
    for(int i = 2; i <= 38; i++)
    {
        path = path + getDis(a[re][i], a[re][i - 1]);
    }
    path += getDis(a[re][38], a[re][1]);
    //cout<<"aaa"<<endl;
    //cout<<"path == "<<path<<endl;
    if(type == 1)//画图
    {
       printf("路径长度为：\n");
       cout<<path<<endl;
       printf("路径为：\n");
       printf("%d", a[re][1]);
       for(int i = 2; i <= 38; i++)
       {
         printf("->%d", a[re][i]);
       }
       printf("\n");
       freopen("Min.txt", "w", stdout);
       for(int i = 1; i < maxg; i++)
       {
         printf("%d %.4lf\n", i, M[i]);
         printf("%d %.4lf\n", i + 1, M[i + 1]);
         printf("\n");
       }
       freopen("average.txt", "w", stdout);
       for(int i = 1; i <= maxg; i++)
       {
         printf("%d %.4lf\n", i, A[i]);
         printf("%d %.4lf\n", i + 1, A[i + 1]);
         printf("\n");
       }
       freopen("plot.txt", "w", stdout);
       for(int i = 1; i <= 37; i++)
       {
         int no = a[re][i];
         int no1 = a[re][i + 1];
         printf("%.5lf %.5lf\n",x[no], y[no]);
         printf("%.5lf %.5lf\n",x[no1], y[no1]);
         printf("\n");
       }
       int no = a[re][38];
       int no1 = a[re][1];
       printf("%.5lf %.5lf\n",x[no], y[no]);  
       printf("%.5lf %.5lf\n",x[no1], y[no1]);
       fclose (stdout);
       FILE *fp=popen("gnuplot","w");
       char *aa="plot for [i=0:37] \"plot.txt\" index i with lines\n";
       fprintf(fp,"%s",aa);
       fflush(fp);
       sleep(1000);
    }
    return path;
}
int main()
{
    int res = 0;
    for(int i = 1; i <= 12; i++)
    {
        if(gcd(i, 12) == 1) prime[++res] = i;
    }
    for(int i = 1; i <= res; i++)
    {
        for(int j = 1; j <= 12; j++)
        {
            ans[j][i] = (prime[i] * j) % 12;
            if(ans[j][i] == 0) ans[j][i] = 12;
        }
    }
    printf("均匀表为：\n");
    for(int i = 1; i <= 12; i++)
    {
        for(int j = 1; j <= res; j++)
        {
            printf("%d ", ans[i][j]);
        }
        printf("\n");
    }
    printf("正在根据均匀表寻找最优解......\n");
    double sum[20];
    for(int i = 1; i <= 12; i++)
    {
        //cout<<"i == "<<i<<endl;
        double se = selectprob * ans[i][1];
        double cro = crossprob * ans[i][2];
        double muta = mutateprob * ans[i][3];
        int maxg = maxgen * ans[i][4];

        sum[i] = 0;//进行5次，取平均值
        for(int j = 1; j <= 5; j++)
        {
            //cout<<"j == "<<j<<endl;
            sum[i] += ga(se, cro, muta, maxg,0);
        }
        sum[i] = sum[i] / 5.0;
    }
    double Min = inf;
    int loc = -1;
    for(int i = 1; i <= 12; i++)
    {
        if(sum[i] < Min)
        {
            Min = sum[i];
            loc = i;
        }
    }
    printf("根据均匀设计找到最佳组合：\n");
    printf("当选择概率，交叉概率，变异概率，遗传代数如下时，可以得到最优解\n");

    printf("选择概率为：%.3lf\n", ans[loc][1] * selectprob);
    printf("交叉概率为：%.3lf\n", ans[loc][2] * crossprob);
    printf("变异概率为：%.3lf\n", ans[loc][3] * mutateprob);
    printf("遗传代数为：%d\n", ans[loc][4] * maxgen);
    printf("最优平均值为：%.3lf\n", Min);
    printf("在最优组合下，再进行一次实验结果如下:\n");
    ga(ans[loc][1] * selectprob, ans[loc][2] * crossprob, ans[loc][3] * mutateprob, ans[loc][4] * maxgen, 1);
    

    return 0;
}

