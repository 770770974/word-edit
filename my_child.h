#ifndef MY_CHILD_H
#define MY_CHILD_H

#include <QObject>
#include <QTextEdit>
#include <QWidget>

class my_child : public QTextEdit
{
 Q_OBJECT
public:

    my_child();
    //新建my_child操作
    void newFile();
    //提取文件名
    QString userFriendlyCurrentFile();
    //返回当前文件路径
    QString currentFile() {return curFile;}
    //根据文件名加载文件
    bool loadFile(const QString &fileName);
    //保存文件（当文件曾经被保存过，就直接saveFile,反之，则选择新文件保存）
    bool save();
    //将图片另存为选择的新文件
    bool saveAs();
    //保存文件
    bool saveFile(QString fileName);
    //格式字体设置
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    //文本对齐方式
    void setAlign(int align);
    //段落前的标号
    void setStyle(int style);

protected:
    //重新定义关闭事件（关闭的时候问：文件修改，是否保存文件）
    void closeEvent(QCloseEvent *event);

private slots:
    //如果文档被更改了，更改窗口状态标识
    void documentWasModified();

private:
    //去除路径后的文件名(直接被上面的userFriendlyCurrentFile函数返回)
    QString strippName(const QString &fullFileName);

    QString curFile;  //保存当前文件路径
    bool isUntitled;  //当前文件是否被保存过（是否有名字）
    void setCurrentFile(const QString &fileName);   //设置当前文件

    //设置窗口编号，因为编号会一直保存，所以需要使用静态变量（类外进行初始化）
    static int sequenceNumber;

    bool maybeSave();   //当关闭已经被修改的文件时，进行提示是否保存

};

#endif // MY_CHILD_H
