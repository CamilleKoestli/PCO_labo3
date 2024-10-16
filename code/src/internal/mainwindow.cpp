
#include <iostream>

#include "mainwindow.h"

#include "utils.h"

#define CONSOLE_MINIMUM_WIDTH 200

MainWindow::MainWindow(unsigned int nbMines, unsigned int nbFactories, unsigned int nbWholesalers, QWidget * parent) :
    QMainWindow(parent)
{
    m_nbConsoles = nbMines + nbFactories + nbWholesalers;
    m_consoles = std::vector<QTextEdit*>(m_nbConsoles);
//    m_button = new QPushButton("Quit simulation", this);
////    m_button->setGeometry(QRect(QPoint(500, 500), QSize(200, 50)));
//    m_button->show();

//    connect(m_button, &QPushButton::released, this, &MainWindow::handleButton);

    for (unsigned int i = 0; i < m_nbConsoles; ++i) {
        m_consoles[i] = new QTextEdit(this);
        m_consoles[i]->setMinimumWidth(CONSOLE_MINIMUM_WIDTH);
    }

    m_docks = std::vector<QDockWidget*>(m_nbConsoles);

    for (unsigned int i = 0; i < m_nbConsoles; ++i){
        m_docks[i] = new QDockWidget(this);
        m_docks[i]->setWidget(m_consoles[i]);
    }

    for(unsigned int i = 0; i < m_nbConsoles/2; i++) {
        this->addDockWidget(Qt::LeftDockWidgetArea, m_docks[i]);
    }

    for(unsigned int i = m_nbConsoles/2; i < m_nbConsoles; i++) {
        this->addDockWidget(Qt::RightDockWidgetArea, m_docks[i]);
    }
    
    display = new DisplayView(nbMines, nbFactories, nbWholesalers, this);
    setCentralWidget(display);
}

//void MainWindow::handleButton(){
//    m_button->setText("Exemple");
//    m_button->resize(100,100);
//}


void MainWindow::setUtils(Utils* utils)
{
    this->utils = utils;
}

void MainWindow::consoleAppendText(unsigned int consoleId, const QString &text){
    if(consoleId >= m_nbConsoles){
        return;
    }

    m_consoles[consoleId]->append(text);
}

void MainWindow::updateStock(unsigned int id, std::map<ItemType, int>* stocks){
    display->update_stocks(id, stocks);
}

void MainWindow::updateFund(unsigned int id, unsigned new_fund){
    display->update_fund(id, QString::number(new_fund));
}

void MainWindow::set_link(int from, int to){
    display->set_link(from, to);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    std::cout << "close !" << std::endl;
    utils->externalEndService();
    QMessageBox::information(this, "Final report", utils->getFinalReport());
    event->accept();
}
