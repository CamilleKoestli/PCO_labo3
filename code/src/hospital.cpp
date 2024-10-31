#include "hospital.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

IWindowInterface* Hospital::interface = nullptr;

Hospital::Hospital(int uniqueId, int fund, int maxBeds)
    : Seller(fund, uniqueId), maxBeds(maxBeds), currentBeds(0), nbHospitalised(0), nbFree(0)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Hospital Created with " + QString::number(maxBeds) + " beds");
    
    std::vector<ItemType> initialStocks = { ItemType::PatientHealed, ItemType::PatientSick };

    for(const auto& item : initialStocks) {
        stocks[item] = 0;
    }
}

int Hospital::request(ItemType what, int qty){
    // TODO 

    return 0;
}

void Hospital::freeHealedPatient() {
    // TODO 


}

void Hospital::transferPatientsFromClinic() {
    // TODO
}


int Hospital::send(ItemType it, int qty, int bill) {
    // TODO
    // Si le patient est malade, vérification lit disponible et fonds suffisants
    if (it == ItemType::PatientSick) {
        if (currentBeds + qty <= maxBeds && money >= bill) {
            money -= bill;      
            currentBeds += qty;   
            mutex.unlock();
            return bill;         
        }
    } 
    // Si le patient est guéri, réduire les lits occupés
    else if (it == ItemType::PatientHealed) {
        if (currentBeds >= qty) { 
            currentBeds -= qty;   
            mutex.unlock();
            return bill;  
        }
    }
    
    mutex.unlock();
    return 0; // Échec de la transaction
}

void Hospital::run()
{
    if (clinics.empty()) {
        std::cerr << "You have to give clinics to a hospital before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Hospital routine");

    while (true /*TODO*/) {
        transferPatientsFromClinic();

        freeHealedPatient();

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        interface->simulateWork(); // Temps d'attente
    }

    interface->consoleAppendText(uniqueId, "[STOP] Hospital routine");
}

int Hospital::getAmountPaidToWorkers() {
    return nbHospitalised * getEmployeeSalary(EmployeeType::Nurse);
}

int Hospital::getNumberPatients(){
    return stocks[ItemType::PatientSick] + stocks[ItemType::PatientHealed] + nbFree;
}

std::map<ItemType, int> Hospital::getItemsForSale()
{
    return stocks;
}

void Hospital::setClinics(std::vector<Seller*> clinics){
    this->clinics = clinics;

    for (Seller* clinic : clinics) {
        interface->setLink(uniqueId, clinic->getUniqueId());
    }
}

void Hospital::setInterface(IWindowInterface* windowInterface){
    interface = windowInterface;
}
