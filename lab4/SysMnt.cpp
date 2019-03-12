#include<iostream>
#include<vector>
#include<string>

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

/*destroy a window 一个回调函数*/
gint destroy(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit();
    return false;
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
        str.push_back(text);
    }
    return true;
}

int main(int argc,char **argv){
    GtkWidget *window;  //定义窗口
    GtkWidget *tabel;   //定义组合表
    GtkWidget *notebook;//笔记本控件

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
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy),NULL);//处理信号
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


    GtkWidget *frame1;

    /*系统信息:处理器信息、操作系统信息、About...*/
    frame1=gtk_frame_new("处理器信息");//一个新的框架
    gtk_container_set_border_width(GTK_CONTAINER(frame1),8);
    gtk_widget_set_size_request(frame1,500,150);

    //vendor_id:2  
    //cpu name:5
    //cpu MHz:8
    //cache size :9
    vector<string> cpu_info;
    GetCpuInfo(cpu_info);
    string myCpuInfo=cpu_info[1]+cpu_info[4]+cpu_info[7]+cpu_info[8];
    label = gtk_label_new (myCpuInfo.c_str());
    gtk_container_add (GTK_CONTAINER (frame1), label);
    gtk_widget_show (label);
    gtk_box_pack_start(GTK_BOX(vbox),frame1,FALSE,FALSE,5);
    gtk_widget_show (frame1);
    

    gtk_widget_show (vbox);
    label = gtk_label_new ("系统信息");
    gtk_notebook_append_page (GTK_NOTEBOOK (notebook), vbox, label);

    /*进程信息*/

    /*模块信息*/

    /*内存资源信息*/

    
    gtk_widget_show (table);
    gtk_widget_show (window);
    gtk_main ();
    return 0;
}