#include "supplier.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>

IWindowInterface *Supplier::interface = nullptr;

Supplier::Supplier(int uniqueId, int fund, std::vector<ItemType> resourcesSupplied)
    : Seller(fund, uniqueId), resourcesSupplied(resourcesSupplied), nbSupplied(0) {
    for (const auto &item: resourcesSupplied) {
        stocks[item] = 0;
    }

    interface->consoleAppendText(uniqueId, QString("Supplier Created"));
    interface->updateFund(uniqueId, fund);
}


int Supplier::request(ItemType it, int qty) {
    // TODO
 if(stocks[it] >= qty){
        mutex.lock();
        stocks[it] -= qty;
        money += getCostPerUnit(it)*qty;
        mutex.unlock();
        return getCostPerUnit(it)*qty;
    }
    return 0;
}

void Supplier::run() {
    interface->consoleAppendText(uniqueId, "[START] Supplier routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        ItemType resourceSupplied = getRandomItemFromStock();
        int supplierCost = getEmployeeSalary(getEmployeeThatProduces(resourceSupplied));
        // TODO

        if (money >= supplierCost) {
            money -= supplierCost;
            mutex.lock();
            stocks[resourceSupplied]++;
            nbSupplied++;
            mutex.unlock();

            // Simule un délai d'attente
            interface->simulateWork();

            
            interface->consoleAppendText(uniqueId, QString("Imported %1 of %2.")
                                         .arg(1).arg(getItemName(resourceSupplied)));
        } else {
            interface->consoleAppendText(uniqueId, "Insufficient funds to pay the employee.");
        }
        

        /* Temps aléatoire borné qui simule l'attente du travail fini*/
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //TODO
    }
    interface->consoleAppendText(uniqueId, "[STOP] Supplier routine");
}


std::map<ItemType, int> Supplier::getItemsForSale() {
    return stocks;
}

int Supplier::getMaterialCost() {
    int totalCost = 0;
    for (const auto &item: resourcesSupplied) {
        totalCost += getCostPerUnit(item);
    }
    return totalCost;
}

int Supplier::getAmountPaidToWorkers() {
    return nbSupplied * getEmployeeSalary(EmployeeType::Supplier);
}

void Supplier::setInterface(IWindowInterface *windowInterface) {
    interface = windowInterface;
}

std::vector<ItemType> Supplier::getResourcesSupplied() const {
    return resourcesSupplied;
}

int Supplier::send(ItemType it, int qty, int bill) {
    return 0;
}
