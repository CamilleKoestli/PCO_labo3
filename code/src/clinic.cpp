#include "clinic.h"
#include "costs.h"
#include "seller.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

IWindowInterface *Clinic::interface = nullptr;

Clinic::Clinic(int uniqueId, int fund, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), nbTreated(0), resourcesNeeded(resourcesNeeded) {
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Clinic created");

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
    // TODO
    int bill = getEmployeeSalary(getEmployeeThatProduces(what));

    if (bill > 0) {
        mutex.lock();
        if (stocks[what] > 0) {
            stocks[what] -= qty;
            money += bill;
            mutex.unlock();
            return 1;
        }
        mutex.unlock();
    }
    return 0;
}

void Clinic::treatPatient() {
    // TODO
    int salaryDoctor = getEmployeeSalary(EmployeeType::Doctor);

    mutex.lock();
    if (money - salaryDoctor >= 0) {
        bool hasAllRessources = true;
        for (auto &resource: resourcesNeeded) {
            if (stocks[resource] <= 0) {
                hasAllRessources = false;
                break;
            }
        }
        if (hasAllRessources) {
            for (const auto &resource: resourcesNeeded) {
                --stocks[resource];
            }
            ++stocks[ItemType::PatientHealed];
            ++nbTreated;
            money -= salaryDoctor;

            interface->simulateWork();
            interface->consoleAppendText(uniqueId, "Clinic has healed a patient.");
        } else {
            interface->consoleAppendText(uniqueId, "Clinic lacks resources to treat a patient.");
        }
    } else {
        interface->consoleAppendText(uniqueId, "Clinic does not have enough funds to pay the doctor.");
    }
    interface->updateStock(uniqueId, &stocks);
    interface->updateFund(uniqueId, money);
    mutex.unlock();
}

void Clinic::orderResources() {
    // TODO
    int qty = 1; // Quantité par ressource commandée

    for (const auto& resource : resourcesNeeded) {
        // Vérifie que la clinique dispose des fonds nécessaires
        int cost = qty * getCostPerUnit(resource);
        if (cost > money) {
            continue; // Passe à la ressource suivante si les fonds sont insuffisants
        }

        int bill = 0;
        if (resource == ItemType::PatientSick) {
            // Demande de patients malades aux hôpitaux
            bill = chooseRandomSeller(hospitals)->request(resource, qty);
        } else if (resource != ItemType::PatientHealed) {
            // Demande d'autres ressources aux fournisseurs, sauf les patients guéris
            bill = chooseRandomSeller(suppliers)->request(resource, qty);
        }

        // Met à jour les stocks et fonds en cas de succès de la demande
        if (bill > 0) {
            std::scoped_lock lock(mutex); // Gestion automatique du déverrouillage du mutex
            money -= bill;
            stocks[resource] += qty;
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
            // Traite un patient
            treatPatient();
            
        } else {
            // Sinon, commande des ressources
            orderResources();
          
        }

        // Simule un délai d'attente
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
