#include "my_child.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QTextList>

my_child::my_child() {

    setAttribute(Qt::WA_DeleteOnClose); //设置在子窗口关闭时销毁这个类的对象
    isUntitled = true;  //初始 isUntitled 为 true

}


int my_child::sequenceNumber = 1;

//创建新my_child
void my_child::newFile()
{

    //新建的文档默认为命名
    isUntitled = true;
    //将当前文件命名为"文档+编号"的形式，编号先使用再加1
    curFile = tr("文档 %1").arg(sequenceNumber++);
    //设置窗口标题，使用[*]可以在文档被更改后在文件名称后显示"*"号
    setWindowTitle(curFile + "[*]" + tr(" - chile word"));
    //文档更改时发送 contentsChanged()信号，执行 documentWasModified() 曹函数
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(documentWasModified()));
}

//如果文档被更改了，更改窗口状态标识
void my_child::documentWasModified()
{
    //根据文档的 isModified() 函数的返回值，判断编辑器内容是否被更改
    setWindowModified(document()->isModified());
}


//返回文件名
QString my_child::userFriendlyCurrentFile()
{
    return strippName(curFile);
}

//根据文件名加载文件
bool my_child::loadFile(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.exists()) {
        qWarning() << "File does not exist:" << fileName;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << fileName;
        return false;
    }

    // 假设文件是以 UTF-8 编码的，这是 HTML 文件的常见情况
    QTextStream in(&file);

    QString content = in.readAll();

    // 检查内容是否为富文本（这里简单假设 HTML 就是富文本）
    if (content.contains("<html>") || content.contains("<HTML>")) { // 简单的检查，可能不够准确
        this->setHtml(content);
    } else {
        this->setPlainText(content);
    }

    setCurrentFile(fileName);

    // 连接信号和槽以监控文档更改（这部分看起来没问题）
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));

    return true;


}


bool my_child::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool my_child::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"),curFile,tr("HTML 文档 (*.htm *.html);;所有文件 (*.*)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);

}

bool my_child::saveFile(QString fileName)
{
    if (!(fileName.endsWith(".htm", Qt::CaseInsensitive) || fileName.endsWith(".html", Qt::CaseInsensitive))) {
        fileName += ".html"; // 默认保存为 HTML 文档
    }
    //将QTextDocument对象中的文本内容，包括富文本格式（如字体、颜色、对齐方式等格式信息）保存到指定的文件中
    QTextDocumentWriter writer(fileName);
    bool success = writer.write(this->document());
    if (success)
        setCurrentFile(fileName);
    return success;

}

void my_child::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    //mergeCharFormat方法会将传入的format与当前选中文本的格式合并。
    //如果选中的文本已经有某些格式属性，这些属性会被保留，而format中的属性会覆盖原有的属性。
    cursor.mergeCharFormat(format);
    //这个方法通常用于设置光标所在位置的新格式，而不会影响已经选中的文本。
    this->mergeCurrentCharFormat(format);

}

void my_child::setAlign(int align)
{
    switch (align) {
    case 1:
        this->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
        break;
    case 2:
        this->setAlignment(Qt::AlignCenter);
        break;
    case 3:
        this->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
        break;
    case 4:
        this->setAlignment(Qt::AlignJustify);
        break;
    }
}

void my_child::setStyle(int style)
{
    QTextCursor cursor = this->textCursor();

    if (style != 0) {
        QTextListFormat::Style stylename = QTextListFormat::ListDisc;

        switch (style) {
        default:
        case 1:
            stylename = QTextListFormat::ListDisc;
            break;
        case 2:
            stylename = QTextListFormat::ListCircle;
            break;
        case 3:
            stylename = QTextListFormat::ListSquare;
            break;
        case 4:
            stylename = QTextListFormat::ListDecimal;
            break;
        case 5:
            stylename = QTextListFormat::ListLowerAlpha;
            break;
        case 6:
            stylename = QTextListFormat::ListUpperAlpha;
            break;
        case 7:
            stylename = QTextListFormat::ListLowerRoman;
            break;
        case 8:
            stylename = QTextListFormat::ListUpperRoman;
            break;
        }

        //开始一个编辑块，以便在结束时可以一次性应用所有更改。这有助于优化性能并确保一致性。
        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        // 检查光标是否在一个已有的列表中
        //如果光标位于一个列表中，则获取该列表的格式；否则，创建一个新的列表格式并设置缩进。
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        //将选定的样式应用到列表格式，然后使用该格式创建一个新的列表。
        listFmt.setStyle(stylename);
        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);

    }
}
    //根据完整文件名返回文件名
    QString my_child::strippName(const QString &fullFileName)
    {
        return QFileInfo(fullFileName).fileName();
    }

    //设置当前文件名
    void my_child::setCurrentFile(const QString &fileName)
    {
        curFile = QFileInfo(fileName).canonicalFilePath();
        isUntitled = false;
        document()->setModified(false);
        setWindowModified(false);
        setWindowTitle(userFriendlyCurrentFile() + "[*]");

    }

    bool my_child::maybeSave()
    {
        if (!document()->isModified())
            return true;
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("提醒"),tr("文档'%1'已被修改,保存吗?").arg(userFriendlyCurrentFile()),QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
        return true;

    }

    void my_child::closeEvent(QCloseEvent *event)
    {
        if(maybeSave()){
            event->accept();
        }else{
            event->ignore();
        }
    }


