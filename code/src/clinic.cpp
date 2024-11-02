#include "clinic.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

IWindowInterface *Clinic::interface = nullptr;

Clinic::Clinic(int uniqueId, int fund, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), nbTreated(0), resourcesNeeded(resourcesNeeded) {
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");

    for (const auto &item: resourcesNeeded) {
        stocks[item] = 0;
    }
}

bool Clinic::verifyResources() {
    for (auto item: resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }
    return true;
}

int Clinic::request(ItemType what, int qty) {
    // TODO à checker
    // Vérifie que le type de patient demandé est soigné
    if (what == ItemType::PatientHealed) {
        mutex.lock();

        if (stocks[what] >= qty) {
            stocks[what] -= qty;
            mutex.unlock();
            return qty;
        }
        mutex.unlock();
    }
    return 0;
}

void Clinic::treatPatient() {
    // TODO a checker
    int salary = getEmployeeSalary(EmployeeType::Doctor);

    if (money >= salary) {
        mutex.lock();

        bool hasAllRessources = true;
        for (auto &resource: resourcesNeeded) {
            if (stocks[resource] <= 0) {
                hasAllRessources = false;
                break;
            }
        }
        if (hasAllRessources) {
            for (const auto &resource: resourcesNeeded) {
                stocks[resource]--;
            }
            stocks[ItemType::PatientHealed]++;
            nbTreated++;
            money -= salary;

            mutex.unlock();

            interface->simulateWork();
            interface->consoleAppendText(uniqueId, "Clinic has healed a patient.");
        } else {
            mutex.unlock();

            interface->consoleAppendText(uniqueId, "Clinic lacks resources to treat a patient.");
        }
    } else {
        interface->consoleAppendText(uniqueId, "Clinic does not have enough funds to pay the doctor.");
    }
    interface->updateStock(uniqueId, &stocks);
    interface->updateFund(uniqueId, money);
}

void Clinic::orderResources() {
    // TODO a checker
    for (const auto &item: resourcesNeeded) {
        // Si la ressource manquant
        if (stocks[item] < 1 && money >= getCostPerUnit(item)) {
            Seller *supplier = chooseRandomSeller(suppliers);
            int cost = supplier->request(item, 1);

            if (cost > 0) {
                mutex.lock();
                stocks[item]++;
                money -= cost;
                mutex.unlock();

                interface->updateFund(uniqueId, money);
                interface->consoleAppendText(uniqueId, "Ordered resources from supplier");
            } else {
                interface->consoleAppendText(uniqueId, "Failed to order resources");
            }
        }
    }
}

void Clinic::run() {
    if (hospitals.empty() || suppliers.empty()) {
        std::cerr << "You have to assign hospitals and suppliers to run a clinic" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Clinic routine");

    // Boucle de routine, qui s'exécute tant que l'arrêt n'est pas demandé
    while (!PcoThread::thisThread()->stopRequested()) {
        // Vérifie si la clinique a les ressources nécessaires pour traiter un patient
        if (verifyResources()) {
            mutex.lock();
            treatPatient();
            mutex.unlock();
        } else {
            mutex.lock();
            orderResources();
            mutex.unlock();
        }

        // Simule un délai de travail
        interface->simulateWork();

        // Met à jour les fonds et les stocks sur l'interface
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }

    interface->consoleAppendText(uniqueId, "[STOP] Clinic routine");
}

void Clinic::setHospitalsAndSuppliers(std::vector<Seller *> hospitals,
                                      std::vector<Seller *> suppliers) {
    this->hospitals = hospitals;
    this->suppliers = suppliers;

    for (Seller *hospital: hospitals) {
        interface->setLink(uniqueId, hospital->getUniqueId());
    }
    for (Seller *supplier: suppliers) {
        interface->setLink(uniqueId, supplier->getUniqueId());
    }
}

int Clinic::getTreatmentCost() {
    return 0;
}

int Clinic::getWaitingPatients() {
    return stocks[ItemType::PatientSick];
}

int Clinic::getNumberPatients() {
    return stocks[ItemType::PatientSick] + stocks[ItemType::PatientHealed];
}

int Clinic::send(ItemType it, int qty, int bill) {
    return 0;
}

int Clinic::getAmountPaidToWorkers() {
    return nbTreated *
           getEmployeeSalary(getEmployeeThatProduces(ItemType::PatientHealed));
}

void Clinic::setInterface(IWindowInterface *windowInterface) {
    interface = windowInterface;
}

std::map<ItemType, int> Clinic::getItemsForSale() {
    return stocks;
}

Pulmonology::Pulmonology(int uniqueId, int fund)
    : Clinic::Clinic(
        uniqueId, fund,
        {ItemType::PatientSick, ItemType::Pill, ItemType::Thermometer}) {
}

Cardiology::Cardiology(int uniqueId, int fund)
    : Clinic::Clinic(
        uniqueId, fund,
        {ItemType::PatientSick, ItemType::Syringe, ItemType::Stethoscope}) {
}

Neurology::Neurology(int uniqueId, int fund)
    : Clinic::Clinic(
        uniqueId, fund,
        {ItemType::PatientSick, ItemType::Pill, ItemType::Scalpel}) {
}
