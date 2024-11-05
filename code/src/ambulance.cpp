#include "ambulance.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>

IWindowInterface *Ambulance::interface = nullptr;

Ambulance::Ambulance(int uniqueId, int fund, std::vector<ItemType> resourcesSupplied,
                     std::map<ItemType, int> initialStocks)
    : Seller(fund, uniqueId), resourcesSupplied(resourcesSupplied), nbTransfer(0) {
    interface->consoleAppendText(uniqueId, QString("Ambulance Created"));

    for (const auto &item: resourcesSupplied) {
        if (initialStocks.find(item) != initialStocks.end()) {
            stocks[item] = initialStocks[item];
        } else {
            stocks[item] = 0;
        }
    }

    interface->updateFund(uniqueId, fund);
}

void Ambulance::sendPatient() {
    
    auto hospital = chooseRandomSeller(hospitals);
    int patient = 1;
    int bill = getCostPerUnit(ItemType::PatientSick);
    int salarySupplier = getEmployeeSalary(EmployeeType::Supplier);
    
    mutex.lock();
    if (stocks[ItemType::PatientSick] >= patient) {
        if (hospital->send(ItemType::PatientSick, patient, bill)) {
            money += bill;

            stocks[ItemType::PatientSick] -= patient;
            ++nbTransfer;

            money -= salarySupplier;
            mutex.unlock();

            interface->consoleAppendText(uniqueId, "Successfully transferred a patient to the hospital.");
        }
    } 
    mutex.unlock();
    interface->consoleAppendText(uniqueId, "Insufficient funds or no patients to transfer.");
}

void Ambulance::run() {
    interface->consoleAppendText(uniqueId, "[START] Ambulance routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        // Envoie un patient à l'hôpital
        sendPatient();

        // Simule un délai d'attente
        interface->simulateWork();

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }

    interface->consoleAppendText(uniqueId, "[STOP] Ambulance routine");
}

std::map<ItemType, int> Ambulance::getItemsForSale() {
    return stocks;
}

int Ambulance::getMaterialCost() {
    int totalCost = 0;
    for (const auto &item: resourcesSupplied) {
        totalCost += getCostPerUnit(item);
    }
    return totalCost;
}

int Ambulance::getAmountPaidToWorkers() {
    return nbTransfer * getEmployeeSalary(EmployeeType::Supplier);
}

int Ambulance::getNumberPatients() {
    return stocks[ItemType::PatientSick];
}

void Ambulance::setInterface(IWindowInterface *windowInterface) {
    interface = windowInterface;
}


void Ambulance::setHospitals(std::vector<Seller *> hospitals) {
    this->hospitals = hospitals;

    for (Seller *hospital: hospitals) {
        interface->setLink(uniqueId, hospital->getUniqueId());
    }
}

int Ambulance::send(ItemType it, int qty, int bill) {
    return 0;
}


int Ambulance::request(ItemType what, int qty) {
    return 0;
}

std::vector<ItemType> Ambulance::getResourcesSupplied() const {
    return resourcesSupplied;
}
