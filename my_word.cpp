#include "my_word.h"
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <Qtwidgets>

my_word::my_word(QWidget *parent)
    : QMainWindow(parent)
{
    //设置大小和标题
    resize(800,500);
    setWindowTitle("小李的记事本1.0");

    //创建一个QMdirArea实例
    mdiArea = new QMdiArea;
    //给mdiArea控件设置水平和竖直滚轮策略
    mdiArea -> setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea -> setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //将其设置为MainWindow中心组件
    setCentralWidget(mdiArea);

    //创建动作类和菜单类
    createActions();
    createMenus();

    //创建工具栏
    createToolBars();

    //创建状态栏
    createStatusBar();

    //更新菜单（通知主窗口：当前活动子窗口更改）
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(updateMenus()));
    updateMenus();

    //用于简化信号与槽函数的类
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mappedObject(QWidget*)),this, SLOT(setActiveSubWindow(QWidget*)));
}

my_word::~my_word() {}

//重写关闭事件
void my_word::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();

    if (mdiArea->currentSubWindow()) {  //如果没有活动窗口，则窗口关闭
        event->ignore();
    } else {
        event->accept();
    }

}


//创建所用到的动作类   (&N 提示这个菜单项可以使用快捷键 Ctrl+N 来访问)
void my_word::createActions()
{
    //创建文件菜单中的动作
    newAct = new QAction(QIcon(":/res/newAct.png"), tr("新建(&N)"), this);
    newAct->setShortcuts(QKeySequence::New);        //设置了快捷键
    newAct->setToolTip("新建");                   //设置工具栏按钮的提示文本
    newAct->setStatusTip(tr("创建一个新文档"));     //设置状态栏提示文本
    connect(newAct, SIGNAL(triggered()), this, SLOT(fileNew()));

    openAct = new QAction(QIcon(":/res/openAct.png"), tr("打开(&O)"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setToolTip("打开");
    openAct->setStatusTip(tr("打开一个已存在的文档"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(fileOpen()));

    saveAct = new QAction(QIcon(":/res/saveAct.png"), tr("保存(&S)"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setToolTip("保存");
    saveAct->setStatusTip(tr("保存当前文档"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(fileSave()));

    saveAsAct = new QAction(QIcon(":/res/saveAsAct.png"),tr("另存为(&A)"), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("以一个新名字保存文档"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

    printAct = new QAction(QIcon(":/res/printAct.png"), tr("打印(&P)"), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setToolTip("打印");
    printAct->setStatusTip(tr("打印文档"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(filePrint()));

    printPreviewAct = new QAction(QIcon(":/res/printPreviewAct.png"),tr("打印预览"), this);
    printPreviewAct->setStatusTip(tr("预览打印效果"));
    connect(printPreviewAct, SIGNAL(triggered()), this, SLOT(filePrintPreview()));

    exitAct = new QAction(QIcon(":/res/exitAct.png"),tr("退出(&X)"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("退出应用程序"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));




    //创建编辑菜单中的动作(其中的槽函数不需要子窗口去实现)
    undoAct = new QAction(QIcon(":/res/undoAct.png"),tr("撤销(&U)"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setToolTip("撤销");
    undoAct->setStatusTip(tr("撤销当前操作"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));

    redoAct = new QAction(QIcon(":/res/redoAct.png"),tr("重做(&R)"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setToolTip("重做");
    redoAct->setStatusTip(tr("恢复之前操作"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));

    cutAct = new QAction(QIcon(":/res/cutAct.png"),tr("剪切(&T)"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setToolTip("剪切");
    cutAct->setStatusTip(tr("从文档中裁剪所选内容，并将其放入剪贴板"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

    copyAct = new QAction(QIcon(":/res/copyAct.png"),tr("复制(&C)"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setToolTip("复制");
    copyAct->setStatusTip(tr("拷贝所选内容，并将其放入剪贴板"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/res/pasteAct.png"),tr("粘贴(&P)"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setToolTip("粘贴");
    pasteAct->setStatusTip(tr("将剪贴板的内容粘贴到文档"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));




    //创建格式菜单中的动作
    //格式菜单中的字体菜单动作
    boldAct = new QAction(QIcon(":/res/boldAct.png"),tr("加粗(&B)"), this);
    boldAct->setCheckable(true);
    boldAct->setShortcut(Qt::CTRL | Qt::Key_B);
    boldAct->setToolTip("加粗");
    boldAct->setStatusTip(tr("将所选中的文字加粗"));
    QFont bold;
    bold.setBold(true);
    boldAct->setFont(bold);
    connect(boldAct, SIGNAL(triggered()), this, SLOT(textBold()));

    italicAct = new QAction(QIcon(":/res/italicAct.png"),tr("倾斜(&I)"), this);
    italicAct->setCheckable(true);
    italicAct->setShortcut(Qt::CTRL | Qt::Key_I);
    italicAct->setToolTip("倾斜");
    italicAct->setStatusTip(tr("将所选文字用斜体显示"));
    QFont italic;
    italic.setItalic(true);
    italicAct->setFont(italic);
    connect(italicAct, SIGNAL(triggered()), this, SLOT(textItalic()));

    underlineAct = new QAction(QIcon(":/res/underlineAct.png"),tr("下划线(&U)"), this);
    underlineAct->setCheckable(true);
    underlineAct->setShortcut(Qt::CTRL | Qt::Key_U);
    underlineAct->setToolTip("下划线");
    underlineAct->setStatusTip(tr("给所选文字加下划线"));
    QFont underline;
    underline.setUnderline(true);
    underlineAct->setFont(underline);
    connect(underlineAct, SIGNAL(triggered()), this, SLOT(textUnderline()));


    //格式菜单中的段落菜单动作
    //格式菜单中的段落菜单下只能选中其中一项，只有一种对齐方式，所以想到用动作组类(默认是互斥的，同一时刻只能一个动作被选中)。
    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    if (QApplication::isLeftToRight())  //用于判断应用程序的布局方向是否为从左到右
    {
        leftAlignAct = new QAction(QIcon(":/res/leftAct.png"),tr("左对齐(&L)"), grp);
        centerAct = new QAction(QIcon(":/res/centerAct.png"),tr("居中(&E)"), grp);
        rightAlignAct = new QAction(QIcon(":/res/rightAct.png"),tr("右对齐(&R)"), grp);
    } else {
        rightAlignAct = new QAction(QIcon(":/res/rightAct.png"),tr("右对齐(&R)"), grp);
        centerAct = new QAction(QIcon(":/res/centerAct.png"),tr("居中(&E)"), grp);
        leftAlignAct = new QAction(QIcon(":/res/leftAct.png"),tr("左对齐(&L)"), grp);
    }
    justifyAct = new QAction(QIcon(":/res/justifyAct.png"),tr("两端对齐(&J)"), grp);

    leftAlignAct->setShortcut(Qt::CTRL | Qt::Key_L);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setToolTip("左对齐");
    leftAlignAct->setStatusTip(tr("将文字左对齐"));

    centerAct->setShortcut(Qt::CTRL | Qt::Key_E);
    centerAct->setCheckable(true);
    centerAct->setToolTip("居中");
    centerAct->setStatusTip(tr("将文字居中对齐"));

    rightAlignAct->setShortcut(Qt::CTRL | Qt::Key_R);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setToolTip("右对齐");
    rightAlignAct->setStatusTip(tr("将文字右对齐"));

    justifyAct->setShortcut(Qt::CTRL | Qt::Key_J);
    justifyAct->setCheckable(true);
    justifyAct->setToolTip("两端对齐");
    justifyAct->setStatusTip(tr("将文字左右两端同时对齐，并根据需要增加字间距"));

    QPixmap pix(16, 16);
    pix.fill(Qt::red);
    colorAct = new QAction(pix, tr("颜色"), this);
    colorAct->setToolTip("颜色");
    colorAct->setStatusTip(tr("设置文字颜色"));
    connect(colorAct, SIGNAL(triggered()), this, SLOT(textColor()));




    //创建窗口菜单中的动作
    closeAct = new QAction(QIcon(":/res/closeAct.png"),tr("关闭(&O)"), this);
    closeAct->setStatusTip(tr("关闭活动文档子窗口"));
    connect(closeAct, SIGNAL(triggered()),mdiArea, SLOT(closeActiveSubWindow()));           //不用自己实现

    closeAllAct = new QAction(QIcon(":/res/closeAllAct.png"),tr("关闭所有(&A)"), this);
    closeAllAct->setStatusTip(tr("关闭所有子窗口"));
    connect(closeAllAct, SIGNAL(triggered()),mdiArea, SLOT(closeAllSubWindows()));          //不用自己实现

    tileAct = new QAction(QIcon(":/res/tileAct.png"),tr("平铺(&T)"), this);
    tileAct->setStatusTip(tr("平铺子窗口"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));                 //不用自己实现

    cascadeAct = new QAction(QIcon(":/res/cascadeAct.png"),tr("层叠(&C)"), this);
    cascadeAct->setStatusTip(tr("层叠子窗口"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));           //不用自己实现

    nextAct = new QAction(QIcon(":/res/nextAct.png"),tr("下一个(&X)"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("移动焦点到下一个子窗口"));
    connect(nextAct, SIGNAL(triggered()),mdiArea, SLOT(activateNextSubWindow()));           //不用自己实现

    previousAct = new QAction(QIcon(":/res/previousAct.png"),tr("前一个(&V)"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("移动焦点到前一个子窗口"));
    connect(previousAct, SIGNAL(triggered()),mdiArea, SLOT(activatePreviousSubWindow()));   //不用自己实现

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);




    //创建菜单栏中的关于动作
    aboutAct = new QAction(QIcon(":/res/aboutAct.png"),tr("关于(&A)"), this);
    aboutAct->setStatusTip(tr("关于 Myself Word"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(QIcon(":/res/aboutqtAct.png"),tr("关于 Qt(&Q)"), this);
    aboutQtAct->setStatusTip(tr("关于 Qt 库"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

//创建所用到的菜单类
void my_word::createMenus()
{
    //菜单栏中的文件菜单
    fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
    fileMenu->addAction(printPreviewAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);


    //菜单栏中的编辑菜单
    editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);


    //菜单栏中的格式菜单
    formatMenu = menuBar()->addMenu(tr("格式(&F)"));
    fontMenu = formatMenu->addMenu(QIcon(":/res/typefaceMenu.png"),tr("字体"));   //字体子菜单
    fontMenu->addAction(boldAct);
    fontMenu->addAction(italicAct);
    fontMenu->addAction(underlineAct);
    alignMenu = formatMenu->addMenu(QIcon(":/res/paragraphMenu.png"),tr("段落"));      //段落子菜单
    alignMenu->addAction(leftAlignAct);
    alignMenu->addAction(centerAct);
    alignMenu->addAction(rightAlignAct);
    alignMenu->addAction(justifyAct);




    //菜单栏中的窗口菜单
    windowMenu = menuBar()->addMenu(tr("窗口(&W)"));
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(nextAct);
    windowMenu->addAction(separatorAct);
    updateWindowMenu();

    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    menuBar()->addSeparator();




    //菜单栏中的关于菜单
    aboutMenu = menuBar()->addMenu(tr("关于(&A)"));
    aboutMenu->addAction(aboutAct);
    aboutMenu->addAction(aboutQtAct);

}

void my_word::createToolBars()
{
    //"文件"工具栏
    fileToolBar = addToolBar(tr("文件"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();        //分隔条
    fileToolBar->addAction(printAct);

    //"编辑"工具栏
    editToolBar = addToolBar(tr("编辑"));
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addSeparator();        //分隔条
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    //"格式"工具栏
    formatToolBar = addToolBar(tr("格式"));
    formatToolBar->addAction(boldAct);
    formatToolBar->addAction(italicAct);
    formatToolBar->addAction(underlineAct);
    formatToolBar->addSeparator();      //分隔条
    formatToolBar->addAction(leftAlignAct);
    formatToolBar->addAction(centerAct);
    formatToolBar->addAction(rightAlignAct);
    formatToolBar->addAction(justifyAct);
    formatToolBar->addSeparator();      //分隔条
    formatToolBar->addAction(colorAct);

    //组合工具栏
    addToolBarBreak(Qt::TopToolBarArea);    //使这个工具条在界面上另起一行显示
    comboToolBar = addToolBar(tr("组合选择"));
    comboStyle = new QComboBox();
    comboToolBar->addWidget(comboStyle);
    comboStyle->addItem("标准");
    comboStyle->addItem("项目符号 (●)");
    comboStyle->addItem("项目符号 (○)");
    comboStyle->addItem("项目符号 (■)");
    comboStyle->addItem("编号 (⒈⒉⒊)");
    comboStyle->addItem("编号 ( a.b.c.)");
    comboStyle->addItem("编号 ( A.B.C.)");
    comboStyle->addItem("编号 (ⅰ.ⅱ.ⅲ.)");
    comboStyle->addItem("编号 (Ⅰ.Ⅱ.Ⅲ.)");
    comboStyle->setStatusTip("段落加标号或编号");
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox();
    comboToolBar->addWidget(comboFont);
    comboFont->setStatusTip("更改字体");
    connect(comboFont, SIGNAL(textActivated(QString)), this, SLOT(textFamily(QString)));

    comboSize = new QComboBox();
    comboToolBar->addWidget(comboSize);
    comboSize->setEditable(true);
    comboSize->setStatusTip("更改字号");

    foreach(int size, QFontDatabase::standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(textActivated(QString)), this, SLOT(textSize(QString)));
    //设置当前的字体大小
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));
}

void my_word::createStatusBar()
{
    statusBar()->showMessage(tr("就绪"));
}


//设置文本对齐的槽函数
void my_word::textAlign(QAction *a)
{
    if(activeMyChild())
    {
        if (a == leftAlignAct)
            activeMyChild()->setAlign(1);
        else if (a == centerAct)
            activeMyChild()->setAlign(2);
        else if (a == rightAlignAct)
            activeMyChild()->setAlign(3);
        else if (a == justifyAct)
            activeMyChild()->setAlign(4);
    }

}

//创建新文件的槽函数
void my_word::fileNew()
{
    my_child *child = createMyChild();
    child->newFile();
    child->show();
    enabledText();//调用这个函数使得格式中的动作可用
}

void my_word::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开"),"请选择你需要打开的文件", tr("所有文件 (*.*);;HTML 文档 (*.htm *.html)"));
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMyChild(fileName);
        //如果选择的文件名和之前打开窗口的文件名一样。（返回的是打开的子窗口，如果没打开过，返回的就是空，就需要重新创建）
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }
        my_child *child = createMyChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("文件已载入"), 2000);
            child->show();
            enabledText();      //使得字体设置菜单可用
        } else {
            child->close();
        }
    }

}

void my_word::fileSave()
{
    if (activeMyChild() && activeMyChild()->save())
        statusBar()->showMessage(tr("保存成功"), 2000);
}

void my_word::fileSaveAs()
{
    if (activeMyChild() && activeMyChild()->saveAs())
        statusBar()->showMessage(tr("保存成功"), 2000);
}

void my_word::undo()
{
    if (activeMyChild())
        activeMyChild()->undo();
}

void my_word::redo()
{
    if (activeMyChild())
        activeMyChild()->redo();
}

void my_word::cut()
{
    if (activeMyChild())
        activeMyChild()->cut();
}

void my_word::copy()
{
    if (activeMyChild())
        activeMyChild()->copy();
}

void my_word::paste()
{
    if (activeMyChild())
        activeMyChild()->paste();
}

void my_word::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldAct->isChecked() ? QFont::Bold : QFont::Normal);
    if(activeMyChild())     //如果有活动窗口
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);

}

void my_word::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicAct->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void my_word::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineAct->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void my_word::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    QFont font(f);
    fmt.setFont(font);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void my_word::textSize(const QString &p)
{
    QTextCharFormat fmt;
    qreal size = p.toInt();
    fmt.setFontPointSize(size);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void my_word::textColor()
{
    if(activeMyChild())
    {
        QColor col = QColorDialog::getColor(activeMyChild()->textColor(), this);
        if (!col.isValid())
            return;
        QTextCharFormat fmt;
        fmt.setForeground(col);
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
        colorChanged(col);
    }

}

void my_word::textStyle(int styleIndex)
{
    if(activeMyChild())
    {
        activeMyChild()->setStyle(styleIndex);
    }

}

void my_word::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    //创建打印对话框
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (activeMyChild()->textCursor().hasSelection())
        dlg->setOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("打印文档"));
    //如果对话框接受，则打印。
    if (dlg->exec() == QDialog::Accepted)
        activeMyChild()->print(&printer);
    //没有继承关系，需要手动释放内存。
    delete dlg;
}

void my_word::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);

    QPrintPreviewDialog preview(&printer, this);
    //当按打印时候，自动发出信号打印
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();

}

void my_word::printPreview(QPrinter *printer)
{
    activeMyChild()->print(printer);
}

//创建类对象并返回的槽函数
my_child *my_word::createMyChild()
{
    my_child *child = new my_child();

    mdiArea->addSubWindow(child);
    //当选择文本或者取消的时候，上方对应的按钮设置是否可用。
    connect(child, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));

    return child;
}

//更新菜单槽函数
void my_word::updateMenus()
{
    //至少有一个子文档打开着的情况
    bool hasMyChild = (activeMyChild()!=0);

    saveAct->setEnabled(hasMyChild);
    saveAsAct->setEnabled(hasMyChild);
    printAct->setEnabled(hasMyChild);
    printPreviewAct->setEnabled(hasMyChild);
    pasteAct->setEnabled(hasMyChild);
    closeAct->setEnabled(hasMyChild);
    closeAllAct->setEnabled(hasMyChild);
    tileAct->setEnabled(hasMyChild);
    cascadeAct->setEnabled(hasMyChild);
    nextAct->setEnabled(hasMyChild);
    previousAct->setEnabled(hasMyChild);
    separatorAct->setVisible(hasMyChild);
    //文档打开着并且其中有内容被选中的情况
    bool hasSelection = (activeMyChild() && activeMyChild()->textCursor().hasSelection());

    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);

    boldAct->setEnabled(hasSelection);
    italicAct->setEnabled(hasSelection);
    underlineAct->setEnabled(hasSelection);
    leftAlignAct->setEnabled(hasSelection);
    centerAct->setEnabled(hasSelection);
    rightAlignAct->setEnabled(hasSelection);
    justifyAct->setEnabled(hasSelection);
    colorAct->setEnabled(hasSelection);

}

void my_word::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());
    //显示当前打开着的文档子窗口项
    for (int i = 0; i < windows.size(); ++i) {
        my_child *child = qobject_cast<my_child *>(windows.at(i)->widget());
        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());//可以有快捷键
        } else {
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMyChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
    enabledText();          //使得字体设置菜单可用
}

//在fileNew创建新文件时候调用，使得格式中的动作可用
void my_word::enabledText()
{
    boldAct->setEnabled(true);
    italicAct->setEnabled(true);
    underlineAct->setEnabled(true);
    leftAlignAct->setEnabled(true);
    centerAct->setEnabled(true);
    rightAlignAct->setEnabled(true);
    justifyAct->setEnabled(true);
    colorAct->setEnabled(true);
}

//返回活动窗口
my_child *my_word::activeMyChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<my_child *>(activeSubWindow->widget());
    return 0;

}

QMdiSubWindow *my_word::findMyChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        my_child *myChild = qobject_cast<my_child *>(window->widget());
        if (myChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;

}

void my_word::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    colorAct->setIcon(pix);
}
