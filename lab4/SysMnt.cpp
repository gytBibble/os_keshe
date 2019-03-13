#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
//#include <gtkclist.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <sys/time.h>
#include <dirent.h>

using namespace std;
char *txt_pid2=NULL;
/*destroy a window 一个回调函数*/
gint destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit();
    return false;
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
    //vector<string> str;
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
/*get proc info*/
bool get_proc_info(GtkWidget *clist,vector<string> &str,vector<int> &proc_int){
    
    gtk_clist_set_column_title(GTK_CLIST(clist),0,"PID");
    gtk_clist_set_column_title(GTK_CLIST(clist),1,"名称");
	gtk_clist_set_column_title(GTK_CLIST(clist),2,"状态"); 
	gtk_clist_set_column_title(GTK_CLIST(clist),3,"优先级"); 
	gtk_clist_set_column_title(GTK_CLIST(clist),4,"占用内存"); 
    gtk_clist_set_column_width(GTK_CLIST(clist),0,50);
    gtk_clist_set_column_width(GTK_CLIST(clist),1,100);
    gtk_clist_set_column_width(GTK_CLIST(clist),2,50);
    gtk_clist_set_column_width(GTK_CLIST(clist),3,50);
    gtk_clist_set_column_width(GTK_CLIST(clist),4,50);
    gtk_clist_column_titles_show(GTK_CLIST(clist)); 
    char txt[5][100];
    gchar *tgt[5];
    DIR *dir;
    dir=opendir("/proc");
    struct dirent *ptr;
    while(ptr=readdir(dir)){
        
    }
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

void refresh2(GtkWidget *clist){
    vector<string> str;
    get_module_info(clist,str);
}

int main(int argc,char **argv){
    GtkWidget *window;  //定义窗口
    GtkWidget *table;   //定义组合表
    GtkWidget *notebook;//笔记本控件

    GtkWidget *scrolled_window;
    /*定义按钮*/
    GtkWidget *button1; //
    GtkWidget *button2; //
    GtkWidget *button3; //

    

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
    GtkWidget *frame1,*frame2,*frame3;

    frame1=gtk_frame_new("处理器信息");//一个新的框架
    gtk_container_set_border_width(GTK_CONTAINER(frame1),8);
    gtk_widget_set_size_request(frame1,500,150);
    //vendor_id:2  cpu name:5   cpu MHz:8   cache size :9
    vector<string> cpu_info;
    GetCpuInfo(cpu_info);
    string myCpuInfo=cpu_info[1]+"\n"+cpu_info[4]+"\n"+cpu_info[7]+"\n"+cpu_info[8];
    label = gtk_label_new(myCpuInfo.data());
    gtk_container_add(GTK_CONTAINER (frame1), label);
    //gtk_frame_set_label_align(GTK_FRAME(frame1),0.0,0.0);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox),frame1,TRUE,TRUE,5);
    gtk_widget_show(frame1);
    
    frame2 = gtk_frame_new("操作系统信息");
    gtk_container_set_border_width (GTK_CONTAINER (frame2), 8);
    gtk_widget_set_size_request (frame2, 500, 120);
    vector<string> system_info;
    GetSysInfo(system_info);
    string mySysInfo=system_info[0]+"\n"+system_info[1];
    label = gtk_label_new(mySysInfo.data());
    gtk_container_add(GTK_CONTAINER (frame2), label);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox),frame2,TRUE,TRUE,5);
    gtk_widget_show(frame2);

    frame3 = gtk_frame_new("---About---");
    gtk_container_set_border_width (GTK_CONTAINER (frame3), 8);
    gtk_widget_set_size_request (frame3, 500, 120);
    //vector<string> system_info;
    //GetSysInfo(system_info);
    string myInfo="SysMnt Pro. Version\nCopyRight@2019 gyt";
    label = gtk_label_new(myInfo.data());
    gtk_container_add(GTK_CONTAINER (frame3), label);
    gtk_widget_show(label);
    gtk_box_pack_start(GTK_BOX(vbox),frame3,TRUE,TRUE,5);
    gtk_widget_show(frame3);

    gtk_widget_show_all(vbox);
    label = gtk_label_new ("系统信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);








    /*进程信息*/
    vbox = gtk_vbox_new(FALSE,0);

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolled_window, 300, 300);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);   
    
    vector<string> proc_str;
    vector<int> proc_int;
    GtkWidget *clist_proc;
    clist_proc=gtk_clist_new(5);
    get_proc_info(clist_proc,proc_str,proc_int);

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),clist_proc); 
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

    gtk_widget_show(scrolled_window);

    gtk_widget_show_all(vbox);
    label = gtk_label_new ("进程信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);






    /*模块信息*/
    vbox = gtk_vbox_new(FALSE,0);
    
   
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_set_size_request (scrolled_window, 500, 250);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);   
    
    vector<string> module_str;
    GtkWidget *clist;
    clist=gtk_clist_new(3);
    get_module_info(clist,module_str);
    //gtk_signal_connect(GTK_OBJECT(clist),"select_row",G_CALLBACK(select_row_callback2),NULL);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),clist); 
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

    gtk_widget_show(scrolled_window);

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

    gtk_widget_show_all(vbox);
    label = gtk_label_new ("内存资源信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);
    
    gtk_widget_show (table);
    gtk_widget_show (window);
    gtk_main ();
    return 0;
}