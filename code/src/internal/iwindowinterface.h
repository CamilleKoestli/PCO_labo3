#ifndef IWINDOWINTERFACE_H
#define IWINDOWINTERFACE_H

#include <QString>
#include <map>
#include <pcosynchro/pcothread.h>
#include "seller.h"

class Utils;

class IWindowInterface {
public:
    virtual ~IWindowInterface() = default;

    virtual void consoleAppendText(unsigned int consoleId, QString text) = 0;
    virtual void updateFund(unsigned int id, unsigned new_fund) = 0;
    virtual void updateStock(unsigned int id, std::map<ItemType, int>* stocks) = 0;
    virtual void setLink(int from, int to) = 0;
    virtual void setUtils(Utils* utils) = 0;
    virtual void simulateWork() = 0;
};

#endif // IWINDOWINTERFACE_H
