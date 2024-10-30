#include "ambulance.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>

IWindowInterface* Ambulance::interface = nullptr;

Ambulance::Ambulance(int uniqueId, int fund, std::vector<ItemType> resourcesSupplied, std::map<ItemType, int> initialStocks)
    : Seller(fund, uniqueId), resourcesSupplied(resourcesSupplied), nbTransfer(0) 
{
    interface->consoleAppendText(uniqueId, QString("Ambulance Created"));

    for (const auto& item : resourcesSupplied) {
        if (initialStocks.find(item) != initialStocks.end()) {
            stocks[item] = initialStocks[item];
        } else {
            stocks[item] = 0;
        }
    }

    interface->updateFund(uniqueId, fund);
}

void Ambulance::sendPatient(){
    // TODO a checker

    // On choisit un hopital de manière aléatoire
    Seller* hospital = chooseRandomSeller(hospitals);

    // Vérification si l'hôpital prend le patient
    int patientsToSend = std::min(stocks[ItemType::PatientSick], 1);
    if (patientsToSend > 0) {
        int bill = hospital->request(ItemType::PatientSick, patientsToSend);
        if (bill > 0) {
            // Acceptation du patient
            mutex.lock();
            stocks[ItemType::PatientSick] -= patientsToSend;
            money += bill;
            nbTransfer++;
            mutex.unlock();
            interface->consoleAppendText(uniqueId, "Patient sent to hospital.");
        } else {
            interface->consoleAppendText(uniqueId, "No hospital could accept the patient.");
        }
    }

}

void Ambulance::run() {
    interface->consoleAppendText(uniqueId, "[START] Ambulance routine");

    while (!PcoThread::thisThread()->stopRequested()/*true TODO*/) {

        // S'il y a des patients malades
        if (getNumberPatients() > 0) {  
            sendPatient();
        }
    
        sendPatient();
        
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
    for (const auto& item : resourcesSupplied) {
        totalCost += getCostPerUnit(item);
    }
    return totalCost;
}

int Ambulance::getAmountPaidToWorkers() {
    return nbTransfer * getEmployeeSalary(EmployeeType::Supplier);
}

int Ambulance::getNumberPatients(){
    return stocks[ItemType::PatientSick];
}

void Ambulance::setInterface(IWindowInterface *windowInterface) {
    interface = windowInterface;
}


void Ambulance::setHospitals(std::vector<Seller*> hospitals){
    this->hospitals = hospitals;

    for (Seller* hospital : hospitals) {
        interface->setLink(uniqueId, hospital->getUniqueId());
    }
}

int Ambulance::send(ItemType it, int qty, int bill) {
    return 0;
}


int Ambulance::request(ItemType what, int qty){
    return 0;
}

std::vector<ItemType> Ambulance::getResourcesSupplied() const
{
    return resourcesSupplied;
}
