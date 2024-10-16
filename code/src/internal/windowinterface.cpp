#include "windowinterface.h"

bool WindowInterface::sm_didInitialize = false;
MainWindow *WindowInterface::mainwindow = nullptr;

WindowInterface::WindowInterface() {
    if(!sm_didInitialize){
        std::cout << "Vous devez appeler WindowInterface::initialize()" << std::endl;
        QMessageBox::warning(nullptr,"Erreur","Vous devez appeler "
                                     "WindowInterface::initialize() avant de créer un "
                                     "objet WindowInterface");
        exit(-1);
    }

    if (!QObject::connect(this,
                          SIGNAL(sig_consoleAppendText(unsigned int,QString)),
                          mainwindow,
                          SLOT(consoleAppendText(unsigned int, const QString&)),
                          Qt::QueuedConnection)) {
            std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this,
                          SIGNAL(sig_updateFund(unsigned int, unsigned int)),
                          mainwindow,
                          SLOT(updateFund(unsigned int, unsigned int)),
                          Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this,
                          SIGNAL(sig_updateStock(unsigned int, std::map<ItemType, int>*)),
                          mainwindow,
                          SLOT(updateStock(unsigned int, std::map<ItemType, int>*)),
                          Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }

    if (!QObject::connect(this,
                          SIGNAL(sig_set_link(int, int)),
                          mainwindow,
                          SLOT(set_link(int, int)),
                          Qt::QueuedConnection)) {
        std::cout << "Error with signal-slot connection" << std::endl;
    }
}

void WindowInterface::consoleAppendText(unsigned int consoleId, QString text) {
    emit sig_consoleAppendText(consoleId, text);
}


void WindowInterface::updateFund(unsigned int id, unsigned new_fund) {
    emit sig_updateFund(id, new_fund);
}

void WindowInterface::updateStock(unsigned int id, std::map<ItemType, int>* stocks) {
    emit sig_updateStock(id, stocks);
}

void WindowInterface::setLink(int from, int to){
    emit sig_set_link(from, to);
}

void WindowInterface::initialize(unsigned int nbExtractors, unsigned int nbFactories, unsigned int nbWholesalers) {
    if(sm_didInitialize){
        std::cout << "Vous devez ne devriez appeler WindowInterface::initialize()"
                     << " qu'une seule fois" << std::endl;
                QMessageBox::warning(nullptr,"Erreur","Vous ne devriez appeler "
                                     "WindowInterface::initialize() "
                                     "qu'une seule fois");
                return;
    }

    mainwindow = new MainWindow(nbExtractors, nbFactories, nbWholesalers, nullptr);
    mainwindow->show();
    sm_didInitialize = true;
}

void WindowInterface::simulateWork(){
    PcoThread::usleep((rand() % 100 + 1) * 10000);
}

void WindowInterface::setUtils(Utils* utils)
{
    mainwindow->setUtils(utils);
}
