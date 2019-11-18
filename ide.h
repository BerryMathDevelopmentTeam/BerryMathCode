#ifndef IDE_H
#define IDE_H

#include <QMainWindow>
#include <QFile>
#include <QThread>
#include <QEventLoop>
#include <QTextCursor>
#include <BerryMath.h>

namespace Ui {
class IDE;
}
class IDE : public QMainWindow
{
    Q_OBJECT

public:
    explicit IDE(QWidget *parent = 0);
    ~IDE();

public slots:
    void slotUpdate() { update(); }

private slots:
    void on_actionOpen_File_triggered();

    void on_editor_textChanged();

    void on_editor_cursorPositionChanged();

    void on_actionSave_2_triggered();

private:
    void editionMode();
    void update();
    bool openFile();
    Ui::IDE *ui;
    QString fileName;
    QString content;
    QFile* file;
    BM::Lexer lexer;
    bool _update = true;
    QTextCursor curCursor;
};

#endif // IDE_H
