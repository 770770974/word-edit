#ifndef MY_WORD_H
#define MY_WORD_H

#include <QMainWindow>
#include <QPrinter>
#include "my_child.h"


//对所用到的类进行声明
class QMdiArea;
class QMenu;
class QAction;
class QComboBox;
class QFontComboBox;
class my_child;
class QMdiSubWindow;
class QSignalMapper;

class my_word : public QMainWindow
{
    Q_OBJECT

public:
    my_word(QWidget *parent = nullptr);
    ~my_word();


protected:
    void closeEvent(QCloseEvent *event);    //重写关闭函数

private slots:
    void textAlign(QAction *a);

    //创建文件子窗口

    my_child *createMyChild();  //创建子类

    void updateMenus();    //更新菜单（通知主窗口：当前活动子窗口更改）
    void updateWindowMenu();    //更新菜单栏中的窗口菜单

    void fileNew();     //新建
    void fileOpen();   //打开
    void fileSave();    //保存
    void fileSaveAs();     //另存为
    void undo();        //撤销
    void redo();        //重做
    void cut();         //剪切
    void copy();        //复制
    void paste();       //粘贴
    void textBold();        //加粗
    void textItalic();      //斜体
    void textUnderline();   //下划线
    void textFamily(const QString &f);  //字体样式
    void textSize(const QString &p);    //字体大小
    void textColor();   //文字颜色
    void textStyle(int styleIndex);     //段落前的标号
    void filePrint();               //文件打印
    void filePrintPreview();        //文件打印预览
    void printPreview(QPrinter *);



private:
    //创建菜单栏和工具栏
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    void enabledText();     //在fileNew创建新文件时候调用，使得格式中的动作可用
    my_child *activeMyChild();   //返回活动窗口
    QMdiSubWindow *findMyChild(const QString &fileName);//选择的文件名和之前打开窗口的文件名是否一样。（在openFile函数中使用）
    void colorChanged(const QColor &c);     //选择颜色后，将右上角的颜色QAction颜色换成自己选的颜色












    QSignalMapper *windowMapper;   //用于简化信号与槽的类

    //定义QMdirArea变量
    QMdiArea *mdiArea;

    //菜单栏中的文件菜单
    QMenu *fileMenu;
    //文件菜单中的动作(Action)
    QAction *newAct;    //新建
    QAction *openAct;   //打开
    QAction *saveAct;   //保存
    QAction *saveAsAct;     //另存为
    QAction *printAct;      //打印
    QAction *printPreviewAct;   //打印预览
    QAction *exitAct;       //退出

    //菜单栏中的编辑菜单
    QMenu *editMenu;
    //编辑菜单中的动作(Action)
    QAction *undoAct;   //撤销
    QAction *redoAct;   //重做
    QAction *cutAct;    //剪切
    QAction *copyAct;   //复制
    QAction *pasteAct;  //粘贴

    //菜单栏中的格式菜单
    QMenu *formatMenu;
    QMenu *fontMenu;            //格式菜单的子菜单
    QMenu *alignMenu;           //格式菜单的子菜单
    //格式菜单中的动作(Action)
    QAction *boldAct;       //加粗
    QAction *italicAct;     //斜体
    QAction *underlineAct;   //下划线
    QAction *leftAlignAct;  //左对齐
    QAction *centerAct;     //居中对齐
    QAction *rightAlignAct;     //右对齐
    QAction *justifyAct;    //两端对齐
    QAction *colorAct;      //颜色

    //菜单栏中的窗口菜单
    QMenu *windowMenu;
    //窗口菜单中的动作(QAction)
    QAction *closeAct;      //关闭活动窗口
    QAction *closeAllAct;   //关闭所有窗口
    QAction *tileAct;       //平铺窗口
    QAction *cascadeAct;    //堆叠窗口
    QAction *nextAct;       //下一个窗口
    QAction *previousAct;   //上一个窗口
    QAction *separatorAct;  //分隔符

    //菜单栏中的关于窗口
    QMenu *aboutMenu;
    QAction *aboutAct;
    QAction *aboutQtAct;



    //工具栏
    QToolBar *fileToolBar;      //文件 工具条
    QToolBar *editToolBar;      //编辑 工具条
    QToolBar *formatToolBar;    //格式 工具条
    QToolBar *comboToolBar;     //组合选择框
    QComboBox *comboStyle;      //子控件   标号与编号类型选择框
    QFontComboBox *comboFont;   //子控件   字体选择框
    QComboBox *comboSize;       //子控件   字号选择框


};
#endif // MY_WORD_H
