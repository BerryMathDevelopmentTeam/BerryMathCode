#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <iostream>
#include <QTextStream>
#include <QInputDialog>
#include <QTextBlock>
#include <QThread>
#include <QTimer>
#include "ide.h"
#include "ui_ide.h"

IDE::IDE(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IDE),
    file(nullptr)
{
    ui->setupUi(this);
    QFontMetrics metrics(ui->editor->font());
    ui->editor->setFont(QFont(tr("Monospace"), 14));
    ui->editor->setTabStopWidth(4 * metrics.width(' '));
}

IDE::~IDE()
{
    if (file) delete file;
    delete ui;
}

void IDE::editionMode()
{
    ui->editor->setText(content);
    update();
}
void IDE::update()
{
//    curCursor = ui->editor->textCursor();
    auto cPos = ui->editor->textCursor().position();
//    qDebug() << "t:" << curCursor.position();
//    auto col = curCursor.columnNumber();
//    auto row = curCursor.blockNumber();
    if (!_update) return;
//    if (content.length() > ui->editor->toPlainText().length()) {// 说明删除了一些字符
//        curCursor.setPosition(curCursor.columnNumber());
//        qDebug() << "deleted: " << curCursor.columnNumber() << ", " << curCursor.blockNumber();
//    if (content.length() < ui->editor->toPlainText().length()) {// 说明添加了一些字符
//        col += 1;
//        qDebug() << "added: " << curCursor.columnNumber() << ", " << curCursor.blockNumber();
//    }
    content = ui->editor->toPlainText();
//    qDebug() << content;
//    qDebug() << ui->editor->toHtml();
    QString text;
    lexer.open(content.toStdString());
    auto token = lexer.get();
    auto lastIndex = 0;
    while (true) {
        if (token.t == BM::Lexer::PROGRAM_END || lexer.index() > content.length()) break;
        else if (token.s == "this") text += "<span style='color:#cc7832; font-weight: bold;'>";
        else if (token.t == BM::Lexer::UNKNOWN_TOKEN || token.t >= BM::Lexer::ADD_TOKEN) text += "<span style='color:#ffffff;'>";
        else if (token.t < BM::Lexer::NUMBER_TOKEN) text += "<span style='color:#cc7832; font-weight: bold;'>";
        else if (token.t == BM::Lexer::NUMBER_TOKEN) text += "<span style='color:#6897bb; font-weight: bold;'>";
        else if (token.t == BM::Lexer::STRING_TOKEN) text += "<span style='color:#6a8759; font-weight: bold;'>";
        else if (token.t == BM::Lexer::NOTE_TOKEN) text += "<span style='color:#797572; font-weight: bold;'>";
        for (auto i = lastIndex; i < lexer.index(); i++) {
            if (content[i] == "\n" || content[i] == "\r") text += "<br>";
            else if (content[i] == " ") text += "&nbsp;";
            else if (content[i] == "\t") text += "&nbsp;&nbsp;&nbsp;&nbsp;";
            else text += content[i];
        }
        text += "</span>";
//        std::cerr << lexer.index() << std::endl;
        lastIndex = lexer.index();
        token = lexer.get();
    }
//    qDebug() << curCursor;
    _update = false;
    ui->editor->setText(text);
    _update = true;
//    QTextBlock _row(ui->editor->document()->findBlockByLineNumber(row));
//    curCursor = ui->editor->document()->findBlockByLineNumber(row - 1).position();
//    move(col, row);
    curCursor = ui->editor->textCursor();
//    if (cPos > curCursor.columnNumber()) cPos = curCursor.columnNumber();
    curCursor.setPosition(cPos);
//    curCursor.blockNumber();
    if (cPos != 0 && curCursor.position() == 0) {
//        qDebug() << content;
        text += "<span style='color:#ffffff;'><br><span>";
        _update = false;
        ui->editor->setText(text);
        _update = true;
//        qDebug() << text;
//        qDebug() << "==============";
        ui->editor->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }
    else ui->editor->setTextCursor(curCursor);
//    qDebug() << text.length() << cPos << curCursor.columnNumber();
//    ui->editor->setTextCursor(curCursor);
//    qDebug() << "changed:" << curCursor.position();
}

void IDE::on_actionOpen_File_triggered()
{
    if (!openFile()) {
        QMessageBox::warning(NULL, "Open File Error", "Cannot open the file you selected, it may has deleted or is unauthorized", QMessageBox::Yes, QMessageBox::Yes);
        return;
    }
    QTextStream outText(file->readAll());
    content = outText.readAll();
    lexer.open(content.toStdString());
//    qDebug() << content;
    editionMode();
}

void IDE::on_editor_textChanged()
{
    if (!_update) return;
//    std::cerr << "c" << std::endl;
    update();
}

void IDE::on_editor_cursorPositionChanged()
{
    curCursor = ui->editor->textCursor();
}


void IDE::on_actionSave_2_triggered()
{
    if (file) file->write(content.toStdString().c_str());
    else {
        fileName = QFileDialog::getExistingDirectory(this, tr("Choose a Path"));
        bool isOK;
        while (true) {
            QString filename = QInputDialog::getText(NULL, "Input Your Filename",
                                                               "Please your filename",
                                                               QLineEdit::Normal,
                                                               "untitled.bm",
                                                               &isOK);
            if (isOK) {
                fileName += filename;
                break;
            } else QMessageBox::warning(NULL, "Save File Error", "Your filename is null", QMessageBox::Yes, QMessageBox::Yes);
        }
        file = new QFile(fileName);
        if (file->open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate)) file->write(content.toStdString().c_str());
        else QMessageBox::warning(NULL, "Open File Error", "Cannot open the file you selected, it may is unauthorized", QMessageBox::Yes, QMessageBox::Yes);
    }
}

bool IDE::openFile()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"));
//    qDebug() << fileName;
    if (file) delete file;
    file = new QFile(fileName);
    return file->open(QIODevice::Text | QIODevice::ReadWrite);
}
