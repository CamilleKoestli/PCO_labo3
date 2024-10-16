#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "display.h"

#include <QMainWindow>
#include <QTextEdit>
#include <QDockWidget>
#include <QCloseEvent>
#include <QPushButton>
#include <QMessageBox>

class Utils;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(unsigned int nbMines, unsigned int nbFactories, unsigned int nbWholesalers, QWidget * parent = nullptr);
//    ~MainWindow();

    DisplayView * display;
    std::vector<QTextEdit* > m_consoles;
    std::vector<QDockWidget*> m_docks;
//    std::vector<QTextBlock* > m_docks;
    void setUtils(Utils* utils);

protected:
    unsigned int m_nbConsoles;
    void closeEvent(QCloseEvent *event) override;
    Utils *utils;

public slots:
    void consoleAppendText(unsigned int consoleId, const QString& text);
//    void handleButton();

    void updateFund(unsigned int id, unsigned new_fund);
    void updateStock(unsigned int id, std::map<ItemType, int>* stocks);
    void set_link(int from, int to);
private:
//    QPushButton *m_button;
};
#endif // MAINWINDOW_H
