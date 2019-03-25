#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/time.h>
#include <dirent.h>

using namespace std;

char *txt_pid=NULL;
char *txt_pid2=NULL;

static char temp_cpu[50];	//cpu使用率
static char temp_mem[50];	//内存使用
static long idle,total;		//计算cpu时的数据
static int flag=0;		    //计算cpu使用率时启动程序的标志
static int flag1=0;		    //计算单个进程cpu使用率时使用的标志
static long mem_total;		//内存总大小
static long mem_free;		//空闲内存
static float cpu_used_percent=0;	//cpu使用率

/*destroy a window 一个回调函数*/
gint destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit();
    return false;
}
void select_row_callback(GtkWidget *clist,gint row,gint column,GdkEventButton *event,gpointer data)
{
	
	gtk_clist_get_text(GTK_CLIST(clist),row,column,&txt_pid);
	printf("%s\n",txt_pid);
}
void select_row_callback2(GtkWidget *clist,gint row,gint column,GdkEventButton *event,gpointer data)
{
	
	gtk_clist_get_text(GTK_CLIST(clist),row,column,&txt_pid2);
	printf("%s\n",txt_pid2);
}
/*get cpu info*/
bool GetCpuInfo(vector<string>& str){
    ifstream text;
    text.open("/proc/cpuinfo",ios::in);
    str.clear();
    while(!text.eof()){
        string inbuf;
        getline(text,inbuf,'\n');
        str.push_back(inbuf);
    }
    text.close();
    return true;
}
/*get sys info*/
bool GetSysInfo(vector<string>& str){
    str.clear();
    ifstream text;
    text.open("/etc/issue",ios::in);
    string inbuf;
    getline(text,inbuf,'\\');
    str.push_back("操作系统类型\t: "+inbuf);
    text.close();
    text.open("/proc/version",ios::in);
    getline(text,inbuf,'#');
    str.push_back("操作系统版本\t: "+inbuf);
    text.close();
    return true;
}
char* stat_read()
{
	long  user_t, nice_t, system_t, idle_t,total_t;	//此次读取的数据
	long total_c,idle_c;	//此次数据与上次数据的差
	char cpu_t[10],buffer[70+1];		
	int fd;
	fd=open("/proc/stat",O_RDONLY);
	read(fd,buffer,70);
	sscanf(buffer, "%s %ld %ld %ld %ld", cpu_t, &user_t, &nice_t, &system_t, &idle_t);

	if(flag==0)	
	{
		flag=1;	
		idle=idle_t;
		total=user_t+nice_t+system_t+idle_t;
		cpu_used_percent=0;	
	}
	else
	{
		total_t=user_t+nice_t+system_t+idle_t;
		total_c=total_t-total;
		idle_c=idle_t-idle;	
		cpu_used_percent=100*(total_c-idle_c)/total_c;
		total=total_t;	//此次数据保存
		idle=idle_t;
	}
	close(fd);	
	sprintf(temp_cpu,"cpu使用率：%0.1f%%",cpu_used_percent);
	return temp_cpu;
}
char* meminfo_read()
{
	char buffer[100+1];
	char data[20];	
	long total=0,free=0;	//总内存和用户内存
	int i=0,j=0,counter=0;
	int fd;
	fd=open("/proc/meminfo",O_RDONLY);
	read(fd,buffer,100);	
	
	for(i=0,j=0;i<100;i++,j++)
	{
		if (buffer[i]==':') counter++;
		if (buffer[i]==':'&&counter==1)	//MemTotal总内存
		{
			while(buffer[++i]==' ');
			for(j=0;j<20;j++,i++)
			{
				if(buffer[i]=='k')  break;
				data[j]=buffer[i];
			}
			data[--j]='\0';
			total=atol(data)/1024;
		}

		if (buffer[i]==':'&&counter==2)	//MemFree空闲内存
		{
			while(buffer[++i]==' ');
			for(j=0;j<20;j++,i++)
			{
				if(buffer[i]=='k')  break;
				data[j]=buffer[i];
			}
			data[--j]='\0';
			free+=atol(data)/1024;
		}

		if (buffer[i]==':'&&counter==3)	//Buffers设备缓冲
		{
			while(buffer[++i]==' ');
			for(j=0;j<20;j++,i++)
			{
				if(buffer[i]=='k')  break;
				data[j]=buffer[i];
			}
			data[--j]='\0';
			free+=atol(data)/1024;
		}

		if (buffer[i]==':'&&counter==4)	//Cached高速缓冲
		{
			while(buffer[++i]==' ');
			for(j=0;j<20;j++,i++)
			{
				if(buffer[i]=='k')  break;
				data[j]=buffer[i];
			}
			data[--j]='\0';
			free+=atol(data)/1024;
		}
	}
	mem_total=total;
	mem_free=free;
	sprintf(temp_mem,"内存:%ldM/%ldM",total-free,total);
	close(fd);	
	return temp_mem;
}
gint cpu_refresh(GtkWidget* cpu_label)
{
	gtk_label_set_text(GTK_LABEL(cpu_label),stat_read());
	gtk_widget_show(cpu_label);
	return TRUE;
}
gint mem_refresh(GtkWidget* mem_label)
{
	gtk_label_set_text(GTK_LABEL(mem_label),meminfo_read());	
	gtk_widget_show(mem_label);
	return TRUE;
}
bool GetMemInfo(vector<string>& str){
    str.clear();
    string a="",b="";
    str.push_back(a+stat_read());
    str.push_back(b+meminfo_read());
}
/*get proc info*/
bool get_proc_info(GtkWidget *clist,vector<string> &str,vector<int> &proc_int){
    proc_int.clear();
    for(int i=0;i<4;i++)proc_int.push_back(0);
    char buf[1024];
    gtk_clist_set_column_title(GTK_CLIST(clist),0,"PID");
    gtk_clist_set_column_title(GTK_CLIST(clist),1,"名称");
	gtk_clist_set_column_title(GTK_CLIST(clist),2,"状态"); 
	gtk_clist_set_column_title(GTK_CLIST(clist),3,"优先级"); 
	gtk_clist_set_column_title(GTK_CLIST(clist),4,"占用内存"); 
    gtk_clist_set_column_width(GTK_CLIST(clist),0,35);
    gtk_clist_set_column_width(GTK_CLIST(clist),1,140);
    gtk_clist_set_column_width(GTK_CLIST(clist),2,40);
    gtk_clist_set_column_width(GTK_CLIST(clist),3,50);
    gtk_clist_set_column_width(GTK_CLIST(clist),4,60);
    gtk_clist_column_titles_show(GTK_CLIST(clist)); 
    char txt[5][100];
    gchar *tgt[5];
    DIR *dir;
    dir=opendir("/proc");
    struct dirent *ptr;
    while(ptr=readdir(dir)){
        if((ptr->d_name)[0]>='0'&&(ptr->d_name)[0]<='9'){
            proc_int[0]++;
            str.clear();
            sprintf(buf,"/proc/%s/stat",ptr->d_name);
            ifstream text;
            text.open(buf,ios::in);
            string inbuf,result;
            getline(text,inbuf,'\n');
            stringstream input(inbuf);
            while(input>>result){
                str.push_back(result);
            }
            //pid
            str[0].copy(txt[0],str[0].length(),0);
            *(txt[0]+str[0].length())='\0';
            tgt[0]=txt[0];

            //name
            string pid_name;
            int j=0;
            for(int i=0;i<str[1].length();i++){
                if(str[1][i]!='('&&str[1][i]!=')')pid_name+=str[1][i];
            }
            while(str[j+1][str[j+1].length()-1]!=')'){
                pid_name+=' ';
                for(int i=0;i<str[j+2].length();i++){
                    if(str[j+2][i]==')')break;
                    pid_name+=str[j+2][i];
                }
                j++;
               
            }
            pid_name.copy(txt[1],pid_name.length(),0);
            *(txt[1]+pid_name.length())='\0';
            tgt[1]=txt[1];
            
            str[2+j].copy(txt[2],str[17+j].length(),0);
            *(txt[2]+str[2+j].length())='\0';
            tgt[2]=txt[2];

            str[17+j].copy(txt[3],str[17+j].length(),0);
            *(txt[3]+str[17+j].length())='\0';
            tgt[3]=txt[3];//pri:18

            str[23+j].copy(txt[4],str[23+j].length(),0);
            *(txt[4]+str[23+j].length())='\0';
            tgt[4]=txt[4];//24

            if(str[2]=="R")proc_int[1]++;
            else if(str[2]=="S")proc_int[2]++;
			else if(str[2]=="Z")proc_int[3]++;
            //else if(str[2]=="D")
            gtk_clist_append(GTK_CLIST(clist),tgt);
            text.close();
        }
    }
    closedir(dir);
}
void kill_proc(void)
{
    char buf[20];
	sprintf(buf,"kill -9 %s",txt_pid);
    system(buf);
}
/*get module info*/
bool get_module_info(GtkWidget *clist,vector<string> &str){

    gtk_clist_set_column_title(GTK_CLIST(clist),0,"模块名称");
	gtk_clist_set_column_title(GTK_CLIST(clist),1,"占用内存");
	gtk_clist_set_column_title(GTK_CLIST(clist),2,"使用次数");
	gtk_clist_set_column_width(GTK_CLIST(clist),0,200);
	gtk_clist_set_column_width(GTK_CLIST(clist),1,150);
	gtk_clist_set_column_width(GTK_CLIST(clist),2,150);
	gtk_clist_column_titles_show(GTK_CLIST(clist));

    char txt[3][100];
    gchar *tgt[3];
    
    ifstream text;
    text.open("/proc/modules",ios::in);
    while(!text.eof()){
        str.clear();
        string inbuf,result;
        getline(text,inbuf,'\n');
        stringstream input(inbuf);
        while(input>>result){
           str.push_back(result);
        }
        for(int i=0;i<3;i++){
            str[i].copy(txt[i],str[i].length(),0);
            *(txt[i]+str[i].length())='\0';
            tgt[i]=txt[i];
        }
        gtk_clist_append(GTK_CLIST(clist),tgt);
    }
    text.close();
    return true;
}
void refresh(GtkWidget *clist){
    gtk_clist_clear(GTK_CLIST(clist));
    vector<string> str;vector<int> proc_int;
    get_proc_info(clist,str,proc_int);
}
void refresh2(GtkWidget *clist){
    gtk_clist_clear(GTK_CLIST(clist));
    vector<string> str;
    get_module_info(clist,str);
}

int main(int argc,char **argv){
    /*声明要使用的控件*/
    GtkWidget *window;  //定义窗口
    GtkWidget *table;   //定义组合表
    GtkWidget *notebook;//笔记本控件
    GtkWidget *scrolled_window;//窗口滑动   
    GtkWidget *cpu_label;	//cpu使用率
    GtkWidget *mem_label;	//内存使用情况
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *label;

    /*初始化*/ 
    gtk_init(&argc,&argv);//这个函数在所有的GTK程序都要调用，参数由命令行中解析出来并且送到该程序中
    vbox=gtk_vbox_new(FALSE,0);

    /*创建一个新的窗口*/
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建一个窗口
    gtk_signal_connect(GTK_OBJECT(window), "destroy", G_CALLBACK(destroy),NULL);//处理信号
    gtk_window_set_title(GTK_WINDOW(window),"SysMnt");//设置窗口的标题
    gtk_window_set_default_size(GTK_WINDOW(window),550,450);//设置窗口默认大小为长宽为550*450像素
    gtk_container_set_border_width (GTK_CONTAINER (window),8);//设置边框宽度

    /*组合表、笔记本控件，可切换标签置于顶部*/
    table=gtk_table_new(3,6,FALSE);//3行6列
    gtk_container_add (GTK_CONTAINER (window), table);
    notebook=gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);
    gtk_table_attach_defaults(GTK_TABLE(table),notebook,0,6,0,1);//将笔记本控件加到表中第一行
    gtk_widget_show(notebook);//显示内容

    /*系统信息:处理器信息、操作系统信息、About...*/
    GtkWidget *frame1,*frame2,*frame3;//利用三个框架分别展示信息

    frame1=gtk_frame_new("处理器信息");//一个新的框架，标题信息
    gtk_container_set_border_width(GTK_CONTAINER(frame1),8);
    gtk_widget_set_size_request(frame1,500,150);
    //vendor_id:2  cpu name:5   cpu MHz:8   cache size :9
    vector<string> cpu_info;
    GetCpuInfo(cpu_info);//获取信息保存在数组cpu_info中
    string myCpuInfo=cpu_info[1]+"\n"+cpu_info[4]+"\n"+cpu_info[7]+"\n"+cpu_info[8];
    label = gtk_label_new(myCpuInfo.data());
    gtk_container_add(GTK_CONTAINER (frame1), label);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox),frame1,TRUE,TRUE,5);
    gtk_widget_show(frame1);
    
    frame2 = gtk_frame_new("操作系统信息");//一个新的框架，标题信息
    gtk_container_set_border_width (GTK_CONTAINER (frame2), 8);
    gtk_widget_set_size_request (frame2, 500, 120);
    vector<string> system_info;
    GetSysInfo(system_info);//获取信息保存在system_info中
    string mySysInfo=system_info[0]+"\n"+system_info[1];
    label = gtk_label_new(mySysInfo.data());
    gtk_container_add(GTK_CONTAINER (frame2), label);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox),frame2,TRUE,TRUE,5);
    gtk_widget_show(frame2);

    frame3 = gtk_frame_new("---About---");//一个新的框架，标题信息
    gtk_container_set_border_width (GTK_CONTAINER (frame3), 8);
    gtk_widget_set_size_request (frame3, 500, 120);
    string myInfo="SysMnt Pro. Version\nCopyRight@2019 gyt";
    label = gtk_label_new(myInfo.data());
    gtk_container_add(GTK_CONTAINER (frame3), label);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox),frame3,TRUE,TRUE,5);
    gtk_widget_show(frame3);

    gtk_widget_show_all(vbox);//展示容器所有内容
    label = gtk_label_new ("系统信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);


    /*进程信息*/
    hbox = gtk_hbox_new(FALSE,0);

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolled_window, 325, 325);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);   
    vector<string> proc_str;
    vector<int> proc_int;
    GtkWidget *clist_proc;
    clist_proc=gtk_clist_new(5);//列表内容实现
    get_proc_info(clist_proc,proc_str,proc_int);//获取进程信息
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),clist_proc); 
    gtk_box_pack_start(GTK_BOX(hbox), scrolled_window, TRUE, TRUE, 5);
    gtk_widget_show(scrolled_window);


    vbox=gtk_vbox_new(FALSE, 5);
    GtkWidget *frame;
    frame = gtk_frame_new("");
    gtk_widget_set_size_request(frame, 100, 215);
    char bufferf1[1024];
    sprintf(bufferf1,"All process：%d\n\nRunning：%d\n\nSleeping：%d\n\nZombied：%d\n",proc_int[0],proc_int[1],proc_int[2],proc_int[3]);
    label = gtk_label_new (bufferf1);
    GtkWidget *button1;//结束进程的按钮
    gtk_container_add (GTK_CONTAINER (frame), label);
    gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 10);
    button1 = gtk_button_new_with_label("结束进程");
    g_signal_connect(G_OBJECT(clist_proc),"select_row",G_CALLBACK(select_row_callback),NULL);
    g_signal_connect(G_OBJECT (button1), "clicked",G_CALLBACK (kill_proc),NULL);//调用kill_proc杀死进程
    gtk_box_pack_start(GTK_BOX(vbox), button1, FALSE, FALSE, 10);
    GtkWidget *button2;//刷新进程的按钮
    button2 = gtk_button_new_with_label("刷新");
    g_signal_connect_swapped (G_OBJECT (button2), "clicked",G_CALLBACK (refresh), clist_proc);

    gtk_box_pack_start(GTK_BOX(vbox), button2, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 5);

    gtk_widget_show_all(hbox);
    label = gtk_label_new ("进程信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), hbox, label);


    /*模块信息*/
    vbox = gtk_vbox_new(FALSE,0);
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolled_window, 500, 250);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);   
    
    vector<string> module_str;
    GtkWidget *clist;
    clist=gtk_clist_new(3);
    get_module_info(clist,module_str);
    gtk_signal_connect(GTK_OBJECT(clist),"select_row",G_CALLBACK(select_row_callback2),NULL);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),clist); 
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);
    gtk_widget_show(scrolled_window);
    GtkWidget *button3;//刷新按钮
    hbox=gtk_hbox_new(FALSE, 0);
    button3 = gtk_button_new_with_label("刷新");
    g_signal_connect_swapped(GTK_OBJECT (button3),"clicked",G_CALLBACK(refresh2), clist);
    gtk_box_pack_end(GTK_BOX(hbox), button3, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, TRUE, 0);
    
    gtk_widget_show_all(vbox);
    label = gtk_label_new("模块信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);

    /*内存资源信息*/
    vbox = gtk_vbox_new(FALSE,0);
    cpu_label = gtk_label_new("");
    mem_label = gtk_label_new("");
    gtk_timeout_add(1000,(GtkFunction)cpu_refresh,cpu_label);	//cpu使用率刷新
    gtk_timeout_add(1000,(GtkFunction)mem_refresh,mem_label);	//内存使用刷新
    gtk_widget_show(cpu_label);
    gtk_widget_show(mem_label);
    gtk_box_pack_start(GTK_BOX(vbox),cpu_label,FALSE,FALSE,10);
    gtk_box_pack_start(GTK_BOX(vbox),mem_label,FALSE,FALSE,10);
    gtk_widget_show_all(vbox);
    label = gtk_label_new ("内存资源信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);
    
    gtk_widget_show (table);
    gtk_widget_show (window);
    gtk_main ();
    return 0;
}