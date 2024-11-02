#include "supplier.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>

IWindowInterface* Supplier::interface = nullptr;

Supplier::Supplier(int uniqueId, int fund, std::vector<ItemType> resourcesSupplied)
    : Seller(fund, uniqueId), resourcesSupplied(resourcesSupplied), nbSupplied(0) 
{
    for (const auto& item : resourcesSupplied) {    
        stocks[item] = 0;    
    }

    interface->consoleAppendText(uniqueId, QString("Supplier Created"));
    interface->updateFund(uniqueId, fund);
}


int Supplier::request(ItemType it, int qty) {
    // TODO
    if (std::find(resourcesSupplied.begin(), resourcesSupplied.end(), it) == resourcesSupplied.end()) {
        return 0; 
    }

    int price = qty * getCostPerUnit(it); 

    mutex.lock();
    if (price <= money) { 
        stocks[it] += qty; 
        money -= price;    
        mutex.unlock();
        interface->consoleAppendText(uniqueId, QString("Bought %1 of %2 for %3.")
                                    .arg(qty).arg(getItemName(it)).arg(price));
        return price;    
    }
    mutex.unlock();
    interface->consoleAppendText(uniqueId, QString("Not enough funds to buy %1 of %2.")
                                .arg(qty).arg(getItemName(it)));
    mutex.unlock();
}

void Supplier::run() {
    interface->consoleAppendText(uniqueId, "[START] Supplier routine");
    while (true /*TODO*/) {
        ItemType resourceSupplied = getRandomItemFromStock();
        int supplierCost = getEmployeeSalary(getEmployeeThatProduces(resourceSupplied));
        // TODO 

        /* Temps aléatoire borné qui simule l'attente du travail fini*/
        interface->simulateWork();
        //TODO

        nbSupplied++;

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Supplier routine");
}


std::map<ItemType, int> Supplier::getItemsForSale() {
    return stocks;
}

int Supplier::getMaterialCost() {
    int totalCost = 0;
    for (const auto& item : resourcesSupplied) {
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

std::vector<ItemType> Supplier::getResourcesSupplied() const
{
    return resourcesSupplied;
}

int Supplier::send(ItemType it, int qty, int bill){
    return 0;
}
